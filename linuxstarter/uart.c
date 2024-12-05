#include <stdint.h>

#include "uart.h"

#define THR 0
#define LSR 5

#define LSR_TRE 32

void printchar(char c)
{
	volatile uint32_t *uart = (void *) 0xB8002000;

	if (c == '\n') {
		printchar('\r');
	}

	while(((uart[LSR] >> 24) & LSR_TRE) != LSR_TRE) {
		/* Wait until transmit FIFO is empty. */
	}

	uart[THR] = c << 24;
}

void prints(char *text)
{
	while(*text != 0) {
		printchar(*text);
		text++;
	}
}

static void print_digit(int i)
{
    i = i & 0x0f;

    if (i < 10) {
        printchar('0' + i);
    } else {
        printchar('a' + i - 10);
    }
}

void printx(unsigned int i)
{
    printchar('0');
    printchar('x');
    print_digit(i >> 28);
    print_digit(i >> 24);
    print_digit(i >> 20);
    print_digit(i >> 16);
    print_digit(i >> 12);
    print_digit(i >> 8);
    print_digit(i >> 4);
    print_digit(i >> 0);
}

void printxx(unsigned long long i)
{
    printchar('0');
    printchar('x');
    print_digit(i >> 60);
    print_digit(i >> 56);
    print_digit(i >> 52);
    print_digit(i >> 48);
    print_digit(i >> 44);
    print_digit(i >> 40);
    print_digit(i >> 36);
    print_digit(i >> 32);
    print_digit(i >> 28);
    print_digit(i >> 24);
    print_digit(i >> 20);
    print_digit(i >> 16);
    print_digit(i >> 12);
    print_digit(i >> 8);
    print_digit(i >> 4);
    print_digit(i >> 0);
}

void printd(unsigned int i)
{
    if (i >= 10) {
        unsigned int n;
#ifndef __THUMBEL__
        n = i % 10;
        i = i / 10;
#else
        n = i;
        i = 0;
        while (n > 10) {
            n -= 10;
            i++;
        }
#endif
        printd(i);
        printchar('0' + n);
    } else {
        printchar('0' + i);
    }
}
