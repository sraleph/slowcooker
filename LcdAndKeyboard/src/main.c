#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

#include "keyb.h"

#define LED PC1
// Some macros that make the code more readable
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

int main()
{
   uint8_t key = 69;
   uint8_t count = 0;

   //set_output(DDRC, LED);
   //Initialize LCD module
   /*LCDInit(LS_BLINK|LS_ULINE);*/
   LCDInit(LS_BLINK|LS_ULINE);

   //Clear the screen


   LCDClear();

   //Simple string printing

   LCDWriteString("Salimooo ");
   _delay_ms(1000);


//   while (1) {
//     // turn on the LED for 200ms
//     output_high(PORTC, LED);
//     _delay_ms(200);
//     // now turn off the LED for another 200ms
//     output_low(PORTC, LED);
//     _delay_ms(200);
//     // now start over
//   }
   //Clear the screen
   LCDClear();

   //Some more text
   _delay_ms(500);

   while (1){
	   count++;
	   //LCDClear();
	   key = GetKeyPressed();
	   LCDWriteStringXY(0,0,"Count:");
	   LCDWriteInt(count,3);
	   switch (key){
	   	   case (KEY_SHARP):
			   LCDWriteStringXY(0,1,"#        ");
	   			break;
	   	   case (KEY_STAR):
				LCDWriteStringXY(0,1,"*       ");
			   	break;
	   	   case (KEY_NOKEY):
				LCDWriteStringXY(0,1,"NO KEY");
	   	   	    break;
	   	   default:
	   		   LCDWriteStringXY(0,1,"          ");
	   		   LCDGotoXY(0,1);
	   		   LCDWriteInt(convertKeyToNumber(key),1);
	   		   //LCDWriteInt(key,2);
	   		   break;

	   }
	   //_delay_ms(100);

   }

}

