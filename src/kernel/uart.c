#include <stddef.h>
#include <stdint.h>
// These are freestanding C binaries, which means that we can use them even in barebones C.

#include <kernel/uart.h>
#include <common/stdlib.h>


// These functions handle Memory Mapped IO (MMIO), which is interacting with hardware devices by reading and writing to predefined memory addresses.
// A peripheral is a hardware device with a specific memory address that it reads and writes data from. All peripherals have a location that is offset by the Peripheral Base Address (which is 0x3F000000 on RPI2).
// A register is a four byte memory piece. A control register has each bit correspond to a certain behavior. A write register sends anything on it to the hardware.

// Takes as input a register and writes a four byte word to the register.
void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

// Takes as input a register and reads the four byte word in it.
uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}


// This is just a busy loop written in assembly.
// Loop <delay> times in a way that the compiler won't optimize away
void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}


// Initializes the UART hardware for use, by writing configuration flags in various registers.
void uart_init()
{
    // Disables all aspects of UART.
    mmio_write(UART0_CR, 0x00000000);
    
    // Marks that pins of GPIO should be disabled
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // Specifies which pins should be disabled (pins 14 and 15)
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    
    // Makes the disabling take effect
    mmio_write(GPPUDCLK0, 0x00000000);

    // Sets all the flags in the Interrupt Clear Register, clearing all pending interrupts from the UART hardware.
    mmio_write(UART0_ICR, 0x7FF);

    // Sets the baud rate of the connection, or how many bits per second that can go across the serial port.
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);
    
    // Sets bit 4, 5, and 6 of the Line Control Register to 1 (on). UART will hold data in an 8 item deep FIFO and data sent and received will be 8-bit words.
    // An 8 item deep FIFO buffer is a queue-like structure that holds up to 8 data items.
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Disables all interrupts from UART by writing relavant ones to the Interrupt Mask Set Clear register.
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
    
    // Write bits 1, 8, and 9 to the control register, enabling UART, data reception, and data transmission.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

// Enables reading and writing characters to and from UART
void uart_putc(unsigned char c)
{
    // UART0_FR is the flags register. It tells whether the read FIFO is not availiable to read at bit 5. So if it is on, the FIFO is still busy and full.
    while ( mmio_read(UART0_FR) & (1 << 5) ) { }

    // But as soon as it is availiable, write the data to it.
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
    // Check if the write FIFO is availiable for reading 
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }

    // Return the data in the register.
    return mmio_read(UART0_DR);
}

// Wraps uart_putc() to allow writing entire strings
void uart_puts(const char* str)
{
    // Keep putting a character if the string is not terminated
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}

