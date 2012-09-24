#include "keyb.h"

uint8_t convertKeyToNumber(uint8_t number){
	uint8_t result = number + 1;
	if (result == 11){
		return 0;
	}
	return result;
}

/*
 * Obtains the key pressed.
 * the result does not map to text in the buttons but to the following code
 *
 * [0][1][2]
 * [3][4][5]
 * [6][7][8]
 * [9][10][11]
 */
uint8_t GetKeyPressed()
{
   uint8_t r,c;

   //KEYPAD_PORT|= 0X0F;
   KEY_ROW_PORT |= 0b00001111 << ROW_PORT_SHIFT;
   KEY_COL_PORT |= (0b00000111 << COL_PORT_SHIFT);

   for(c=0;c<3;c++)
   {
      //KEYPAD_DDR&=~(0X7F);
      KEY_ROW_DDR &= ~(0b00001111 << ROW_PORT_SHIFT);
      KEY_COL_DDR &= ~(0b00000111 << COL_PORT_SHIFT);

      //KEY_COL_DDR |= (0x01 << (COL_PORT_SHIFT + c));

      //KEYPAD_DDR|=(0X40>>c);
      for(r=0;r<4;r++)
      {

    	 KEY_ROW_DDR |= (0b00000001 << (ROW_PORT_SHIFT+r));
    	 KEY_ROW_PORT &= ~(0b00000001 << (ROW_PORT_SHIFT+r));
    	 _delay_us(1);
    	 if(!(KEY_COL_PIN & (0X01<<(COL_PORT_SHIFT + c))))
         {
    		  KEY_ROW_DDR &= ~(0b00001111 << ROW_PORT_SHIFT);
    		  KEY_ROW_PORT &= ~(0b00001111 << ROW_PORT_SHIFT);
    		  return (r*3+c);
         }
    	 KEY_ROW_DDR &= ~(0b00001111 << ROW_PORT_SHIFT);
    	 KEY_ROW_PORT &= ~(0b00001111 << ROW_PORT_SHIFT);
      }
   }

   return 0XFF;//Indicate No key pressed
}
