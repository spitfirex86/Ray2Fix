#!/usr/bin/env python3
"""Audit upstream GameFrame Glide/Win32 exports before Rayman 2 testing.

Build-only utility; never edits a game executable or a user's Glide DLL.
"""
import re
import sys
from pathlib import Path

import pefile


NEEDED_GLIDE_211 = {
    "_grGlideInit@0",
    "_grGlideShutdown@0",
    "_grSstOpen@24",
    "_grSstQueryHardware@4",
    "_grBufferSwap@4",
    "_grLfbGetWritePtr@4",
    "_grLfbEnd@0",
    "_grAlphaBlendFunction@16",
    "_grAlphaCombine@20",
    "_guColorCombineFunction@4",
    "_grDrawLine@8",
}


def audit(dll_file: Path, def_file: Path) -> int:
    expected = set()
    for line in def_file.read_text(encoding="utf-8").splitlines():
        match = re.match(r"\s*(_[A-Za-z0-9_]+@\d+)\s*=", line)
        if match:
            expected.add(match.group(1))
    if not expected:
        raise ValueError("GameFrame .def has no decorated stdcall names")

    pe = pefile.PE(str(dll_file))
    if pe.FILE_HEADER.Machine != 0x014C or not pe.is_dll():
        raise ValueError("Glide DLL must be a 32-bit x86 Windows PE DLL")
    pe.parse_data_directories(
        directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]]
    )
    exports = {
        sym.name.decode("ascii")
        for sym in pe.DIRECTORY_ENTRY_EXPORT.symbols
        if sym.name
    }
    missing = sorted(expected - exports)
    surplus = sorted(exports - expected)
    old_missing = sorted(NEEDED_GLIDE_211 - exports)
    print(f"PE32 machine: 0x{pe.FILE_HEADER.Machine:04X}")
    print(f"GameFrame .def exports expected: {len(expected)}")
    print(f"PE exports found: {len(exports)}")
    print(f"Missing expected .def names: {missing}")
    print(f"Unexpected export names: {surplus}")
    print(f"Missing known Glide 2.11 essentials: {old_missing}")
    if missing or old_missing:
        return 1
    print("Static DLL export ABI check: PASS")
    print("Rayman 2 rendering, input and alt-tab: NOT YET TESTED")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("usage: audit_exports.py <glide-dll> <upstream-glide2x.def>")
    sys.exit(audit(Path(sys.argv[1]), Path(sys.argv[2])))
