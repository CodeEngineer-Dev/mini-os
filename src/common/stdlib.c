#include <common/stdlib.h>

// Memory copy function. Takes a destination address, a source address, and the number of bytes to copy.
void memcpy(void* dest, void* src, int bytes) {
	// One byte for the address and the source
	char* d = dest, *s = src;

	// While there are still bytes to copy
	while (bytes--) {
		// Set the value at d to equal the value at s and increment the memory addresses by one byte.
		*d++ = *s++;
	}
}

// Function that zeroes a section of memory. Takes a destination address and the number of bytes to zero.
void bzero(void* dest, int bytes) {
	char* d = dest;
	while (bytes--) {
		// Set the value at the destination address equal to zero and increase the address d by one.
		*d++ = 0;
	}
}

// Convert integer to ASCII, will be useful for debugging.
char* itoa(int i) {
	// An section of memory as an array that will hold the ASCII character.
	static char intbuf[12];

	// Some utility variables.
	int j = 0, isneg = 0;

	// If the number is equal to zero, just set the first byte to zero and the second byte to the null character and that's it.
	if (i == 0) {
		intbuf[0] = '0';
		intbuf[1] = '\0';
		return intbuf;
	}

	// If the number is negative then note that for later and make the number positive.
	if (i < 0) {
		isneg = 1;
		i = -1;
	}

	// While i is greater than zero
	while (i != 0) {
		// Set the current position in the intbuf to ASCII value ('0' + remainder of i/10). '0' corresponds to 48. Then increment j.
		intbuf[j++] = '0' + (i % 10);

		// Divide off the last digit (this works because i is an int and cuts off any remainders).
		i /= 10;
	}

	// If it is negative add a negative sign to the end of the buffer.
	if (isneg)
		intbuf[j++] = '-';

	// Terminating character at the end.
	intbuf[j] = '\0';

	// Start with the first character we want to display.
	j--;

	// Start i at the beginning of the array, which has the last character we want to swap out.
	i = 0;

	// This takes the first and last untouched characters, swaps them, and then moves to the next untouched characters. Essentially reverses the entire array without touching the '\0' at the end.
	while (i < j) {
		isneg = intbuf[i];
		intbuf[i] = intbuf[j];
		intbuf[j] = isneg;
		i++;
		j--;
	}

	// This is the ASCII string we want to return.
	return intbuf;
}
