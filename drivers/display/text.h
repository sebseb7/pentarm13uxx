#ifndef _TEXT_H
#define _TEXT_H

#include <stdint.h>

void draw_text_8x6(uint8_t x, uint8_t y, const char *text, uint8_t r,uint8_t g,uint8_t b);
void draw_number_8x6(uint8_t x, uint8_t y, int32_t number, uint8_t length, uint8_t pad, uint8_t r, uint8_t g , uint8_t b);


#endif
