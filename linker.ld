ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

PHDRS
{
    text PT_LOAD FLAGS(5);  /* RX */
    data PT_LOAD FLAGS(6);  /* RW */
    bss  PT_LOAD FLAGS(6);  /* RW */
}

SECTIONS
{
    . = 0x0100000;

    .text : AT (ADDR(.text)) {
        *(.multiboot)
        *(.text*)
        *(.rodata)
    } :text

    .data : AT (ADDR(.data)) {
        start_ctors = .;
        KEEP(*( .init_array));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        end_ctors = .;
        *(.data)
    } :data

    .bss : AT (ADDR(.bss)) {
        *(.bss)
    } :bss

    /DISCARD/ : 
    {
        *(.fini_array*)
        *(.comment)
    }
}