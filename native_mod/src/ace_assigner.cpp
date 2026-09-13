#include "ace_assigner.hpp"
#include "scanner.hpp"
#include "hook.hpp"
#include "signatures.hpp"
#include <cstdio>
#include <cstdarg>
#include <atomic>

namespace AceAssigner {

typedef void(*PFN_AssignAces)(void* pAirManager);
typedef void*(*PFN_Country_GetAirManager)(void* pCountry);
typedef void*(*PFN_LogDispatcher)(void* pLogger, const void* pStr);

static PFN_AssignAces fnAssignAces = nullptr;
static X64Hook g_hookGetAirManager;
static X64Hook g_hookLogDispatcher;

// Track active AirManagers seen in the game
static const int MAX_AIR_MANAGERS = 512;
static void* g_airManagers[MAX_AIR_MANAGERS];
static int g_airManagerCount = 0;
static void* g_pLastAirManager = nullptr;
static CRITICAL_SECTION g_csList;

// Configuration flags
// Default state is ON as requested by user
static std::atomic<bool> g_bPeriodicEnabled(true);

// Safe non-locking logger
void Log(const char* format, ...) {
    FILE* f = nullptr;
    if (fopen_s(&f, "ace_assigner.log", "a") != 0 || !f) {
        return;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
    fprintf(f, "[%02d:%02d:%02d.%03d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);

    fprintf(f, "\n");
    fclose(f);
}

// MSVC std::string helper
static inline const char* GetMsvcString(const void* pStr) {
    if (!pStr || IsBadReadPtr(pStr, 0x20)) return nullptr;
    const size_t capacity = *reinterpret_cast<const size_t*>(reinterpret_cast<const uint8_t*>(pStr) + 0x18);
    const size_t size = *reinterpret_cast<const size_t*>(reinterpret_cast<const uint8_t*>(pStr) + 0x10);
    if (size == 0 || size > 10000) return nullptr;

    const char* pText = nullptr;
    if (capacity <= 15) {
        pText = reinterpret_cast<const char*>(pStr);
    } else {
        pText = *reinterpret_cast<const char* const*>(pStr);
    }
    if (!pText || IsBadReadPtr(pText, 1)) return nullptr;
    return pText;
}

void TriggerAutoAssign(bool isManual = false) {
    // If periodic is disabled and this is not a manual 1-time request, do NOT run!
    if (!isManual && !g_bPeriodicEnabled.load()) {
        return;
    }

    EnterCriticalSection(&g_csList);
    for (int i = 0; i < g_airManagerCount; ++i) {
        void* mgr = g_airManagers[i];
        if (fnAssignAces && mgr) {
            // Read unassigned reserve ace count using configured struct offset
            int unassignedBefore = 0;
            void* pCount = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mgr) + Signatures::OFFSET_AIRMGR_UNASSIGNED_COUNT);
            if (!IsBadReadPtr(pCount, sizeof(int))) {
                unassignedBefore = *reinterpret_cast<int*>(pCount);
            }

            if (unassignedBefore > 0) {
                fnAssignAces(mgr);

                int unassignedAfter = unassignedBefore;
                if (!IsBadReadPtr(pCount, sizeof(int))) {
                    unassignedAfter = *reinterpret_cast<int*>(pCount);
                }

                int assigned = unassignedBefore - unassignedAfter;
                if (assigned > 0) {
                    Log("[AUTO-ASSIGN] Successfully replaced/assigned %d ace(s)! (Reserve pool: %d -> %d)",
                        assigned, unassignedBefore, unassignedAfter);
                } else {
                    Log("[AUTO-ASSIGN] Evaluated %d reserve ace(s), but no compatible vacant wing found.", unassignedBefore);
                }
            } else if (isManual) {
                Log("[AUTO-ASSIGN] Reserve ace pool is empty (0 available).");
            }
        }
    }
    LeaveCriticalSection(&g_csList);
}

// Hook for Country->GetAirManager
static void* Hooked_Country_GetAirManager(void* pCountry) {
    auto orig = g_hookGetAirManager.GetOriginal<PFN_Country_GetAirManager>();
    void* pAirMgr = orig(pCountry);

    if (pAirMgr) {
        g_pLastAirManager = pAirMgr;

        EnterCriticalSection(&g_csList);
        bool exists = false;
        for (int i = 0; i < g_airManagerCount; ++i) {
            if (g_airManagers[i] == pAirMgr) {
                exists = true;
                break;
            }
        }
        if (!exists && g_airManagerCount < MAX_AIR_MANAGERS) {
            g_airManagers[g_airManagerCount++] = pAirMgr;
            Log("[AIR_MGR] Registered AirManager #%d: %p", g_airManagerCount, pAirMgr);
        }
        LeaveCriticalSection(&g_csList);
    }

    return pAirMgr;
}

// Hook for LogDispatcher (central logger in hoi4.exe)
// Catches log effects triggered from Decisions, On-Actions & Events on the MAIN GAME THREAD
static void* Hooked_LogDispatcher(void* pLogger, const void* pStr) {
    if (pStr) {
        const char* pText = GetMsvcString(pStr);
        if (pText) {
            if (strstr(pText, "ACE_AUTO_ASSIGN:TRIGGER")) {
                Log("[DECISION] 'Immediate Ace Auto-Assign (1-Time)' clicked!");
                TriggerAutoAssign(true);
            } else if (strstr(pText, "ACE_AUTO_ASSIGN:ACE_DIED")) {
                Log("[EVENT] Ace pilot died in combat/accident! Automatically replacing from reserve pool...");
                TriggerAutoAssign(false);
            } else if (strstr(pText, "ACE_AUTO_ASSIGN:PROMOTED")) {
                Log("[EVENT] New Ace pilot promoted! Automatically assigning to vacant wing...");
                TriggerAutoAssign(false);
            } else if (strstr(pText, "ACE_AUTO_ASSIGN:DAILY")) {
                TriggerAutoAssign(false);
            } else if (strstr(pText, "ACE_AUTO_ASSIGN:DISABLE")) {
                Log("[DECISION] 'Disable Periodic Ace Auto-Assign' clicked! Auto-assignment paused.");
                g_bPeriodicEnabled.store(false);
            } else if (strstr(pText, "ACE_AUTO_ASSIGN:ENABLE")) {
                Log("[DECISION] 'Enable Periodic Ace Auto-Assign' clicked! Auto-assignment resumed.");
                g_bPeriodicEnabled.store(true);
                TriggerAutoAssign(false);
            } else if (strstr(pText, "Resetting game") || strstr(pText, "Launching SINGLEPLAYER")) {
                EnterCriticalSection(&g_csList);
                g_airManagerCount = 0;
                g_pLastAirManager = nullptr;
                LeaveCriticalSection(&g_csList);
                Log("[SESSION] Game reset detected. Cached AirManagers refreshed.");
            }
        }
    }

    auto orig = g_hookLogDispatcher.GetOriginal<PFN_LogDispatcher>();
    return orig(pLogger, pStr);
}

bool Initialize() {
    InitializeCriticalSection(&g_csList);

    Log("=================================================");
    Log("Hearts of Iron IV AceAutoAssigner Native Mod v2.1");
    Log("Features: Instant Death Replacement + Promotion Auto-Assign + Daily Main-Thread Pulse");
    Log("In-Game Decisions Mode: Controlled via Decisions & Events!");
    Log("Default State: Periodic Auto-Assign ON (Main Thread Synchronous)");
    Log("Target version: %s", Signatures::TARGET_GAME_VERSION);
    Log("=================================================");

    uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
    Log("hoi4.exe ImageBase: 0x%p", (void*)base);

    // 1. Scan for AssignAces
    uintptr_t addrAssign = FindPattern(NULL, Signatures::PAT_ASSIGN_ACES, Signatures::MASK_ASSIGN_ACES);
    if (!addrAssign) {
        Log("AssignAces signature scan failed. Falling back to default RVA 0x%llX...", (unsigned long long)Signatures::RVA_ASSIGN_ACES);
        addrAssign = base + Signatures::RVA_ASSIGN_ACES;
    }
    fnAssignAces = reinterpret_cast<PFN_AssignAces>(addrAssign);
    Log("Found AssignAces function at: 0x%p", (void*)addrAssign);

    // 2. Scan for Country_GetAirManager
    uintptr_t addrGetAir = FindPattern(NULL, Signatures::PAT_GET_AIR_MANAGER, Signatures::MASK_GET_AIR_MANAGER);
    if (!addrGetAir) {
        Log("Country_GetAirManager signature scan failed. Falling back to default RVA 0x%llX...", (unsigned long long)Signatures::RVA_GET_AIR_MANAGER);
        addrGetAir = base + Signatures::RVA_GET_AIR_MANAGER;
    }
    Log("Found Country_GetAirManager at: 0x%p", (void*)addrGetAir);

    // 3. Scan for LogDispatcher
    uintptr_t addrLog = FindPattern(NULL, Signatures::PAT_LOG_DISPATCHER, Signatures::MASK_LOG_DISPATCHER);
    if (!addrLog) {
        Log("LogDispatcher signature scan failed. Falling back to default RVA 0x%llX...", (unsigned long long)Signatures::RVA_LOG_DISPATCHER);
        addrLog = base + Signatures::RVA_LOG_DISPATCHER;
    }
    Log("Found LogDispatcher at: 0x%p", (void*)addrLog);

    // 4. Install hooks
    if (g_hookGetAirManager.Hook(addrGetAir, reinterpret_cast<uintptr_t>(Hooked_Country_GetAirManager), Signatures::HOOK_SIZE_GET_AIR_MANAGER)) {
        Log("Successfully installed hook on Country_GetAirManager!");
    } else {
        Log("WARNING: Failed to hook Country_GetAirManager.");
    }

    if (g_hookLogDispatcher.Hook(addrLog, reinterpret_cast<uintptr_t>(Hooked_LogDispatcher), Signatures::HOOK_SIZE_LOG_DISPATCHER)) {
        Log("Successfully installed hook on LogDispatcher!");
    } else {
        Log("WARNING: Failed to hook LogDispatcher.");
    }

    Log("AceAutoAssigner v2.1 ready! Main-thread auto-replace & daily pulse active.");
    return true;
}

void Shutdown() {
    Log("Shutting down AceAutoAssigner...");
    g_hookGetAirManager.Unhook();
    g_hookLogDispatcher.Unhook();
    DeleteCriticalSection(&g_csList);
    Log("AceAutoAssigner unloaded cleanly.");
}

}
