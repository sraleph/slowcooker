/*
 * menuManagerStates.h
 *
 *  Created on: Apr 3, 2013
 *      Author: root
 */

#ifndef MENUMANAGERSTATES_H_
#define MENUMANAGERSTATES_H_

#ifndef MENUMANAGER_MAX_MENU_OPTS
#define MENUMANAGER_MAX_MENU_OPTS 10
#endif

uint8_t keyboardUint8Input(uint8_t key, void * context);
uint8_t simpleMenu(uint8_t key, void * context);
uint8_t displayUint8(uint8_t key, void * context);

typedef struct keyboardUint8InputContext_t{
	uint8_t tmpBuffer;
	uint8_t * variableToChange;
} keyboardUint8InputContext;

typedef struct displayUint8Context_t{
	uint8_t * variableToDisplay;
	char * title;
} displayUint8Context;


typedef struct simpleMenuContext_t{
	char * entries[MENUMANAGER_MAX_MENU_OPTS];
	uint8_t entryAmt;
	uint8_t currsor;
} simpleMenuContext;


#endif /* MENUMANAGERSTATES_H_ */
