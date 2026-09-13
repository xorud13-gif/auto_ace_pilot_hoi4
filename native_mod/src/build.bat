@echo off
setlocal
echo Compiling AceAutoAssigner (version.dll) with Zig C++...

..\..\tools\zig\zig.exe c++ -shared -target x86_64-windows -O3 -o ..\version.dll ^
    main.cpp proxy.cpp scanner.cpp hook.cpp ace_assigner.cpp ^
    proxy.def -lpsapi

if %ERRORLEVEL% equ 0 (
    copy /y ..\version.dll ..\..\version.dll > nul
    del ..\version.dll
    echo [SUCCESS] version.dll built successfully in Hearts of Iron IV root directory!
) else (
    echo [ERROR] Build failed with error code %ERRORLEVEL%
)
