Overview
This utility is designed to perform a comprehensive expansion memory test directly from a TI-99/4A console. The program is optimized to operate entirely within an 8K cartridge space and console scratchpad RAM, ensuring that the target memory being tested is never used by the application itself during execution.

This project was originally taken from JediMatt42 (Matthew Splett), 'ti-99/4a-memtest'.  On top of expanded this programs ability to be able
to detect and test the 838-MEGARAM card, I have also broke it out into separate modules.

Supported Cards
The following hardware is currently supported:

• 838-MEG-A-RAM (New Support)
• SAMS Memory (Up to 16M)
• Basic 32K Memory Expansions (Internal or External)
• Myarc 128K or 512K
• Foundation 128K (Including 512K mods)
• Corcomp 256K/512K RAM Disk
• Other cards may fallback to a basic 32K detection mode.

Note: A hardware error on a SAMS unit may occasionally be identified as a 64K Foundation card due to overlapping paging schemes.

Build Instructions

This has been designed to be built with GCC 4.4.0 for the tms9900 with Tursi's libti99.

For a complete automated build of GCC 4.4.0 please use my install script:
https://github.com/shift838/gcc-tms9900_install

Path Configuration: Add your GCC 4.4.0 tms9900gcc bin folder to your system path.

Library Setup: Set an environment variable pointing to the path of Tursi’s libti99 GCC library.

Makefile: Update the Makefile with your correct path for libti99.

Execute: Run make to generate the exptest_c.bin 8K cartridge image.

Technical Highlights
This project serves as a practical example of utilizing GCC to develop TI-99/4A code that functions independently of the 32K expansion RAM.

• Interrupts: Disabled throughout the process.
• Stack Management: The GCC stack is configured to work backwards from the top of the scratchpad space (>8400).
• Data Segment: The data segment (>8320) begins immediately following the GCC workspace (>8300).
• Storage: All code and constants are stored strictly within the 8K cartridge ROM space (>6000).

License
Public Domain – Feel free to use and modify, or refer to License.txt for details.
