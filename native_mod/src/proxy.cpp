#include "proxy.hpp"
#include <cstdio>

static HMODULE g_hOriginalDll = nullptr;

typedef BOOL(WINAPI* PFN_GetFileVersionInfoA)(LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoByHandle)(int, HANDLE, LPVOID, int*);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoExA)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoExW)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeExA)(DWORD, LPCSTR, LPDWORD);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeExW)(DWORD, LPCWSTR, LPDWORD);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD(WINAPI* PFN_VerFindFileA)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT, LPSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerFindFileW)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerInstallFileA)(DWORD, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerInstallFileW)(DWORD, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerLanguageNameA)(DWORD, LPSTR, DWORD);
typedef DWORD(WINAPI* PFN_VerLanguageNameW)(DWORD, LPWSTR, DWORD);
typedef BOOL(WINAPI* PFN_VerQueryValueA)(LPCVOID, LPCSTR, LPVOID*, PUINT);
typedef BOOL(WINAPI* PFN_VerQueryValueW)(LPCVOID, LPCWSTR, LPVOID*, PUINT);

static PFN_GetFileVersionInfoA Orig_GetFileVersionInfoA = nullptr;
static PFN_GetFileVersionInfoByHandle Orig_GetFileVersionInfoByHandle = nullptr;
static PFN_GetFileVersionInfoExA Orig_GetFileVersionInfoExA = nullptr;
static PFN_GetFileVersionInfoExW Orig_GetFileVersionInfoExW = nullptr;
static PFN_GetFileVersionInfoSizeA Orig_GetFileVersionInfoSizeA = nullptr;
static PFN_GetFileVersionInfoSizeExA Orig_GetFileVersionInfoSizeExA = nullptr;
static PFN_GetFileVersionInfoSizeExW Orig_GetFileVersionInfoSizeExW = nullptr;
static PFN_GetFileVersionInfoSizeW Orig_GetFileVersionInfoSizeW = nullptr;
static PFN_GetFileVersionInfoW Orig_GetFileVersionInfoW = nullptr;
static PFN_VerFindFileA Orig_VerFindFileA = nullptr;
static PFN_VerFindFileW Orig_VerFindFileW = nullptr;
static PFN_VerInstallFileA Orig_VerInstallFileA = nullptr;
static PFN_VerInstallFileW Orig_VerInstallFileW = nullptr;
static PFN_VerLanguageNameA Orig_VerLanguageNameA = nullptr;
static PFN_VerLanguageNameW Orig_VerLanguageNameW = nullptr;
static PFN_VerQueryValueA Orig_VerQueryValueA = nullptr;
static PFN_VerQueryValueW Orig_VerQueryValueW = nullptr;

bool InitProxy() {
    if (g_hOriginalDll) return true;

    char sysPath[MAX_PATH];
    GetSystemDirectoryA(sysPath, MAX_PATH);
    strcat_s(sysPath, "\\version.dll");

    g_hOriginalDll = LoadLibraryA(sysPath);
    if (!g_hOriginalDll) {
        return false;
    }

    Orig_GetFileVersionInfoA = (PFN_GetFileVersionInfoA)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoA");
    Orig_GetFileVersionInfoByHandle = (PFN_GetFileVersionInfoByHandle)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoByHandle");
    Orig_GetFileVersionInfoExA = (PFN_GetFileVersionInfoExA)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoExA");
    Orig_GetFileVersionInfoExW = (PFN_GetFileVersionInfoExW)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoExW");
    Orig_GetFileVersionInfoSizeA = (PFN_GetFileVersionInfoSizeA)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoSizeA");
    Orig_GetFileVersionInfoSizeExA = (PFN_GetFileVersionInfoSizeExA)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoSizeExA");
    Orig_GetFileVersionInfoSizeExW = (PFN_GetFileVersionInfoSizeExW)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoSizeExW");
    Orig_GetFileVersionInfoSizeW = (PFN_GetFileVersionInfoSizeW)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoSizeW");
    Orig_GetFileVersionInfoW = (PFN_GetFileVersionInfoW)GetProcAddress(g_hOriginalDll, "GetFileVersionInfoW");
    Orig_VerFindFileA = (PFN_VerFindFileA)GetProcAddress(g_hOriginalDll, "VerFindFileA");
    Orig_VerFindFileW = (PFN_VerFindFileW)GetProcAddress(g_hOriginalDll, "VerFindFileW");
    Orig_VerInstallFileA = (PFN_VerInstallFileA)GetProcAddress(g_hOriginalDll, "VerInstallFileA");
    Orig_VerInstallFileW = (PFN_VerInstallFileW)GetProcAddress(g_hOriginalDll, "VerInstallFileW");
    Orig_VerLanguageNameA = (PFN_VerLanguageNameA)GetProcAddress(g_hOriginalDll, "VerLanguageNameA");
    Orig_VerLanguageNameW = (PFN_VerLanguageNameW)GetProcAddress(g_hOriginalDll, "VerLanguageNameW");
    Orig_VerQueryValueA = (PFN_VerQueryValueA)GetProcAddress(g_hOriginalDll, "VerQueryValueA");
    Orig_VerQueryValueW = (PFN_VerQueryValueW)GetProcAddress(g_hOriginalDll, "VerQueryValueW");

    return true;
}

