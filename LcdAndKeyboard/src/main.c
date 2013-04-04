#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

#include "keyb.h"
#include "menuManager.h"
#include "menuManagerStates.h"


int main()
{
   uint8_t key = 69;
   uint8_t count = 0;
   uint8_t releasedCount = 0;
   uint8_t gain = 0;

   menuManager menu;
   menuState menuState[3];
   keyboardUint8InputContext inputCtx;
   simpleMenuContext simpleMenuCtx;
   displayUint8Context displayContext;

   menu.currentState = 0;
   menu.states = &menuState[0];

   menuState[0].onkey=&simpleMenu;
   menuState[0].transitions[0]=0;
   menuState[0].transitions[1]=0;
   menuState[0].transitions[2]=1;
   menuState[0].transitions[3]=2;
   menuState[0].stateContext=&simpleMenuCtx;
   simpleMenuCtx.currsor=0;
   simpleMenuCtx.entryAmt=3;
   simpleMenuCtx.entries[0]="Do Nothing";
   simpleMenuCtx.entries[1]="Edit Gain";
   simpleMenuCtx.entries[2]="View Gain";

   menuState[1].onkey=&keyboardUint8Input;
   menuState[1].transitions[0]=2;
   menuState[1].transitions[1]=2;
   menuState[1].stateContext=&inputCtx;
   inputCtx.tmpBuffer=0;
   inputCtx.variableToChange=&gain;

   menuState[2].onkey=&displayUint8;
   menuState[2].transitions[0]=0;
   menuState[2].transitions[1]=1;
   menuState[2].stateContext=&displayContext;
   displayContext.title="Gain:";
   displayContext.variableToDisplay=&gain;


   //Initialize LCD module
   LCDInit(0);

   //Clear the screen
   LCDClear();

   //Simple string printing
   LCDWriteString("Salimooo ");
   _delay_ms(1000);


   //Clear the screen
   LCDClear();
   //Force initial paint of menu
   onKey(&menu,MENU_MANAGER_KEY_NOKEY);

   while (1){
	   key = GetKeyPressed();

	   if(key != KEY_NOKEY && releasedCount < 150 ) continue;

	   switch (key){
	   	   case (KEY_NOKEY):
	   	   	   	if (releasedCount < 200){
	   	   	   		releasedCount++;
	   	   	   	}
	   	   	   	break;
	   	   default:
	   		   releasedCount = 0;
	   		   count++;
	   		   onKey(&menu,convertKeyToNumberOrKey(key));
	   		   break;
	   }
	}
}
