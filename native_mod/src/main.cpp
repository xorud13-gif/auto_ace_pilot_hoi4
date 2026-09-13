#include <windows.h>
#include "proxy.hpp"
#include "ace_assigner.hpp"

DWORD WINAPI BackgroundInitThread(LPVOID) {
    // Wait a brief moment for process modules to be fully loaded
    Sleep(500);

    // Check if running inside hoi4.exe
    char modulePath[MAX_PATH];
    GetModuleFileNameA(NULL, modulePath, MAX_PATH);

    // Case-insensitive check for hoi4
    if (strstr(modulePath, "hoi4") != nullptr || strstr(modulePath, "HOI4") != nullptr) {
        AceAssigner::Initialize();
    }
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        InitProxy();
        CreateThread(nullptr, 0, BackgroundInitThread, nullptr, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        if (!lpvReserved) {
            AceAssigner::Shutdown();
            FreeProxy();
        }
        break;
    }
    return TRUE;
}
