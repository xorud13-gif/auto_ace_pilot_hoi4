#include "hook.hpp"
#include <cstring>

bool X64Hook::Hook(uintptr_t targetAddr, uintptr_t hookFunc, size_t stolenLen) {
    if (m_bHooked || stolenLen < 14 || stolenLen > 32) return false;

    m_pTarget = reinterpret_cast<uint8_t*>(targetAddr);
    m_stolenLen = stolenLen;

    // Allocate trampoline buffer (executable)
    m_pTrampoline = reinterpret_cast<uint8_t*>(VirtualAlloc(nullptr, stolenLen + 14, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!m_pTrampoline) return false;

    // Backup original bytes
    memcpy(m_origBytes, m_pTarget, stolenLen);

    // Fill trampoline: stolen bytes + jmp back to targetAddr + stolenLen
    memcpy(m_pTrampoline, m_pTarget, stolenLen);
    uint8_t* pTrampolineJmp = m_pTrampoline + stolenLen;
    pTrampolineJmp[0] = 0xFF;
    pTrampolineJmp[1] = 0x25;
    *reinterpret_cast<int32_t*>(pTrampolineJmp + 2) = 0; // RIP + 0
    *reinterpret_cast<uintptr_t*>(pTrampolineJmp + 6) = targetAddr + stolenLen;

    // Write hook: jmp qword ptr [rip + 0] to hookFunc
    DWORD oldProtect;
    if (!VirtualProtect(m_pTarget, stolenLen, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(m_pTrampoline, 0, MEM_RELEASE);
        m_pTrampoline = nullptr;
        return false;
    }

    m_pTarget[0] = 0xFF;
    m_pTarget[1] = 0x25;
    *reinterpret_cast<int32_t*>(m_pTarget + 2) = 0; // RIP + 0
    *reinterpret_cast<uintptr_t*>(m_pTarget + 6) = hookFunc;

    // Fill remainder with NOP
    for (size_t i = 14; i < stolenLen; ++i) {
        m_pTarget[i] = 0x90;
    }

    VirtualProtect(m_pTarget, stolenLen, oldProtect, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), m_pTarget, stolenLen);
    FlushInstructionCache(GetCurrentProcess(), m_pTrampoline, stolenLen + 14);

    m_bHooked = true;
    return true;
}

void X64Hook::Unhook() {
    if (!m_bHooked) return;

    DWORD oldProtect;
    if (VirtualProtect(m_pTarget, m_stolenLen, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memcpy(m_pTarget, m_origBytes, m_stolenLen);
        VirtualProtect(m_pTarget, m_stolenLen, oldProtect, &oldProtect);
        FlushInstructionCache(GetCurrentProcess(), m_pTarget, m_stolenLen);
    }

    if (m_pTrampoline) {
        VirtualFree(m_pTrampoline, 0, MEM_RELEASE);
        m_pTrampoline = nullptr;
    }

    m_bHooked = false;
}
