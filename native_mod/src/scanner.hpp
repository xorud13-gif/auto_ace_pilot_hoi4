#pragma once
#include <windows.h>
#include <cstdint>

uintptr_t FindPattern(const char* moduleName, const char* pattern, const char* mask);
uintptr_t FindPatternInRange(uintptr_t start, size_t size, const char* pattern, const char* mask);
