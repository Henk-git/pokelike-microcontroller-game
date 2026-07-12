#include "uart.h"
#include <avr/io.h>

// USART initialisieren
void uart_init(uint32_t baud) {
    uint16_t ubrr = F_CPU / 16 / baud - 1;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << TXEN0);
    // 8 Datenbits, 1 Stoppbit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// ein Zeichen senden
void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

// String senden
void uart_print(const char *s) {
    while (*s) uart_putc(*s++);
}

// String mit Zeilenumbruch senden
void uart_println(const char *s) {
    uart_print(s);
    uart_putc('\r');
    uart_putc('\n');
}

// Zahl als String senden
void uart_print_int(int8_t n) {
    if (n < 0) { uart_putc('-'); n = -n; }
    if (n >= 10) uart_putc('0' + n / 10);
    uart_putc('0' + n % 10);
}
