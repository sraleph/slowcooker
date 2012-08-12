#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

static uint8_t count = 0;
static double delay = 1;
static int32_t integral = 0, prev_error = 0;

#define SENSOR_CHANNEL 5

#define MAX_DELAY 8000
#define MIN_DELAY 1000

#define FOSC 8000000
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

	_delay_ms(100);
	expectedTemp = readADC(SENSOR_CHANNEL) + 4;
	printf("Initial expected temp:  %u\n", expectedTemp);

	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR1A

	TIMSK |= (1 << OCIE1A);
	//Set interrupt on compare match


	while (1) {
		count++;

		_delay_ms(100);

		currentTempRead = readADC(SENSOR_CHANNEL);
		correction = pid_correct(currentTempRead, expectedTemp);
		delay = getDelayFromPercetage(correction);
		cli();
		OCR1A = (uint16_t)(delay / 128.0);
		sei();
		printf("Delay,  %u , Correction, %u, Temp,  %u \n",
				(unsigned int) delay, (unsigned int) correction,
				(unsigned int) currentTempRead);

		if (count == 0) {
			PORTC &= ~_BV(PC0);
		} else if (count == 128) {
			PORTC |= _BV(PC0);
		}

	}
}

ISR( INT0_vect) {
	TCCR1B |= (1 << CS12) | (1 << CS10);
	// set prescaler to 1024 and start the timer
	//UDR = 'b';
}

ISR (TIMER1_COMPA_vect) {
	// stop timer
	TCCR1B &= ~((1 << CS12) | (1 << CS10));

	//generate triac activating pulse
	PORTC |= _BV(PC1);
	_delay_us(40);
	PORTC &= ~_BV(PC1);
	//UDR = 'a';
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
	//printf("Proportional, %lld , Differential, %lld, Integral, %lld", (long long int)proportional,  (long long int)differential,  (long long int)integral);
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
	DDRD = 0b11111110; //PORTD (RX on PD0)

	//USART Baud rate: 9600
	UBRRH = MYUBRR >> 8;
	UBRRL = MYUBRR;
	UCSRB = (1 << RXEN) | (1 << TXEN);
	/* Set frame format: 8data, 2stop bit */UCSRC = (1 << URSEL) | (1 << USBS)
			| (3 << UCSZ0);
	stdout= &mystdout; //Required for printf init
}

static int uart_putchar(char c, FILE *stream) {
	if (c == '\n')
		uart_putchar('\r', stream);

	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;

	return 0;
}

uint8_t uart_getchar(void) {
	while (!(UCSRA & (1 << RXC)))
		;
	return (UDR );
}

