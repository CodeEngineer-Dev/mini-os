#include <stddef.h>
#include <stdint.h>
#include <kernel/uart.h>
#include <common/stdio.h>
#include <common/stdlib.h>

// The heart of the kernel where stuff happens.
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    char buf[256];
    
    // These three parameters are the initialization parameters of r0, r1, and r2 by convention.
    (void) r0;
    (void) r1;

    // Contains a pointer to the atags, which is info about the hardware and commandline used to run the kernel in memory.
    (void) atags;
    
    // Initializes UART.
    uart_init();

    // Put "Hello World" to UART
    puts("Hello, kernel World!\n");
    
    // Forever, just get UART data (which will be the keyboard input in this case) and put it to UART (which will print to our terminal). 
    while (1) {
        putc(getc());
        putc('\n');
    }
}
