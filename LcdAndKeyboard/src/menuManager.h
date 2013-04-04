/*
 * menuManager.h
 *
 *  Created on: Apr 3, 2013
 *      Author: root
 */

#ifndef MENUMANAGER_H_
#define MENUMANAGER_H_

#ifndef MENUMANAGER_MAX_STATE_TRANSITIONS
#define MENUMANAGER_MAX_STATE_TRANSITIONS 4
#endif

#define MENU_MANAGER_KEY_NOKEY 0xFF

typedef struct menuState_t {
	void * stateContext;
	uint8_t (*onkey)(uint8_t, void *);
	uint8_t transitions [MENUMANAGER_MAX_STATE_TRANSITIONS];
} menuState;

typedef struct menuManager_t {

	menuState * states;
	uint8_t currentState;

} menuManager;

void onKey( menuManager* self, uint8_t key);

#endif /* MENUMANAGER_H_ */
