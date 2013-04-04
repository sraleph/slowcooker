/*
 * menuManagerStates.c
 *
 *  Created on: Apr 3, 2013
 *      Author: root
 */
#include <stdint.h>
#include "menuManagerStates.h"
#include "lcd.h"

#define MENU_MANAGER_KEY_STAR 0xFE
#define MENU_MANAGER_KEY_SHARP 0xFD


#define MENU_MANAGER_OUT_OK 2
#define MENU_MANAGER_OUT_BACK 1

uint8_t keyboardUint8Input(uint8_t key, void * context){
	keyboardUint8InputContext * ctx = context;
	if(key == MENU_MANAGER_KEY_SHARP){
		*ctx->variableToChange = ctx->tmpBuffer;
		ctx->tmpBuffer=0;
		return MENU_MANAGER_OUT_OK;
	}
	if(key==MENU_MANAGER_KEY_STAR){
		ctx->tmpBuffer=0;
		return MENU_MANAGER_OUT_BACK;
	}
	if (key<9){
		ctx->tmpBuffer=ctx->tmpBuffer*10+key;
	}
	LCDClear();
	LCDWriteStringXY(0,0,"INPUT VALUE");
	LCDWriteStringXY(0,1,"          ");
	LCDGotoXY(0,1);
	LCDWriteInt(ctx->tmpBuffer,3);
	return 0;
}

void writeSimpleMenuEntry(simpleMenuContext* ctx, uint8_t offset) {
	LCDWriteStringXY(0, offset, "          ");
	LCDGotoXY(0, offset);
	LCDWriteInt(ctx->currsor+offset, 1);
	LCDWriteString(":");
	LCDWriteString(ctx->entries[(ctx->currsor)+offset]);
}

uint8_t simpleMenu(uint8_t key, void * context){
	simpleMenuContext * ctx = context;
	if(key == MENU_MANAGER_KEY_SHARP){
		ctx->currsor=(ctx->currsor)+1;
		if(ctx->currsor==ctx->entryAmt -1){
			ctx->currsor=0;
		}
	}
	if(key==MENU_MANAGER_KEY_STAR){
		ctx->currsor=0;
		return MENU_MANAGER_OUT_BACK;
	}

	if(key<=ctx->entryAmt-1){
		return 2 + key;
	}
	LCDClear();
	writeSimpleMenuEntry(ctx,0);
	writeSimpleMenuEntry(ctx,1);
	return 0;
}

uint8_t displayUint8(uint8_t key, void * context){
	displayUint8Context * ctx = context;
	if(key == MENU_MANAGER_KEY_SHARP){
		return MENU_MANAGER_OUT_OK;
	}
	if(key==MENU_MANAGER_KEY_STAR){
		return MENU_MANAGER_OUT_BACK;
	}
	LCDClear();
	LCDWriteStringXY(0, 0,ctx->title);
	LCDGotoXY(0,1);
	LCDWriteInt(*(ctx->variableToDisplay),3);
	return 0;
}

