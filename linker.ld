/*
* . means current address
* .text is where executable code goes
* .rodata is read only data (global constants)
* .data is where global variables initialized at compile time are placed
* .bss is where uninitialized global variables are placed
* */

/* Declares that the symbol _start from boot.S is the entry point to our code */
ENTRY(_start)

/*  */
SECTIONS
{
    /* Starts at LOADER_ADDR. */

    /* This is where the __start and __text_start symbols are initialized to 0x8000 */
    . = 0x8000;
    __start = .;
    __text_start = .;

    /* Declares the .text section to start */
    .text :
    {
        /* .text.boot is where the code from boot.S is. KEEP indicates that the linker should not try to optimize the code in it. */
        KEEP(*(.text.boot))

        /* Loads all of the code from all the other objects */
        *(.text)
    }

    /* Align to the nearest address divisible by 4096 (important for working with memory) */
    . = ALIGN(4096); /* align to page size */
    __text_end = .;
    
    /* Repeat the process with .rodata, .data, and .bss */

    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __rodata_end = .;
 
    __data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
}
