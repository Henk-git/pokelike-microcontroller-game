#include "uart.h"
#include <avr/io.h>

// USART initialisieren
void uart_init(uint32_t baud)
{
	UCSR0A = (1 << U2X0);
	uint16_t ubrr = F_CPU / 8 / baud - 1;
	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// ein Zeichen senden
void uart_putc(char c)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

// String senden
void uart_print(const char *s)
{
	while (*s)
		uart_putc(*s++);
}

// String mit Zeilenumbruch senden
void uart_println(const char *s)
{
	uart_print(s);
	uart_putc('\r');
	uart_putc('\n');
}

// Zahl als String senden (bis zu 3-stellig)
void uart_print_int(int16_t n)
{
	if (n < 0)
	{
		uart_putc('-');
		n = -n;
	}
	if (n >= 100)
		uart_putc('0' + n / 100);
	if (n >= 10)
		uart_putc('0' + (n / 10) % 10);
	uart_putc('0' + n % 10);
}