# Default target: builds everything including the cartridge binary
all: exptest_c.bin

CC       = tms9900-unknown-elf-gcc
AS       = tms9900-unknown-elf-as
LD       = tms9900-unknown-elf-ld
OBJCOPY  = tms9900-unknown-elf-objcopy
LIBTI99 = $(HOME)/libTi99All

CFLAGS_COMMON = -std=c99 --save-temp -DTI99 -I$(LIBTI99)

# Default rule for most .c files (optimized for size)
%.o: %.c
	$(CC) -c $< $(CFLAGS_COMMON) -Os -o $@

# Special rule: main.c must be compiled with -O0 to avoid PC8 overflows
main.o: main.c
	$(CC) -c main.c $(CFLAGS_COMMON) -O0 -o main.o

# Special rule: expansion_sams.c must also be compiled with -O0
expansion_sams.o: expansion_sams.c
	$(CC) -c expansion_sams.c $(CFLAGS_COMMON) -O0 -o expansion_sams.o

# Assemble .asm files
%.o: %.asm
	$(AS) $< -o $@

# Object list - Ensure header.o is first for cartridge boot
OBJECT_LIST = \
    header.o \
    crt0.o \
    main.o \
    testblock.o \
    cru.o \
    expansion_foundation.o \
    expansion_corcomp.o \
    expansion_sams.o \
    utils.o \
    hasram.o

# Linker flags
LDFLAGS = \
    --section-start .text=6000 \
    --section-start .data=8320

# 1. Link step: Creates the ELF file
exptest_c.elf: $(OBJECT_LIST)
	$(LD) $(OBJECT_LIST) $(LDFLAGS) -L$(LIBTI99) -o exptest_c.elf -lti99 -Map=mapfile

# 2. Binary step: Creates the TI-99/4A Cartridge Binary and truncates to 8192 bytes
exptest_c.bin: exptest_c.elf
	$(OBJCOPY) -O binary exptest_c.elf exptest_c.bin
	truncate -s 8192 exptest_c.bin
	@echo "The binary has been truncated to 8192 bytes and is ready for use in an emulator or to burn to an EPROM."

# Cleanup
clean:
	rm -f *.o *.elf *.lst *.s mapfile *.bin *.i *.ii
