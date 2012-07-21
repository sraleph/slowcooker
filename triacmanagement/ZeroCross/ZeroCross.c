#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

static volatile bool fire = false;
static uint8_t count = 0;
static uint8_t delay = 0;


int main(void) {
	DDRC = _BV(PC0); /* PC0 is digital output (alive signal) */
	DDRC |= _BV(PC1); /* PC1 is digital output (triac) */

	PORTC &= ~_BV(PC1); // Initialize triac off

	DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
	// PD2 (INT0 pin) is now an input

	PORTD |= (1 << PORTD2);    // turn On the Pull-up
	// PD0 is now an input with pull-up enabled

	// set INT0 to trigger on Falling edge
//	MCUCR |= (0 << ISC00);
//	MCUCR |= (1 << ISC01);
	MCUCR |= (1 << ISC00);

	GICR |= (1 << INT0);      // Turns on INT0

	sei();
	// turn on interrupts

	while (1) {
		if (fire == true) {
			fire = false;
			count++;
			if (count == 0) {
				/* set PC0 on PORTC (digital high) and delay for 500mS */
				PORTC &= ~_BV(PC0);
				if (delay <= 8) {
					delay++;
				} else {
					delay = 1;
				}
			} else if (count == 128){
				/*  PC0 on PORTC (digital 0) and delay for 500mS */
				PORTC |= _BV(PC0);
			}
			_delay_ms(delay-1);
			_delay_us(500);
			PORTC |= _BV(PC1);
			_delay_us(500);
			PORTC &= ~_BV(PC1);


		}
	}
}

ISR( INT0_vect) {
	//fire once ever 256 cycles (5seg)
	fire = true;
}
