@echo off
setlocal enabledelayedexpansion
chcp 65001 > nul

echo =========================================================
echo Hearts of Iron IV - AceAutoAssigner Steam 배포 유틸리티
echo =========================================================

set "STEAM_DIR="

:: 1. 윈도우 레지스트리에서 스팀 HoI4 설치 경로 자동 검색 (Steam App ID: 394360)
for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 394360" /v InstallLocation 2^>nul ^| find "InstallLocation"') do (
    set "STEAM_DIR=%%b"
)

:: 2. 64비트 WOW6432Node 레지스트리 보조 검색
if not defined STEAM_DIR (
    for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 394360" /v InstallLocation 2^>nul ^| find "InstallLocation"') do (
        set "STEAM_DIR=%%b"
    )
)

:: 3. 기본 스팀 경로 후보 확인
if not defined STEAM_DIR (
    if exist "C:\Program Files (x86)\Steam\steamapps\common\Hearts of Iron IV\hoi4.exe" (
        set "STEAM_DIR=C:\Program Files (x86)\Steam\steamapps\common\Hearts of Iron IV"
    ) else if exist "C:\Steam\steamapps\common\Hearts of Iron IV\hoi4.exe" (
        set "STEAM_DIR=C:\Steam\steamapps\common\Hearts of Iron IV"
    ) else if exist "D:\Steam\steamapps\common\Hearts of Iron IV\hoi4.exe" (
        set "STEAM_DIR=D:\Steam\steamapps\common\Hearts of Iron IV"
    ) else if exist "D:\SteamLibrary\steamapps\common\Hearts of Iron IV\hoi4.exe" (
        set "STEAM_DIR=D:\SteamLibrary\steamapps\common\Hearts of Iron IV"
    )
)

:: 4. 자동 감지 결과 확인 및 수동 입력 폴백
if defined STEAM_DIR if exist "!STEAM_DIR!\hoi4.exe" (
    echo [안내] 스팀 설치 폴더가 자동으로 감지되었습니다:
    echo        !STEAM_DIR!
    echo.
    goto :DEPLOY
)

echo [안내] 스팀 본편 폴더를 자동으로 감지하지 못했습니다.
echo Hearts of Iron IV가 설치된 폴더 경로를 입력해 주세요.
echo (폴더를 이 콘솔 창에 드래그 앤 드롭하셔도 됩니다.)
echo.
set /p "STEAM_DIR=설치 폴더 경로 입력: "
if defined STEAM_DIR set "STEAM_DIR=!STEAM_DIR:"=!"

if not exist "!STEAM_DIR!\hoi4.exe" (
    echo.
    echo [ERROR] 지정된 경로에서 hoi4.exe를 찾을 수 없습니다:
    echo         !STEAM_DIR!
    echo 경로를 다시 확인하고 실행해 주세요.
    pause
    exit /b 1
)

:DEPLOY
echo [1/3] version.dll 배포 중...
copy /y "version.dll" "!STEAM_DIR!\version.dll" > nul
if !ERRORLEVEL! neq 0 (
    echo [ERROR] version.dll 복사 실패! 게임이 실행 중인지 확인하세요.
    pause
    exit /b 1
)

echo [2/3] 결정 및 온액션 스크립트 배포 중...
if not exist "!STEAM_DIR!\common\decisions\categories" mkdir "!STEAM_DIR!\common\decisions\categories"
if not exist "!STEAM_DIR!\common\on_actions" mkdir "!STEAM_DIR!\common\on_actions"

copy /y "common\decisions\categories\ace_auto_assign_categories.txt" "!STEAM_DIR!\common\decisions\categories\" > nul
copy /y "common\decisions\ace_auto_assign_decisions.txt" "!STEAM_DIR!\common\decisions\" > nul
copy /y "common\on_actions\ace_auto_assign_on_actions.txt" "!STEAM_DIR!\common\on_actions\" > nul

echo [3/3] 로컬라이징(언어) 파일 배포 중...
if not exist "!STEAM_DIR!\localisation\korean" mkdir "!STEAM_DIR!\localisation\korean"
if not exist "!STEAM_DIR!\localisation\english" mkdir "!STEAM_DIR!\localisation\english"

copy /y "localisation\korean\ace_auto_assign_l_korean.yml" "!STEAM_DIR!\localisation\korean\" > nul
copy /y "localisation\english\ace_auto_assign_l_english.yml" "!STEAM_DIR!\localisation\english\" > nul

echo =========================================================
echo [성공] AceAutoAssigner v2.1 파일이 스팀 폴더로 안전하게 배포되었습니다!
echo 스팀에서 바로 게임을 실행하시면 최신 모드가 적용됩니다.
echo =========================================================
pause