void FreeProxy() {
    if (g_hOriginalDll) {
        FreeLibrary(g_hOriginalDll);
        g_hOriginalDll = nullptr;
    }
}

extern "C" {

BOOL WINAPI Proxy_GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!Orig_GetFileVersionInfoA) InitProxy();
    return Orig_GetFileVersionInfoA ? Orig_GetFileVersionInfoA(lptstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

BOOL WINAPI Proxy_GetFileVersionInfoByHandle(int a1, HANDLE a2, LPVOID a3, int* a4) {
    if (!Orig_GetFileVersionInfoByHandle) InitProxy();
    return Orig_GetFileVersionInfoByHandle ? Orig_GetFileVersionInfoByHandle(a1, a2, a3, a4) : FALSE;
}

BOOL WINAPI Proxy_GetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!Orig_GetFileVersionInfoExA) InitProxy();
    return Orig_GetFileVersionInfoExA ? Orig_GetFileVersionInfoExA(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

BOOL WINAPI Proxy_GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!Orig_GetFileVersionInfoExW) InitProxy();
    return Orig_GetFileVersionInfoExW ? Orig_GetFileVersionInfoExW(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

DWORD WINAPI Proxy_GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
    if (!Orig_GetFileVersionInfoSizeA) InitProxy();
    return Orig_GetFileVersionInfoSizeA ? Orig_GetFileVersionInfoSizeA(lptstrFilename, lpdwHandle) : 0;
}

DWORD WINAPI Proxy_GetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle) {
    if (!Orig_GetFileVersionInfoSizeExA) InitProxy();
    return Orig_GetFileVersionInfoSizeExA ? Orig_GetFileVersionInfoSizeExA(dwFlags, lpwstrFilename, lpdwHandle) : 0;
}

DWORD WINAPI Proxy_GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    if (!Orig_GetFileVersionInfoSizeExW) InitProxy();
    return Orig_GetFileVersionInfoSizeExW ? Orig_GetFileVersionInfoSizeExW(dwFlags, lpwstrFilename, lpdwHandle) : 0;
}

DWORD WINAPI Proxy_GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle) {
    if (!Orig_GetFileVersionInfoSizeW) InitProxy();
    return Orig_GetFileVersionInfoSizeW ? Orig_GetFileVersionInfoSizeW(lptstrFilename, lpdwHandle) : 0;
}

BOOL WINAPI Proxy_GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!Orig_GetFileVersionInfoW) InitProxy();
    return Orig_GetFileVersionInfoW ? Orig_GetFileVersionInfoW(lptstrFilename, dwHandle, dwLen, lpData) : FALSE;
}

DWORD WINAPI Proxy_VerFindFileA(DWORD uFlags, LPCSTR szFileName, LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen) {
    if (!Orig_VerFindFileA) InitProxy();
    return Orig_VerFindFileA ? Orig_VerFindFileA(uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen) : 0;
}

DWORD WINAPI Proxy_VerFindFileW(DWORD uFlags, LPCWSTR szFileName, LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen) {
    if (!Orig_VerFindFileW) InitProxy();
    return Orig_VerFindFileW ? Orig_VerFindFileW(uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen) : 0;
}

DWORD WINAPI Proxy_VerInstallFileA(DWORD uFlags, LPCSTR szSrcFileName, LPCSTR szDestFileName, LPCSTR szSrcDir, LPCSTR szDestDir, LPCSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen) {
    if (!Orig_VerInstallFileA) InitProxy();
    return Orig_VerInstallFileA ? Orig_VerInstallFileA(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen) : 0;
}

DWORD WINAPI Proxy_VerInstallFileW(DWORD uFlags, LPCWSTR szSrcFileName, LPCWSTR szDestFileName, LPCWSTR szSrcDir, LPCWSTR szDestDir, LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen) {
    if (!Orig_VerInstallFileW) InitProxy();
    return Orig_VerInstallFileW ? Orig_VerInstallFileW(uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen) : 0;
}

DWORD WINAPI Proxy_VerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD nSize) {
    if (!Orig_VerLanguageNameA) InitProxy();
    return Orig_VerLanguageNameA ? Orig_VerLanguageNameA(wLang, szLang, nSize) : 0;
}

DWORD WINAPI Proxy_VerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD nSize) {
    if (!Orig_VerLanguageNameW) InitProxy();
    return Orig_VerLanguageNameW ? Orig_VerLanguageNameW(wLang, szLang, nSize) : 0;
}

BOOL WINAPI Proxy_VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    if (!Orig_VerQueryValueA) InitProxy();
    return Orig_VerQueryValueA ? Orig_VerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen) : FALSE;
}

BOOL WINAPI Proxy_VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    if (!Orig_VerQueryValueW) InitProxy();
    return Orig_VerQueryValueW ? Orig_VerQueryValueW(pBlock, lpSubBlock, lplpBuffer, puLen) : FALSE;
}

}
