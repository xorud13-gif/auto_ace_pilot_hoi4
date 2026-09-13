# [유지보수 가이드] HoI4 패치 & DLC 업데이트 대응 매뉴얼

본 문서는 향후 Hearts of Iron IV의 대규모 패치나 신규 DLC가 출시되었을 때, **스팀 본편 폴더를 건드리지 않고 1~2분 만에 네이티브 모드(`version.dll`)를 최신 패치 버전에 맞추어 갱신하는 표준 절차**를 설명합니다.

---

## 1. 핵심 아키텍처 및 의존 지점

이 모드가 게임 바이너리(`hoi4.exe`)에서 의존하는 요소는 **단 4가지**뿐입니다:

| 의존 요소 | 역할 | 상세 설명 & 위치 설정 파일 |
| :--- | :--- | :--- |
| **`AssignAces`** | 대기 예비 에이스를 빈 비행단에 배치 | `native_mod/src/signatures.hpp` (RVA `0x12AF5C0`) |
| **`Country_GetAirManager`** | 국가 객체에서 공군 매니저를 획득 | `native_mod/src/signatures.hpp` (RVA `0x02A8CF0`) |
| **`LogDispatcher`** | 인게임 디시전/온액션 스크립트 실행 함수 가로채기 | `native_mod/src/signatures.hpp` (`CEffectLog::Execute`, RVA `0x137ED90`) |
| **`OFFSET_AIRMGR_UNASSIGNED_COUNT`** | CAirManager 내 미배치 에이스 수 오프셋 (`0x12E4`) | `native_mod/src/signatures.hpp` |

---

## 2. 패치 발생 시 표준 3단계 대응 절차

### [1단계] 패치 자동 진단 도구 실행
스팀 업데이트가 완료된 후, 작업 폴더 터미널에서 아래 명령을 실행합니다:
```powershell
python tools\patch_verifier.py
```

- **출력 결과 A**: `[SUCCESS] ALL SIGNATURES & OFFSETS ARE 100% COMPATIBLE!`
  - 패치 후에도 함수 구조가 바뀌지 않았으므로, **추가 수정 없이 기존 version.dll을 그대로 사용**하시면 됩니다.
- **출력 결과 B**: `[ACTION REQUIRED] Game update detected changes!`
  - 콘솔에 표시된 변경된 RVA 또는 구조체 오프셋을 확인합니다.

---

### [2단계] 시그니처 갱신 (변경점이 있을 경우에만)
만약 진단 툴에서 RVA나 오프셋 변경이 보고된 경우:
1. [`native_mod/src/signatures.hpp`](native_mod/src/signatures.hpp) 파일을 엽니다.
2. 진단 툴이 안내한 새 주소나 오프셋으로 수치를 교체하고 저장합니다.
   ```cpp
   // 예시: 패치로 주소가 변경되었을 경우
   constexpr uintptr_t RVA_ASSIGN_ACES = 0x12XXXXX;
   constexpr uintptr_t OFFSET_AIRMGR_UNASSIGNED_COUNT = 0x12XX;
   ```
3. `native_mod/src/build.bat`을 실행합니다:
   ```cmd
   cd native_mod\src
   build.bat
   ```
   (빌드 성공 시 프로젝트 루트 폴더의 `version.dll`이 자동으로 최신 빌드로 갱신됩니다.)

---

### [3단계] 스팀 폴더로 최신 파일 배포
작업 폴더 루트의 배포 스크립트를 더블클릭하거나 터미널에서 실행합니다:
```cmd
deploy_to_steam.bat
```
스팀 설치 경로가 자동으로 감지되며, 모드 파일들이 단 1초 만에 최신 상태로 동기화됩니다.

---

## 3. 디렉터리 구성 안내

```text
H:\Coding\game\Hearts of Iron IV\
│
├── common\                     # [모드 스크립트]
│   ├── decisions\              # 인게임 결정 (UI 켜기/끄기/즉시배치)
│   └── on_actions\             # 사망/진급/시작 동기화 온액션
│
├── localisation\               # [로컬라이징]
│   ├── korean\                 # 한국어 툴팁
│   └── english\                # 영어 툴팁
│
├── native_mod\                 # [네이티브 C++ 소스]
│   └── src\
│       ├── signatures.hpp      # ★ 주소 & 시그니처 전용 설정 헤더
│       ├── ace_assigner.cpp    # 핵심 로직 (CEffectLog & AssignAces)
│       └── build.bat           # Zig C++ 컴파일러 기반 원클릭 빌더
│
├── tools\                      # [유지보수 & 도구]
│   ├── patch_verifier.py       # ★ 패치 자동 진단기
│   └── zig\                    # 내장 Zig C++ 컴파일러
│
├── deploy_to_steam.bat         # ★ 스팀 본편 폴더 원클릭 배포기
├── version.dll                 # 빌드된 최종 DLL (v2.3)
└── MAINTENANCE.md              # 본 유지보수 설명서
```

---

## 4. 🤖 AI 개발 및 유지보수 안내

본 프로젝트는 최신 AI 코딩 어시스턴트(Google DeepMind Antigravity / Claude)와의 페어 프로그래밍을 통해 개발되었습니다.  
향후 대형 패치나 DLC로 인해 함수 구조가 대대적으로 변경되더라도, AI에게 `MAINTENANCE.md`와 `python tools/patch_verifier.py`의 출력 결과를 전달하시면 신속하게 새로운 시그니처와 오프셋을 역분석 및 재도출하여 모드를 즉시 갱신할 수 있습니다.
