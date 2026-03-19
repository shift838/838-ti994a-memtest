# TI-99/4A Memory Expansion Diagnostic
This project is a modular fork of the original ti994a-memtest developed by JediMatt42 (Matthew Splett). This version introduces native support for the 838-MEGARAM card and restructures the codebase into distinct, manageable modules.

# Project Overview
The utility performs comprehensive expansion memory diagnostics directly from a TI-99/4A console. It operates within an 8K cartridge space and console scratchpad RAM; the memory being tested is never used by the program.

# [!WARNING]
Data Loss Notice: These are destructive memory tests. Executing these routines will erase any existing RAM Disk content or pre-loaded interrupt routines.

# Supported Cards
* 838-MEGARAM (New)
* SAMS Memory (Up to 16MB)
* Basic 32K Expansions (Internal or External)
* Myarc 128K / 512K
* Foundation 128K (Including 512K modifications)
* Corcomp 256K / 512K RAM Disk

*** Legacy cards may fallback to a standard 32K detection. Note: Due to overlapping paging schemes, certain SAMS hardware errors may incorrectly identify as a 64K Foundation card.

# Build Instructions
Compiler Setup:
* Compiler required GCC 4.4.0. Auto-install script: https://github.com/shift838/gcc-tms9900_install
* Add your tms9900gcc bin directory to your system path.
* Library: Set the environment variable for Tursi's libTi99All GCC library path.

Compile:
Run make to generate the exptest_c.bin (8K cartridge image).

# Technical Highlights
* Demonstrates GCC usage for a multi-module program without requiring 32K expansion RAM:
* Interrupts: Maintained in the OFF state.
* Stack: Mapped backwards from the top of scratchpad (>8400).
* Memory Mapping: Data segment (>8320) follows the GCC workspace (>8300).
* Persistence: All constants and code are stored in 8K cartridge ROM space (>6000).

# License
Public Domain — Use freely. See License.txt.
