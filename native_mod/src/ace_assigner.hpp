#pragma once
#include <windows.h>
#include <string>

namespace AceAssigner {
    bool Initialize();
    void Shutdown();
    void TriggerAutoAssign();
    void Log(const char* format, ...);
}
