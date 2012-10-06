#ifndef _CIRCLE_H
#define _CIRCLE_H

void draw_filledCircleSlice(
	unsigned int x, unsigned int y,
	double rad,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint16_t slice_begin,
	uint16_t slide_end );

void draw_filledCircle(
	unsigned int x, unsigned int y,
	double rad,
	uint8_t r,
	uint8_t g,
	uint8_t b );

#endif

