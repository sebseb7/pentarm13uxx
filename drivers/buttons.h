#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "main.h"

#define KEY_A (1<<0)
#define KEY_B (1<<1)
#define KEY_C (1<<2)


uint16_t buttons_get_press( uint16_t key_mask );
uint16_t buttons_get_state( uint16_t key_mask );
void buttons_sample(void);
void buttons_init(void);

#endif
