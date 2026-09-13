<p align="right">
  <b>한국어</b> | <a href="README_EN.md">English</a>
</p>

# Hearts of Iron IV - Ace Auto Assigner (에이스 조종사 자동 배치 모드)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Target: HoI4](https://img.shields.io/badge/Hearts%20of%20Iron%20IV-v1.19.2%2B-red.svg)](https://store.steampowered.com/app/394360/Hearts_of_Iron_IV/)
[![Version: v2.3](https://img.shields.io/badge/Version-v2.3-green.svg)](https://github.com/xorud13-gif/auto_ace_pilot_hoi4/releases/tag/v2.3)
[![Developed with: AI](https://img.shields.io/badge/Developed%20with-AI%20Assisted-blueviolet.svg)](#-개발-방식-및-크레딧)
[![Language: C++ / PDX Script](https://img.shields.io/badge/Language-C%2B%2B%20%7C%20PDX-orange.svg)](#)

Hearts of Iron IV에서 공군을 대규모로 운용할 때, 수십~수백 개의 비행단에 에이스를 일일이 수동으로 배정해야 하는 번거로움을 완전히 해결해 주는 **C++ 네이티브 메모리 훅 & 인게임 결정 통합 모드**입니다.

> 🤖 **AI 지원 개발 안내 (AI-Assisted Project)**: 본 프로젝트의 모든 구성 요소(클라우제비츠 엔진 바이너리 역분석, x64 어셈블리 훅, C++ DLL 프록시, 파이썬 진단 도구 및 인게임 스크립트)는 최신 AI 코딩 어시스턴트(Google DeepMind Antigravity / Claude)와의 페어 프로그래밍을 통해 분석, 설계 및 개발되었습니다.

---

## 🌟 주요 기능

1. **인게임 [결정과 선택(Decisions)] UI 완벽 통합**
   * 게임 화면 상단 '결정과 선택' 메뉴에 **[공군 참모본부: 에이스 조종사 관리]** 카테고리가 생성됩니다.
   * **`[에이스 즉시 자동 배치 (1회)]`**: 대기 풀의 에이스들을 즉시 빈 비행단에 채워 넣는 1회성 실행 버튼.
   * **`[에이스 주기적 자동 배치 비활성화 / 활성화]`**: 자동 배치 기능을 언제든 켜고 끌 수 있는 실시간 토글 지원 (기본 상태: ON).
2. **에이스 전사 즉시 자동 교체 (`ACE_AUTO_ASSIGN:ACE_DIED`)**
   * 일반 대공포/적기 격추, 적 에이스에 의한 격추, 상호 격추, 비행 사고 등 **에이스 사망 4대 상황** 발생 시 0.001초 만에 대기 예비 에이스를 찾아 빈자리에 즉각 교체 투입합니다.
3. **신규 에이스 진급 시 즉각 투입 (`ACE_AUTO_ASSIGN:PROMOTED`)**
   * 작전 중 새로운 에이스가 탄생하면 즉시 빈 비행단을 찾아 자동으로 채워 넣습니다.
4. **실시간 주기 점검 & 세이브 파일 상태 동기화 (`on_startup`)**
   * 활성화 상태일 때 3초 주기로 전체 비행단의 빈자리를 안전하게 자동 스캔 및 보충합니다.
   * 게임을 저장한 뒤 다시 불러오더라도 비활성화 설정이 풀리지 않고 그대로 유지됩니다.
5. **클라우제비츠 엔진 CEffectLog 직접 후킹 (크래시 위험 0%)**
   * 게임 엔진의 스크립트 실행 함수(`CEffectLog::Execute`, RVA `0x137ED90`)를 직접 후킹하여 모든 연산이 메인 게임 스레드에서 무지연으로 수행됩니다. 게임 끊김(렉)이나 크래시가 발생하지 않습니다.

---

## 📁 디렉터리 구성

```text
Hearts-of-Iron-IV-AceAutoAssigner/
├── common/                     # [인게임 모드 스크립트]
│   ├── decisions/              # 결정 UI 스크립트 (켜기/끄기/즉시배치)
│   └── on_actions/             # 사망/진급/시작 동기화 온액션
│
├── localisation/               # [다국어 지원]
│   ├── korean/                 # 한국어 툴팁
│   └── english/                # 영어 툴팁
│
├── native_mod/                 # [C++ 네이티브 모듈 소스코드]
│   └── src/
│       ├── signatures.hpp      # 패치 대비 시그니처 & 오프셋 전용 설정 파일
│       ├── ace_assigner.cpp    # 핵심 로직 (LogDispatcher & AssignAces)
│       ├── hook.cpp / .hpp     # x64 인라인 후킹 모듈
│       ├── proxy.cpp / .def    # version.dll 프록시 로더
│       └── build.bat           # 원클릭 컴파일 스크립트
│
├── tools/                      # [유지보수 도구]
│   └── patch_verifier.py       # 패치/DLC 출시 시 호환성 자동 진단기
│
├── deploy_to_steam.bat         # 스팀 본편 폴더 원클릭 배포 유틸리티 (자동 감지)
├── version.dll                 # 빌드 완료된 릴리즈 DLL (v2.3)
├── MAINTENANCE.md              # 패치 및 DLC 출시 대응 가이드
├── LICENSE                     # GNU General Public License v3.0
├── README.md                   # 한국어 설명서
└── README_EN.md                # 영문 설명서 (English)
```

---

## 🚀 설치 및 적용 방법

### 간편 자동 설치 (추천)
1. **[ZIP 파일 즉시 다운로드 (클릭)](https://github.com/xorud13-gif/auto_ace_pilot_hoi4/archive/refs/heads/main.zip)** 또는 깃허브 우측 상단의 **`Code` ➔ `Download ZIP`**을 클릭하여 다운로드 후 압축을 풉니다.
2. `deploy_to_steam.bat`을 더블클릭하여 실행합니다.
   * **스팀 폴더 자동 감지**: 윈도우 레지스트리를 조회하여 스팀 게임이 어느 드라이브(`C:`, `D:`, `H:` 등)에 설치되어 있든 **사용자의 Hearts of Iron IV 본편 폴더를 자동으로 찾아 1초 만에 배포**합니다.
   * (만약 자동 감지되지 않는 특수한 포터블 환경인 경우, 안내 메시지에 따라 폴더를 콘솔 창으로 드래그 앤 드롭하시면 됩니다.)
3. 스팀에서 평소처럼 게임을 실행합니다.

### 수동 설치
스팀 본편 폴더(`Hearts of Iron IV`)에 아래 파일들을 복사합니다:
* `version.dll` → 게임 루트 폴더 (`hoi4.exe`와 같은 위치)
* `common/` → 게임 루트의 `common/` 폴더로 병합
* `localisation/` → 게임 루트의 `localisation/` 폴더로 병합

---

## 🛠️ 직접 빌드하는 방법 (소스 컴파일)

본 프로젝트는 초경량 크로스 플랫폼 컴파일러인 **Zig (0.13.0+)**를 사용하여 Visual Studio 없이도 즉시 빌드가 가능합니다.

1. [Zig 공식 홈페이지](https://ziglang.org/download/)에서 Zig 컴파일러를 다운로드하여 `tools/zig/` 경로에 압축을 풉니다.
2. `native_mod/src/build.bat`을 실행합니다.
3. 루트 폴더에 새로운 `version.dll`이 자동으로 생성 및 교체됩니다.

---

## 🔄 게임 패치 및 DLC 출시 대응

향후 패치로 인해 게임 바이너리가 변경되었을 때, 본 프로젝트에 포함된 자동 진단 도구를 통해 손쉽게 호환성을 확인할 수 있습니다:

```bash
python tools/patch_verifier.py
```
* 상세한 패치 대응 절차는 [MAINTENANCE.md](MAINTENANCE.md)를 참고하세요.

---

## 📜 버전 변경 내역 (Changelog)

* **v2.3 (최신)**
  - **비활성화 기능 정상화**: 패러독스 클라우제비츠 엔진의 스크립트 실행 함수(`CEffectLog::Execute`, RVA `0x137ED90`)를 직접 후킹하여 인게임 [비활성화] 클릭 시 즉시 자동 배치가 완벽하게 멈추도록 개선.
  - **배포 스크립트(`deploy_to_steam.bat`) 완성**: 윈도우 배치 파일 특유의 캐리지 리턴 및 파싱 버그를 해결하여 어떤 드라이브 환경에서도 추가 입력 없이 원클릭 자동 배포 지원.
  - **세이브 로드 상태 보존**: 세이브 파일을 다시 불러올 때 비활성화 상태가 풀리지 않고 유지되도록 온액션(`on_startup`) 플래그 동기화 추가.
* **v2.2**
  - 인게임 결정(Decisions) 및 온액션(On-Actions) 스크립트 제어 브릿지 도입.
* **v2.1**
  - 다국어 지원 (한국어 / English) 및 패치 자동 진단기(`patch_verifier.py`) 추가.
* **v2.0**
  - 네이티브 메모리 훅 기반 에이스 자동 배치 모드 최초 릴리즈.

---

## 📜 개발 방식 및 크레딧 (License & Credits)

본 프로젝트는 **GNU General Public License v3.0** 하에 배포됩니다 - 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

* **Author**: [xorud13 (xorud13-gif)](https://github.com/xorud13-gif)
* **Development Method**: AI 지원 페어 프로그래밍 (AI-Assisted Development with Google DeepMind Antigravity / Claude)
* **Repository**: [auto_ace_pilot_hoi4](https://github.com/xorud13-gif/auto_ace_pilot_hoi4)
* **Copyright**: (C) 2026 xorud13
