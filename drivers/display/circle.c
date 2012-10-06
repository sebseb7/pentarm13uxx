#include "stdio.h"
#include <math.h>
#include "drivers/ssd1351.h"

static void dla_plot(int x, int y, uint8_t r,uint8_t g , uint8_t b, float br)
{
	uint8_t o_red = 0;
	uint8_t o_green = 0;
	uint8_t o_blue = 0;

//	getLedXY(x,y,&o_red,&o_green,&o_blue);

	r=br*r+((1-br)*o_red);
	g=br*g+((1-br)*o_green);
	b=br*b+((1-br)*o_blue);
	setLedXY(x, y, r,g,b);


}
static unsigned isqrt(unsigned val)
{
	unsigned temp, g = 0, b = 0x8000, bshft = 15;
	do {
		if (val >= (temp = (((g << 1) + b)<<bshft--))) {
			g += b;
			val -= temp;
		}
	} while (b >>= 1);
	return g;
}


static unsigned pythagoras( unsigned side1, unsigned side2 )
{
	return isqrt(pow( side1, 2 ) + pow( side2, 2 ));
}


// uncompleted
#define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)
void draw_filledCircleSlice(
	unsigned int x, unsigned int y,
	double rad,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint16_t slice_begin,
	uint16_t slice_end)
{

	uint8_t i,j;


	if(slice_begin > slice_end)
	{
		swap_(slice_begin,slice_end);
	}

	for(i=0;i<(rad*2);i++)
	{
		for(j=0;j<(rad*2);j++)
		{
	
			double dist = pythagoras( j,i );

			if(dist <= rad-1)
			{
				setLedXY(y-j,x+i,r,g,b);
			}else if(dist < rad)
			{
			//	dla_plot(y-j,x+i,r,g,b,1-(dist-rad+1));
			}
		}
	}
}
#undef swap_

void draw_filledCircle(
	unsigned int x, unsigned int y,
	double rad,
	uint8_t r,
	uint8_t g,
	uint8_t b )
{

	uint8_t i,j;

	for(i=0;i<rad;i++)
	{
		for(j=0;j<(i+1);j++)
		{
			unsigned dist = pythagoras( j,i );
			if(dist <= rad-1)
			{
				setLedXY(y-j,x+i,r,g,b);
				setLedXY(y+j,x+i,r,g,b);
				setLedXY(y+j,x-i,r,g,b);
				setLedXY(y-j,x-i,r,g,b);

				setLedXY(y-i,x-j,r,g,b);
				setLedXY(y-i,x+j,r,g,b);
				setLedXY(y+i,x+j,r,g,b);
				setLedXY(y+i,x-j,r,g,b);

			}else if(dist < rad)
			{
				dla_plot(y-j,x+i,r,g,b,1-(dist-rad+1));
				dla_plot(y+j,x+i,r,g,b,1-(dist-rad+1));
				dla_plot(y+j,x-i,r,g,b,1-(dist-rad+1));
				dla_plot(y-j,x-i,r,g,b,1-(dist-rad+1));
				
				dla_plot(y-i,x+j,r,g,b,1-(dist-rad+1));
				dla_plot(y+i,x+j,r,g,b,1-(dist-rad+1));
				dla_plot(y+i,x-j,r,g,b,1-(dist-rad+1));
				dla_plot(y-i,x-j,r,g,b,1-(dist-rad+1));
			}
		}
	}



}


