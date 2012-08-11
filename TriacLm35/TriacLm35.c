#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

static volatile bool fire = false;
static uint8_t count = 0;
static double delay = 1;
static int32_t integral = 0, prev_error = 0;

#define SENSOR_CHANNEL 5

#define MAX_DELAY 9000
#define MIN_DELAY 1000

#define FOSC 2000000
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

uint16_t readADC(uint8_t ch);
double getDelayFromPercetage(uint16_t power);
uint16_t pid_correct(int16_t currentTemp, uint16_t expectedTemp);

void uart_init(void);
static int uart_putchar(char c, FILE *stream);
uint8_t uart_getchar(void);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);


int main(void) {

	uint16_t expectedTemp;
	uint16_t currentTempRead = 4;
	uint16_t correction = 0;

	/* Define Output/Input Stream */
	stdout= stdin = &uart_str;
	uart_init();

	/* ADC Setup*/ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0); //Enable ADC with Prescalar=Fcpu/16 (2Mhz/16=62500 hz)

	DDRC = _BV(PC0); /* PC0 is digital output (alive signal) */
	DDRC |= _BV(PC1); /* PC1 is digital output (triac) */

	PORTC &= ~_BV(PC1); // Initialize triac off

	DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
	// PD2 (INT0 pin) is now an input

	PORTD |= (1 << PORTD2);    // turn On the Pull-up
	// PD0 is now an input with pull-up enabled

	// set INT0 to trigger on Both edges
	MCUCR |= (1 << ISC00);

	GICR |= (1 << INT0);      // Turns on INT0

	expectedTemp = readADC(SENSOR_CHANNEL) + 4;

	sei();
	// turn on interrupts

	while (1) {
		if (fire == true) {
			fire = false;
			count++;
			if (count == 0) {
				PORTC &= ~_BV(PC0);
				correction = pid_correct(currentTempRead, expectedTemp);
				delay = getDelayFromPercetage(correction);
			} else if (count == 128) {
				PORTC |= _BV(PC0);
			}

			_delay_us(delay - 500);
			PORTC |= _BV(PC1);
			_delay_us(500);
			PORTC &= ~_BV(PC1);

			// Read ADC as a last thing so that AC sync is not affected
			currentTempRead = readADC(SENSOR_CHANNEL);
			if (count == 0) {
				printf("\n\nDelay:  %f \n", delay);
				printf("\n\nCorrection: %d  \n", correction);
			}

		}
	}
}

ISR( INT0_vect) {
	//fire once ever 256 cycles (5seg)
	fire = true;
}

uint16_t readADC(uint8_t ch) {
	//Select ADC Channel ch must be 0-7
	ch = ch & 0b00000111;
	ADMUX |= ch;

	//Start Single conversion
	ADCSRA |= (1 << ADSC);

	//Wait for conversion to complete
	while (!(ADCSRA & (1 << ADIF)))
		;

	//Clear ADIF by writing one to it
	ADCSRA |= (1 << ADIF);

	return (ADC );
}

uint16_t pid_correct(int16_t currentTemp, uint16_t expectedTemp) {

	//the plant error
	//make sure error is positive when you need to apply a larger pwm value (wider positive width)
	int16_t error = expectedTemp - currentTemp;
	//int16_t error = ((int16_t) SET_POINT) - plant_value;

	//proportional * pGain
	int32_t proportional = error * 200;

	//integral * iGain
	int32_t temp_sum = integral + (error * 10);
	//check that the integral doesn't get out of the desired range
	if (temp_sum > 0xFFFF)
		integral = 0xFFFF;
	else if (temp_sum < (-0xFFFF))
		integral = -0xFFFF;
	else
		integral = temp_sum;

	//differential * dGain
	int32_t differential = ((prev_error - error) * 20);

	//the final sum, with an overflow check
	int32_t temp_pid_sum = proportional + integral + differential;
	uint16_t pid_total;
	//set max pwm value at ~95% of 2^16 (change this to whatever you like)
	if (temp_pid_sum > 62258)
		pid_total = 62258;
	else if (temp_pid_sum < 0)
		pid_total = 0;
	else
		pid_total = (uint16_t) temp_pid_sum;

	//store the previous error
	prev_error = error;
	return pid_total;
}

double getDelayFromPercetage(uint16_t power) {
	float percentageDelay = 1 - power / 65535.0;
	return (double) percentageDelay * (MAX_DELAY - MIN_DELAY) + MIN_DELAY;
}

void uart_init(void) {
	UBRRH = (((F_CPU / BAUD_RATE) / 16) - 1) >> 8;		// set baud rate
	UBRRL = (((F_CPU / BAUD_RATE) / 16) - 1);

	/* Enable Tx and Rx */UCSRB = (1 << RXEN) | (1 << TXEN);

	/* Set Frame: Data 8 Bit, No Parity and 1 Stop Bit */
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

int uart_putch(char ch, FILE *stream) {
	if (ch == '\n')
		uart_putch('\r', stream);

	while (!(UCSRA & (1 << UDRE)))
		;
	UDR = ch;

	return 0;
}

int uart_getch(FILE *stream) {
	unsigned char ch;

	while (!(UCSRA & (1 << RXC)))
		;
	ch = UDR;

	/* Echo the output back to the terminal */
	uart_putch(ch, stream);

	return ch;
}
