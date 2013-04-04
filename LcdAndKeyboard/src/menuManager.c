#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

#include "keyb.h"
#include "menuManager.h"

uint8_t executeCurrentState(menuManager* self, uint8_t key) {
	return self->states[self->currentState].onkey(key, self->states[self->currentState].stateContext);
}

void onKey(menuManager * self ,uint8_t key){
	uint8_t keyResult = executeCurrentState(self, key);
	// 0 means stay in the same state other means state transition
	if (keyResult != 0){
		self->currentState = self->states[self->currentState].transitions[keyResult-1];
		executeCurrentState(self,MENU_MANAGER_KEY_NOKEY);
	}
}


