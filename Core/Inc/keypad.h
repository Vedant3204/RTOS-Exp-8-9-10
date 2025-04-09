#ifndef KEYPAD_H
#define KEYPAD_H

#include "main.h"

/* Define GPIO Ports */
#define KEYPAD_ROW_PORT GPIOA
#define KEYPAD_COL_PORT GPIOA

/* Define ROW Pins */
#define ROW1_PIN GPIO_PIN_4
#define ROW2_PIN GPIO_PIN_5
#define ROW3_PIN GPIO_PIN_6
#define ROW4_PIN GPIO_PIN_7

/* Define COLUMN Pins */
#define COL1_PIN GPIO_PIN_0
#define COL2_PIN GPIO_PIN_1
#define COL3_PIN GPIO_PIN_2
#define COL4_PIN GPIO_PIN_3

/* Declare the keypad map (definition in keypad.c) */
extern const char keypad_map[4][4];

/* Function Prototypes */
void keypad_init(void);
char scan_keypad(void);
char keypad_get_key();

#endif /* KEYPAD_H */
