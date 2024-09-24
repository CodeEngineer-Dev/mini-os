#include <stddef.h>
#include <stdint.h>
// These are freestanding C binaries, which means that we can use them even in barebones C.

// These functions handle Memory Mapped IO (MMIO), which is interacting with hardware devices by reading and writing to predefined memory addresses.
// A peripheral is a hardware device with a specific memory address that it reads and writes data from. All peripherals have a location that is offset by the Peripheral Base Address (which is 0x3F000000 on RPI2).
// A register is a four byte memory piece. A control register has each bit correspond to a certain behavior. A write register sends anything on it to the hardware.

// Takes as input a register and writes a four byte word to the register.
static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

// Takes as input a register and reads the four byte word in it.
static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}

// This is just a busy loop written in assembly.
// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}

// Defines perpheral offset of GPIO and UART and some of their registers.
enum
{
    // The GPIO registers base address.
    GPIO_BASE = 0x3F200000, // for raspi2 & 3, 0x20200000 for raspi1

    GPPUD = (GPIO_BASE + 0x94),
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE = 0x3F201000, // for raspi2 & 3, 0x20201000 for raspi1

    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};

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

// The heart of the kernel where stuff happens.
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    // These three parameters are the initialization parameters of r0, r1, and r2 by convention.
    (void) r0;
    (void) r1;

    // Contains a pointer to the atags, which is info about the hardware and commandline used to run the kernel in memory.
    (void) atags;
    
    // Initializes UART.
    uart_init();

    // Put "Hello World" to UART
    uart_puts("Hello, kernel World!\r\n");
    
    // Forever, just get UART data (which will be the keyboard input in this case) and put it to UART (which will print to our terminal). 
    while (1) {
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}
