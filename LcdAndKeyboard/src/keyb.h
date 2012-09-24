#include <avr/io.h>
#include <util/delay.h>
#include "myutils.h"

#ifndef _KEYB_H
#define _KEYB_H

// Do change definitions to pinout
#define KEY_ROW D
#define KEY_COL C

#define COL_PORT_SHIFT 2
#define ROW_PORT_SHIFT 3

//Change according to mapping
#define KEY_STAR 9
#define KEY_SHARP 11


//Definitions not meant to be changed
#define KEY_NOKEY 0xFF

#define KEY_ROW_PORT PORT(KEY_ROW)
#define KEY_COL_PORT PORT(KEY_COL)

#define KEY_ROW_DDR DDR(KEY_ROW)
#define KEY_COL_DDR DDR(KEY_COL)

#define KEY_ROW_PIN PIN(KEY_ROW)
#define KEY_COL_PIN PIN(KEY_COL)

uint8_t convertKeyToNumber(uint8_t number);
uint8_t GetKeyPressed();

#endif
