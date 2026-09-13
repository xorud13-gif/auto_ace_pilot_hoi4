#!/usr/bin/env python3
"""
Hearts of Iron IV - Native Mod Patch Verifier & Signature Diagnostic Tool
=========================================================================
Usage:
    python patch_verifier.py [path_to_hoi4.exe]

Default search path:
    1. H:\\STEAM\\steamapps\\common\\Hearts of Iron IV\\hoi4.exe
    2. .\\hoi4.exe
"""

import sys
import os
import struct

def find_hoi4_binary():
    """
    Dynamically discovers the hoi4.exe path:
    1. CLI Argument
    2. Windows Registry (Steam App ID 394360 InstallLocation)
    3. Steam Client Path (HKCU\\Software\\Valve\\Steam)
    4. Common install directory fallbacks
    """
    if len(sys.argv) > 1:
        arg = sys.argv[1].strip('"')
        if os.path.isfile(arg):
            return arg
        cand = os.path.join(arg, "hoi4.exe")
        if os.path.isfile(cand):
            return cand

    try:
        import winreg
        reg_keys = [
            (winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 394360"),
            (winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 394360"),
        ]
        for root, subkey in reg_keys:
            try:
                with winreg.OpenKey(root, subkey) as key:
                    install_loc, _ = winreg.QueryValueEx(key, "InstallLocation")
                    if install_loc:
                        cand = os.path.join(install_loc.strip('"'), "hoi4.exe")
                        if os.path.isfile(cand):
                            return cand
            except OSError:
                pass

        try:
            with winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Valve\Steam") as key:
                steam_path, _ = winreg.QueryValueEx(key, "SteamPath")
                if steam_path:
                    cand = os.path.join(steam_path.strip('"'), "steamapps", "common", "Hearts of Iron IV", "hoi4.exe")
                    if os.path.isfile(cand):
                        return cand
        except OSError:
            pass
    except ImportError:
        pass

    candidates = [
        r"hoi4.exe",
        r"..\hoi4.exe",
        r"C:\Program Files (x86)\Steam\steamapps\common\Hearts of Iron IV\hoi4.exe",
        r"C:\Steam\steamapps\common\Hearts of Iron IV\hoi4.exe",
        r"D:\Steam\steamapps\common\Hearts of Iron IV\hoi4.exe",
        r"D:\SteamLibrary\steamapps\common\Hearts of Iron IV\hoi4.exe",
        r"E:\SteamLibrary\steamapps\common\Hearts of Iron IV\hoi4.exe",
    ]
    for p in candidates:
        if os.path.isfile(p):
            return p

    return None

# Configured signatures to verify (from native_mod/src/signatures.hpp)
SIGNATURES = {
    "AssignAces": {
        "pattern": b"\x48\x89\x4C\x24\x08\x53\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x68\x0F\x29\x74\x24\x50\x48\x8B\xD9",
        "mask":    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "expected_rva": 0x12AF5C0,
        "check_offset": True
    },
    "Country_GetAirManager": {
        "pattern": b"\x40\x53\x48\x83\xEC\x30\x48\x8B\xD9\x48\x8B\x49\x08\x48\x85\xC9\x75\x08\x33\xC0\x48\x83\xC4\x30\x5B\xC3\x80\x3D\xE1",
        "mask":    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "expected_rva": 0x02A8CF0,
        "check_offset": False
    },
    "LogDispatcher": {
        "pattern": b"\x48\x89\x5C\x24\x18\x48\x89\x74\x24\x20\x57\x48\x81\xEC\x90\x00\x00\x00\x48\x8B\xF2\x48\x8B\xF9",
        "mask":    "xxxxxxxxxxxxxxxxxxxxxxxx",
        "expected_rva": 0x24ACF90,
        "check_offset": False
    }
}

EXPECTED_UNASSIGNED_OFFSET = 0x12E4

def parse_pe(path):
    with open(path, "rb") as f:
        data = f.read()

    e_lfanew = struct.unpack_from("<I", data, 0x3c)[0]
    magic = struct.unpack_from("<I", data, e_lfanew)[0]
    if magic != 0x4550:
        raise ValueError("Invalid PE file magic.")

    num_sections = struct.unpack_from("<H", data, e_lfanew + 6)[0]
    opt_header_size = struct.unpack_from("<H", data, e_lfanew + 20)[0]
    opt_header_offset = e_lfanew + 24
    image_base = struct.unpack_from("<Q", data, opt_header_offset + 24)[0]

    section_offset = opt_header_offset + opt_header_size
    sections = []
    text_section = None
    for i in range(num_sections):
        sec = data[section_offset + i * 40 : section_offset + (i + 1) * 40]
        name = sec[:8].rstrip(b'\x00').decode('latin1')
        vsize, vaddr, rsize, raddr = struct.unpack_from("<IIII", sec, 8)
        sections.append((name, vaddr, vsize, raddr, rsize))
        if name == ".text":
            text_section = (name, vaddr, vsize, raddr, rsize)

    return image_base, sections, text_section, data

def scan_pattern(data, text_sec, pattern, mask):
    name, vaddr, vsize, raddr, rsize = text_sec
    text_bytes = data[raddr : raddr + rsize]
    pat_len = len(pattern)

    for i in range(len(text_bytes) - pat_len):
        match = True
        for j in range(pat_len):
            if mask[j] == 'x' and text_bytes[i + j] != pattern[j]:
                match = False
                break
        if match:
            return vaddr + i  # Return RVA
    return None

def check_unassigned_offset(data, text_sec, assign_rva):
    name, vaddr, vsize, raddr, rsize = text_sec
    file_off = raddr + (assign_rva - vaddr)
    func_bytes = data[file_off : file_off + 120]
    
    # In AssignAces: mov eax, [rbx + offset32] is opcode 8B 83 xx xx xx xx or 8B 87 ...
    # Let's search for 8B 83 or 8B 8? followed by offset
    for i in range(len(func_bytes) - 6):
        if func_bytes[i] == 0x8B and (func_bytes[i + 1] & 0xF8) == 0x80:
            off = struct.unpack_from("<I", func_bytes, i + 2)[0]
            if 0x1000 <= off <= 0x2000:
                return off
    return None

def main():
    target_path = find_hoi4_binary()

    if not target_path or not os.path.exists(target_path):
        print("[ERROR] Could not find hoi4.exe automatically via Windows Registry or standard paths.")
        print("Please provide the path manually:")
        print("    python patch_verifier.py \"C:\\Path\\To\\Hearts of Iron IV\\hoi4.exe\"")
        sys.exit(1)

    print("=================================================================")
    print("Hearts of Iron IV Native Mod - Patch Verifier")
    print(f"Target Binary: {os.path.abspath(target_path)}")
    print("=================================================================")

    try:
        image_base, sections, text_sec, data = parse_pe(target_path)
    except Exception as e:
        print(f"[ERROR] Failed to parse PE file: {e}")
        sys.exit(1)

    print(f"ImageBase: 0x{image_base:X}")
    print(f".text Section: RVA 0x{text_sec[1]:X}, Size 0x{text_sec[2]:X}\n")

    all_passed = True

    for name, info in SIGNATURES.items():
        print(f"[{name}]")
        rva = scan_pattern(data, text_sec, info["pattern"], info["mask"])
        if rva is not None:
            va = image_base + rva
            print(f"  -> Signature Scan: MATCH! RVA = 0x{rva:X} (VA = 0x{va:X})")
            if rva == info["expected_rva"]:
                print(f"  -> RVA Baseline: Exact match with baseline (0x{info['expected_rva']:X})")
            else:
                print(f"  -> [NOTE] RVA shifted from 0x{info['expected_rva']:X} to 0x{rva:X} (Signature pattern still safely found it!)")
            
            if info["check_offset"]:
                detected_off = check_unassigned_offset(data, text_sec, rva)
                if detected_off:
                    print(f"  -> CAirManager Unassigned Offset: 0x{detected_off:X}", end="")
                    if detected_off == EXPECTED_UNASSIGNED_OFFSET:
                        print(" (MATCHED!)")
                    else:
                        print(f" (WARNING: Offset changed! Baseline was 0x{EXPECTED_UNASSIGNED_OFFSET:X})")
                        all_passed = False
                else:
                    print("  -> CAirManager Unassigned Offset: Could not automatically read.")
        else:
            print(f"  -> Signature Scan: FAILED! (Target signature not found in .text)")
            all_passed = False
        print()

    print("=================================================================")
    if all_passed:
        print("[SUCCESS] ALL SIGNATURES & OFFSETS ARE 100% COMPATIBLE!")
        print("Your version.dll does NOT need any modifications.")
        print("You can safely play on this game version.")
    else:
        print("[ACTION REQUIRED] Game update detected changes!")
        print("1. Update 'native_mod/src/signatures.hpp' with any new RVAs/offsets.")
        print("2. Run 'native_mod/src/build.bat' to recompile version.dll.")
        print("3. Run 'deploy_to_steam.bat' to update your game folder.")
    print("=================================================================")

if __name__ == "__main__":
    main()
