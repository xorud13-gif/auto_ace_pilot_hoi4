<p align="right">
  <a href="README.md">한국어</a> | <b>English</b>
</p>

# Hearts of Iron IV - Ace Auto Assigner

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Target: HoI4](https://img.shields.io/badge/Hearts%20of%20Iron%20IV-v1.19.2%2B-red.svg)](https://store.steampowered.com/app/394360/Hearts_of_Iron_IV/)
[![Language: C++ / PDX Script](https://img.shields.io/badge/Language-C%2B%2B%20%7C%20PDX-orange.svg)](#)

A native C++ memory hook and in-game decision integration mod for **Hearts of Iron IV** that completely eliminates the tedious micromanagement of manually assigning ace pilots across dozens or hundreds of air wings.

---

## 🌟 Key Features

1. **Seamless In-Game [Decisions] UI Integration**
   * Adds an **[Air Staff: Ace Pilot Management]** category to the top Decisions menu.
   * **`[Auto-Assign Aces Immediately (Once)]`**: One-click action to instantly assign available pool aces to all vacant air wings.
   * **`[Disable / Enable Periodic Auto-Assignment]`**: Toggle automated assignment on or off anytime (Default: ON).
2. **Instant Replacement on Ace Death (`ACE_AUTO_ASSIGN:ACE_DIED`)**
   * Automatically replaces killed aces within 0.001s across all 4 major death scenarios (flak/enemy aircraft kills, killed by enemy aces, mutual kills, and accidental crashes).
3. **Instant Assignment upon Ace Promotion (`ACE_AUTO_ASSIGN:PROMOTED`)**
   * Whenever a new ace is promoted during combat missions, they are immediately placed into a vacant air wing.
4. **Daily Midnight Routine Scan (`ACE_AUTO_ASSIGN:DAILY`)**
   * Automatically scans and fills vacant air wings once a day as game time progresses.
5. **100% Main Thread Synchronization (Zero Crash Risk)**
   * Uses an internal Clausewitz engine `LogDispatcher` hook to execute all operations synchronously on the main thread, ensuring zero stutter, zero race conditions, and zero crashes.

---

## 📁 Repository Structure

```text
Hearts-of-Iron-IV-AceAutoAssigner/
├── common/                     # [In-game mod scripts]
│   ├── decisions/              # Decision UI scripts (Toggle / Instant execution)
│   └── on_actions/             # On-action triggers (Death / Promotion / Daily)
│
├── localisation/               # [Multi-language localization]
│   ├── korean/                 # Korean tooltips
│   └── english/                # English tooltips
│
├── native_mod/                 # [C++ Native Module Source]
│   └── src/
│       ├── signatures.hpp      # Patch-proof signature & offset configurations
│       ├── ace_assigner.cpp    # Core logic (LogDispatcher & AssignAces)
│       ├── hook.cpp / .hpp     # x64 inline memory hook engine
│       ├── proxy.cpp / .def    # version.dll proxy DLL loader
│       └── build.bat           # One-click compilation script
│
├── tools/                      # [Maintenance utilities]
│   └── patch_verifier.py       # Compatibility diagnostic tool for HoI4 patches/DLCs
│
├── deploy_to_steam.bat         # One-click deployment script for Steam
├── version.dll                 # Pre-built release DLL (v2.1)
├── MAINTENANCE.md              # Patch & DLC maintenance manual
├── LICENSE                     # GNU General Public License v3.0
├── README.md                   # Korean documentation
└── README_EN.md                # English documentation (This file)
```

---

## 🚀 Installation & Setup

### Automated Installation (Recommended)
1. **[Direct Download ZIP Package (Click Here)](https://github.com/xorud13-gif/auto_ace_pilot_hoi4/archive/refs/heads/main.zip)** or click **`Code` ➔ `Download ZIP`** on the GitHub repository, then extract the downloaded ZIP.
2. Double-click and run `deploy_to_steam.bat`.
   * **Auto-detection**: Automatically queries the Windows Registry (`Steam App ID: 394360`) to locate your Hearts of Iron IV directory regardless of which drive (`C:`, `D:`, `H:`, etc.) your Steam library is stored on.
   * (If auto-detection fails in custom/portable setups, you can simply type or drag-and-drop your game folder into the console window.)
3. Launch Hearts of Iron IV normally via Steam.

### Manual Installation
Copy the following files into your Hearts of Iron IV root game directory:
* `version.dll` → Game root folder (alongside `hoi4.exe`)
* `common/` → Merge with the root `common/` folder
* `localisation/` → Merge with the root `localisation/` folder

---

## 🛠️ Building from Source

This project uses the ultra-lightweight, cross-platform compiler **Zig (0.13.0+)**, allowing full compilation on Windows without requiring Microsoft Visual Studio.

1. Download the Zig compiler from the [Zig Official Website](https://ziglang.org/download/) and extract it into `tools/zig/`.
2. Run `native_mod/src/build.bat`.
3. The newly compiled `version.dll` will be generated in the root folder.

---

## 🔄 Handling Game Patches & New DLCs

When Paradox releases a major game patch or DLC that alters the executable binary, you can immediately verify compatibility with the included automated diagnostic tool:

```bash
python tools/patch_verifier.py
```
* Refer to [MAINTENANCE.md](MAINTENANCE.md) for detailed maintenance procedures.

---

## 📜 License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.

* **Author**: [xorud13 (xorud13-gif)](https://github.com/xorud13-gif)
* **Repository**: [auto_ace_pilot_hoi4](https://github.com/xorud13-gif/auto_ace_pilot_hoi4)
* **Copyright**: (C) 2026 xorud13
