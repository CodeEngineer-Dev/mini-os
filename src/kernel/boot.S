# Specify where the code belongs in the compiled binary
.section ".text.boot"

# _start should be accessible from outside this file
.global _start

# Begin section start
_start:
    # Halt three of the four cores

    # mrc is the move to coprocessor register command.
    # It transfers data from a coprocessor register to an ARM register
    # A coprocessor is a specialized processing unit in the CPU that performs a specific task
    # This specific command reads the CPU ID register into register r1. The CPUID identifies the CPU.
    mrc p15, #0, r1, c0, c0, #5

    # Performs an "and" bitwise operation over every bit of the value in r1 and stores it back into r1. This masks out the everything except the last two bits (by using the value 3)
    and r1, r1, #3

    # Compares the value in r1 with zero.
    cmp r1, #0

    # If they are not equal, halt the core.
    bne halt
    
    
    # This next section initializes the stack


    # The C stack should start at address 0x8000 and it grows downward
    # sp is the stack pointer register, and it keeps track of the location of current top of the stack
    mov sp, #0x8000
    
    # Load into the register the addresses of the BSS section's start and end
    # BSS is where C global variables uninitialized at compile time are stored
    ldr r4, =__bss_start
    ldr r9, =__bss_end

    # C requires that all values in the BSS are initialized to zero, so this is where that is done. Start by initializing 4 registers to zero.
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0

    # Branch to section 2
    b       2f

# Begins section 1. This zeros out a section of memory
1:
    # st stores the data in the second operand into memory starting at the location of the first operand.
    # m means that multiple registers will be stored
    # ia means increment after, or increment to the address after the last written address. r4 will be updated to point the next address
    # ! indicates store it back into the register instead of throwing it out
    # {r5-r8} indicates that the values of r5-r8 should all be stored in r4
    # so this effectively zeroes out 16 bytes of memory
    stmia r4!, {r5-r8}

# Begins section 2. This section compares the address in register 4 (bss start) to register 9 (bss end) and handles it accordingly
2:
    # Compare the current start to the bss end
    cmp r4, r9

    # Still lower? Jump to section 1
    blo 1b
    
    # Otherwise, load the address of the kernel into r3
    ldr r3, =kernel_main

    # Branch and link. Jumps to the location stored in r3, which in this case is the kernel
    blx r3

# Forever loop that does nothing once the C function returns
halt:
    # Wait for event (low power mode / sleep state)
    wfe

    # Branch to halt again
    b halt
