TI-99/4A Memory Expansion Diagnostic
This project is a modular fork of the original ti994a-memtest developed by JediMatt42 (Matthew Splett). This version introduces native support for the 838-MEGARAM card and restructures the codebase into distinct, manageable modules.

Project Overview
The utility performs comprehensive expansion memory diagnostics directly from a TI-99/4A console. It is specifically engineered to operate within an 8K cartridge space and console scratchpad RAM, ensuring that the memory being tested remains entirely untouched by the program's internal operations.

[!WARNING]
Data Loss Notice: These are destructive memory tests. Executing these routines will erase any existing RAM Disk content or pre-loaded interrupt routines.

Supported Cards:
• 838-MEGARAM (New)
• SAMS Memory (Up to 16MB)
• Basic 32K Expansions (Internal or External)
• Myarc 128K / 512K
• Foundation 128K (Including 512K modifications)
• Corcomp 256K / 512K RAM Disk

***Legacy cards may fallback to a standard 32K detection.
Note: Due to overlapping paging schemes, certain SAMS hardware errors may incorrectly identify as a 64K Foundation card.

Build Instructions
Compiler: 
• This was designed to be compiled with GCC 4.0.0
    - Download a complete auto-install script at https://github.com/shift838/gcc-tms9900_install
• Add your tms9900gcc bin directory to your system path.
• Library: Set the environment variable for Tursi's libti99 GCC library path.

Compile: Run make to generate the exptest_c.bin (ready to use 8K cartridge image for emulators or burning to an EPROM).

Technical Highlights
This build serves as a demonstration of using GCC to produce TI-99/4A code that functions without requiring the 32K expansion RAM:

- Interrupts: Maintained in the OFF state.
- Stack: The GCC stack is mapped to run backwards from the top of the scratchpad (>8400).
- Memory Mapping: The data segment (>8320) is positioned immediately following the GCC workspace (>8300).
- Persistence: All constants and code are stored within the 8K cartridge ROM space (>6000).

License
Public Domain — Use freely. For more information, see License.txt.
