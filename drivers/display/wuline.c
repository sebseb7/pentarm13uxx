/*
 * modified version of : http://rosettacode.org/wiki/Xiaolin_Wu's_line_algorithm
 */


// 3k code on avr


#include "stdio.h"
#include "main.h"
#include <math.h>
#include "drivers/ssd1351.h"

static void dla_plot(int x, int y, uint8_t r,uint8_t g , uint8_t b, float br)
{
	uint8_t o_red=0;
	uint8_t o_green=0;
	uint8_t o_blue=0;

//	getLedXY(x,y,&o_red,&o_green,&o_blue);

	r=br*r+((1-br)*o_red);
	g=br*g+((1-br)*o_green);
	b=br*b+((1-br)*o_blue);
	setLedXY(x, y, r,g,b);


}

#define ipart_(X) ((int)(X))
#define round_(X) ((int)(((double)(X))+0.5))
#define fpart_(X) (((double)(X))-(double)ipart_(X))
#define rfpart_(X) (1.0-fpart_(X))

#define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)
void draw_line(
	unsigned int x1, unsigned int y1,
	unsigned int x2, unsigned int y2,
	uint8_t r,
	uint8_t g,
	uint8_t b )
{
	double dx = (double)x2 - (double)x1;
	double dy = (double)y2 - (double)y1;
	if ( fabs(dx) > fabs(dy) ) 
	{
		if ( x2 < x1 ) 
		{
			swap_(x1, x2);
			swap_(y1, y2);
		}
		double gradient = dy / dx;
		double xend = round_(x1);
		double yend = y1 + gradient*(xend - x1);
		double xgap = rfpart_(x1 + 0.5);
		int xpxl1 = xend;
		int ypxl1 = ipart_(yend);
		dla_plot(xpxl1, ypxl1, r,g,b,rfpart_(yend)*xgap);
		dla_plot(xpxl1, ypxl1+1, r,g,b,fpart_(yend)*xgap);
		double intery = yend + gradient;

		xend = round_(x2);
		yend = y2 + gradient*(xend - x2);
		xgap = fpart_(x2+0.5);
		int xpxl2 = xend;
		int ypxl2 = ipart_(yend);
		dla_plot(xpxl2, ypxl2, r,g,b,rfpart_(yend) * xgap);
		dla_plot(xpxl2, ypxl2 + 1, r,g,b,fpart_(yend) * xgap);

		int x;
		for(x=xpxl1+1; x <= (xpxl2-1); x++) 
		{
			dla_plot(x, ipart_(intery), r,g,b,rfpart_(intery));
			dla_plot(x, ipart_(intery) + 1, r,g,b,fpart_(intery));
			intery += gradient;
		}
	
	} 
	else 
	{
	
		if ( y2 < y1 ) 
		{
			swap_(x1, x2);
			swap_(y1, y2);
		}
		double gradient = dx / dy;
		double yend = round_(y1);
		double xend = x1 + gradient*(yend - y1);
		double ygap = rfpart_(y1 + 0.5);
		int ypxl1 = yend;
		int xpxl1 = ipart_(xend);
		dla_plot(xpxl1, ypxl1, r,g,b,rfpart_(xend)*ygap);
		dla_plot(xpxl1, ypxl1+1, r,g,b,fpart_(xend)*ygap);
		double interx = xend + gradient;
	
		yend = round_(y2);
		xend = x2 + gradient*(yend - y2);
		ygap = fpart_(y2+0.5);
		int ypxl2 = yend;
		int xpxl2 = ipart_(xend);
		dla_plot(xpxl2, ypxl2, r,g,b,rfpart_(xend) * ygap);
		dla_plot(xpxl2, ypxl2 + 1, r,g,b,fpart_(xend) * ygap);
	
		int y;
		for(y=ypxl1+1; y <= (ypxl2-1); y++) 
		{
			dla_plot(ipart_(interx), y, r,g,b,rfpart_(interx));
			dla_plot(ipart_(interx) + 1, y, r,g,b,fpart_(interx));
			interx += gradient;
		}
	}
}
#undef swap_
#undef ipart_
#undef fpart_
#undef round_
#undef rfpart_


