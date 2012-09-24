#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

#include "keyb.h"





void main()
{
   uint8_t key = 69;
   uint8_t count = 0;

   //Initialize LCD module
   /*LCDInit(LS_BLINK|LS_ULINE);*/
   LCDInit(0);

   //Clear the screen
   LCDClear();

   //Simple string printing
   LCDWriteString("Congrats ");

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
	   		   break;

	   }
	   //_delay_ms(100);

   }

}

