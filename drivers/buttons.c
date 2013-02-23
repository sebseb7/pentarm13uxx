
#include "buttons.h"

static uint16_t key_state;
static uint16_t key_press;


void buttons_sample(void)
{
	static uint16_t ct0, ct1;
	
	uint16_t key_curr =	(LPC_GPIO->B0[22])|((LPC_GPIO->B0[1])<<1)|((LPC_GPIO->B1[13])<<2);

	uint16_t i = key_state ^ ~key_curr;
	
	ct0 = ~( ct0 & i );
	ct1 = ct0 ^ (ct1 & i);
	i &= ct0 & ct1;
	key_state ^= i;
	key_press |= key_state & i;
}
	
//C6 as button input
void buttons_init(void)
{
	LPC_IOCON->PIO0_22  |= 1<<4;
	LPC_IOCON->PIO0_1   |= 1<<4;
	LPC_IOCON->PIO1_13  |= 1<<4;
}


uint16_t buttons_get_press( uint16_t key_mask )
{
	key_mask &= key_press;                          // read key(s)
	key_press ^= key_mask;                          // clear key(s)
	return key_mask;
}

uint16_t buttons_get_state( uint16_t key_mask )
{
	return key_mask & key_state;
}

