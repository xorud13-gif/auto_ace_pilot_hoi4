@echo off
setlocal
echo Compiling AceAutoAssigner (version.dll) with Zig C++...

..\..\tools\zig\zig.exe c++ -shared -target x86_64-windows -O3 -o ..\version.dll ^
    main.cpp proxy.cpp scanner.cpp hook.cpp ace_assigner.cpp ^
    proxy.def -lpsapi

if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] version.dll built successfully in Hearts of Iron IV directory!
) else (
    echo [ERROR] Build failed with error code %ERRORLEVEL%
)
