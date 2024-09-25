#include <kernel/uart.h>
#include <common/stdio.h>
#include <common/stdlib.h>

// Generalized I/O functions
char getc(void) {
	return uart_getc();
}
void putc(char c) {
	uart_putc(c);
}
void puts(const char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		putc(str[i]);
	}
}

// Get a string from the user.
// buf is the buffer where the text will go.
// buflen is how many characters to allow
void gets(char* buf, int buflen) {
	int i;
	char c;
	// Either a carriage return indicates that user is done inputting or the buffer is out of space (keeping one for the null character)
	for (i = 0; (c = getc()) != '\r' && buflen > 1; i++, buflen--) {
		// Display the character
		putc(c);
		// Save the character to the buffer
		buf[i] = c;
	}
	
	// Print a new line to the screen.
	putc('\n');

	// If the last character read was a newline,
	if (c == '\n') {
		// Set this last character to be a null character instead
		buf[i] = '\0';
	}
	else
		buf[buflen-1] = '\0'; // Add the null character to the end of the string
}
