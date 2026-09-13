#pragma once
#include <windows.h>
#include <cstdint>

class X64Hook {
public:
    X64Hook() : m_pTarget(nullptr), m_pTrampoline(nullptr), m_bHooked(false) {}
    ~X64Hook() { Unhook(); }

    bool Hook(uintptr_t targetAddr, uintptr_t hookFunc, size_t stolenLen = 16);
    void Unhook();

    template<typename T>
    T GetOriginal() const {
        return reinterpret_cast<T>(m_pTrampoline);
    }

private:
    uint8_t* m_pTarget;
    uint8_t* m_pTrampoline;
    uint8_t m_origBytes[32];
    size_t m_stolenLen;
    bool m_bHooked;
};
