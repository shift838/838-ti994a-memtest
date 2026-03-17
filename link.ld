ENTRY(_start)

SECTIONS
{
    . = 0xA000;

    .text : {
        crt0.o(.text)
        *(.text*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }
}