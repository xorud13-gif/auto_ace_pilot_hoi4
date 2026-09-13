#include "scanner.hpp"
#include <psapi.h>

uintptr_t FindPatternInRange(uintptr_t start, size_t size, const char* pattern, const char* mask) {
    size_t patternLen = 0;
    while (mask[patternLen]) patternLen++;

    const uint8_t* pStart = reinterpret_cast<const uint8_t*>(start);
    const uint8_t* pEnd = pStart + size - patternLen;

    for (const uint8_t* pCur = pStart; pCur < pEnd; ++pCur) {
        bool found = true;
        for (size_t i = 0; i < patternLen; ++i) {
            if (mask[i] != '?' && pCur[i] != static_cast<uint8_t>(pattern[i])) {
                found = false;
                break;
            }
        }
        if (found) {
            return reinterpret_cast<uintptr_t>(pCur);
        }
    }
    return 0;
}

uintptr_t FindPattern(const char* moduleName, const char* pattern, const char* mask) {
    HMODULE hMod = moduleName ? GetModuleHandleA(moduleName) : GetModuleHandleA(NULL);
    if (!hMod) return 0;

    MODULEINFO modInfo = { 0 };
    if (!GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(modInfo))) {
        return 0;
    }

    return FindPatternInRange(reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll), modInfo.SizeOfImage, pattern, mask);
}
