#include "reverse.h"


uint8_t reverse8(uint8_t in)
{
	in = (in & 0x0F) << 4 | (in & 0xF0) >> 4;
	in = (in & 0x33) << 2 | (in & 0xCC) >> 2;
	in = (in & 0x55) << 1 | (in & 0xAA) >> 1;
	return in;
}


