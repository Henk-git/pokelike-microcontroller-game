#include "input.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Flag wird in ISR gesetzt
static volatile uint8_t btn_flag = 0;

// ADC und INT0 initialisieren
void input_init(void) {
	// ADC: AVcc Referenz, Kanal 0 (PC0)
	ADMUX  = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// INT0: fallende Flanke (PD2), Pull-up aktivieren
	DDRD  &= ~(1 << PD2);
	PORTD |=  (1 << PD2);
	EICRA  = (1 << ISC01);
	EIMSK  = (1 << INT0);
	sei();
}

// ADC-Wert lesen
static uint16_t adc_read(void) {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

// Joystick-Richtung aus ADC-Wert ableiten
Direction joystick_direction(void) {
	uint16_t val = adc_read();
	if (val < 300)  return DIR_LEFT;
	if (val > 700)  return DIR_RIGHT;
	return DIR_CENTER;
}

// Tastendruck abfragen und Flag lschen
uint8_t button_pressed(void) {
	if (btn_flag) { btn_flag = 0; return 1; }
	return 0;
}

// Joystick-Knopf ISR
ISR(INT0_vect) {
	btn_flag = 1;
}