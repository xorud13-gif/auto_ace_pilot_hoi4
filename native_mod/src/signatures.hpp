#pragma once
#include <cstdint>

// ============================================================================
// Hearts of Iron IV Native Mod - Engine Signatures & Offsets Configuration
// ============================================================================
// When a new HoI4 patch or DLC is released:
// Run 'python tools/patch_verifier.py' to automatically verify or update these values!
// ============================================================================

namespace Signatures {

    // Target game version
    constexpr const char* TARGET_GAME_VERSION = "v1.19.2.0";

    // 1. AssignAces: Assigns reserve aces to vacant air wings
    constexpr const char* PAT_ASSIGN_ACES = 
        "\x48\x89\x4C\x24\x08\x53\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x68\x0F\x29\x74\x24\x50\x48\x8B\xD9";
    constexpr const char* MASK_ASSIGN_ACES = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    constexpr uintptr_t RVA_ASSIGN_ACES = 0x12AF5C0;

    // 2. Country_GetAirManager: Obtains CAirManager pointer from CCountry
    constexpr const char* PAT_GET_AIR_MANAGER = 
        "\x40\x53\x48\x83\xEC\x30\x48\x8B\xD9\x48\x8B\x49\x08\x48\x85\xC9\x75\x08\x33\xC0\x48\x83\xC4\x30\x5B\xC3\x80\x3D\xE1";
    constexpr const char* MASK_GET_AIR_MANAGER = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    constexpr uintptr_t RVA_GET_AIR_MANAGER = 0x02A8CF0;
    constexpr size_t HOOK_SIZE_GET_AIR_MANAGER = 16;

    // 3. LogDispatcher: Intercepts script log effects on the main game thread
    constexpr const char* PAT_LOG_DISPATCHER = 
        "\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x20\x57\x48\x81\xEC\x90\x00\x00\x00";
    constexpr const char* MASK_LOG_DISPATCHER = "xxxxxxxxxxxxxxxxxx";
    constexpr uintptr_t RVA_LOG_DISPATCHER = 0x24ACD40;
    constexpr size_t HOOK_SIZE_LOG_DISPATCHER = 18;

    // 4. Struct Offsets
    // CAirManager offset for count of unassigned reserve aces (int32)
    constexpr uintptr_t OFFSET_AIRMGR_UNASSIGNED_COUNT = 0x12E4;

}
