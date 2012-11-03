#include "ssd1351.h"

static lcdProperties_t ssd1351Properties = { 128, 128 };

/*************************************************/
/* Private Methods                               */
/*************************************************/

#define CMD(c)        do {   CLR_CS;CLR_DC; ssd1351SendByte( c );SET_CS;  } while (0)
#define DATA(c)       do {  CLR_CS;SET_DC; ssd1351SendByte( c ); SET_CS; } while (0);

void ssd1351SendByte(uint8_t byte)
{

#ifndef I6800

	int8_t i;

	// Write from MSB to LSB
	__ASM volatile ("nop");
	for (i=7; i>=0; i--) 
	{
		// Set clock pin low
		__ASM volatile ("nop");
		CLR_SCK;
		__ASM volatile ("nop");
		// Set data pin high or low depending on the value of the current bit
		if (byte & (1 << i))
		{
			SET_SID;
		}
		else
		{
			CLR_SID;
		}
		// Set clock pin high
		__ASM volatile ("nop");
		SET_SCK;
	}
	__ASM volatile ("nop");
#endif

#ifdef I6800


	SET_E;

	if (byte & (1 << 0))
	{
		SET_SCK;
	}
	else
	{
		CLR_SCK;
	}
	if (byte & (1 << 1))
	{
		SET_SID;
	}
	else
	{
		CLR_SID;
	}
	if (byte & (1 << 2))
	{
		SET_D2;
	}
	else
	{
		CLR_D2;
	}
	if (byte & (1 << 3))
	{
		SET_D3;
	}
	else
	{
		CLR_D3;
	}
	if (byte & (1 << 4))
	{
		SET_D4;
	}
	else
	{
		CLR_D4;
	}
	if (byte & (1 << 5))
	{
		SET_D5;
	}
	else
	{
		CLR_D5;
	}
	if (byte & (1 << 6))
	{
		SET_D6;
	}
	else
	{
		CLR_D6;
	}
	if (byte & (1 << 7))
	{
		SET_D7;
	}
	else
	{
		CLR_D7;
	}

	__ASM volatile ("nop");

	CLR_E;





#endif

}

/**************************************************************************/
/*! 
  @brief  Sets the cursor to the specified X/Y position
  */
/**************************************************************************/
void ssd1351SetCursor(uint8_t x, uint8_t y)
{
	if ((x >= ssd1351Properties.width) || (y >= ssd1351Properties.height))
		return;

	CMD(SSD1351_CMD_WRITERAM);
	CMD(SSD1351_CMD_SETCOLUMNADDRESS);
	DATA(x);                            // Start Address
	DATA(ssd1351Properties.width-1);    // End Address (0x7F)

	CMD(SSD1351_CMD_SETROWADDRESS);
	DATA(y);                            // Start Address
	DATA(ssd1351Properties.height-1);   // End Address (0x7F)
	CMD(SSD1351_CMD_WRITERAM);
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/**************************************************************************/
/*! 
  @brief  Configures any pins or HW and initialises the LCD controller
  */
/**************************************************************************/
void lcdInit(void)
{

	//Reset the LCD

	SET_RST;
	Delay(20);
	CLR_RST;
	Delay(200);
	SET_RST;
	Delay(20);

	CMD(SSD1351_CMD_SETCOMMANDLOCK);
	DATA(0x12);                               // Unlocked to enter commands
	CMD(SSD1351_CMD_SETCOMMANDLOCK);
	DATA(0xB1);                               // Make all commands accessible 
	CMD(SSD1351_CMD_SLEEPMODE_DISPLAYOFF);
	CMD(SSD1351_CMD_SETFRONTCLOCKDIV);
	DATA(0xf0);//was F1
	CMD(SSD1351_CMD_SETMUXRRATIO);
	DATA(0x7f);//7f
	CMD(SSD1351_CMD_COLORDEPTH);

#ifdef C262K
	DATA(0xB4);
#endif
#ifndef C262K
	//#warning 65k colors
	DATA(0x74);                               // Bit 7:6 = 65,536 Colors, Bit 3 = BGR or RGB
#endif
	CMD(SSD1351_CMD_SETCOLUMNADDRESS);
	DATA(0x00);
	DATA(0x7F);
	CMD(SSD1351_CMD_SETROWADDRESS);
	DATA(0x00);
	DATA(0x7F);
	CMD(SSD1351_CMD_SETDISPLAYSTARTLINE);
	DATA(0x00);
	CMD(SSD1351_CMD_SETDISPLAYOFFSET);
	DATA(0x00);
	CMD(SSD1351_CMD_SETGPIO);
	DATA(0x00);                               // Disable GPIO pins
	CMD(SSD1351_CMD_FUNCTIONSELECTION);
	DATA(0x01);                               // External VDD (0 = External, 1 = Internal)
	CMD(SSD1351_CMD_SETPHASELENGTH);
	DATA(0x32);
	CMD(SSD1351_CMD_ENHANCEDDRIVINGSCHEME);
	DATA(0xA4);
	DATA(0x00);
	DATA(0x00);
	CMD(SSD1351_CMD_SETSEGMENTLOWVOLTAGE);
	DATA(0xA0);                               // Enable External VSL
	DATA(0xB5);
	DATA(0x55);
	CMD(SSD1351_CMD_SETPRECHARGEVOLTAGE);
	DATA(0x17);
	CMD(SSD1351_CMD_SETVCOMHVOLTAGE);
	DATA(0x05);
	CMD(SSD1351_CMD_SETCONTRAST);
	DATA(0xa0);
	DATA(0xc8);
	DATA(0xb0);
	CMD(SSD1351_CMD_MASTERCONTRAST);
	DATA(0x0F);                               // Maximum contrast
	CMD(SSD1351_CMD_SETSECONDPRECHARGEPERIOD);
	DATA(0x01);//was 0x01
	CMD(SSD1351_CMD_SETDISPLAYMODE_RESET);


	// Use default grayscale for now to save flash space (1k), but here are
	// the values if someone wants to change them ...
	/*	CMD(SSD1351_CMD_GRAYSCALELOOKUP);

		DATA(0x02);	 // Gray Scale Level 1
		DATA(0x03);	 // Gray Scale Level 2
		DATA(0x04);	 // Gray Scale Level 3
		DATA(0x05);	 // Gray Scale Level 4

		DATA(0x06);	 // Gray Scale Level 5
		DATA(0x07);	 // Gray Scale Level 6
		DATA(0x08);	 // Gray Scale Level 7
		DATA(0x09);	 // Gray Scale Level 8

		DATA(0x0A);	 // Gray Scale Level 9
		DATA(0x0B);	 // Gray Scale Level 10
		DATA(0x0C);	 // Gray Scale Level 11
		DATA(0x0D);	 // Gray Scale Level 12

		DATA(0x0E);	 // Gray Scale Level 13
		DATA(0x0F);	 // Gray Scale Level 14

		DATA(0x10);	 // Gray Scale Level 15
		DATA(0x11);	 // Gray Scale Level 16
		DATA(0x12);	 // Gray Scale Level 17
		DATA(0x13);	 // Gray Scale Level 18
		DATA(0x15);	 // Gray Scale Level 19
		DATA(0x17);	 // Gray Scale Level 20
		DATA(0x19);	 // Gray Scale Level 21
		DATA(0x1B);	 // Gray Scale Level 22
		DATA(0x1D);	 // Gray Scale Level 23
		DATA(0x1F);	 // Gray Scale Level 24
		DATA(0x21);	 // Gray Scale Level 25
		DATA(0x23);	 // Gray Scale Level 26
		DATA(0x25);	 // Gray Scale Level 27
		DATA(0x27);	 // Gray Scale Level 28
		DATA(0x2A);	 // Gray Scale Level 29
		DATA(0x2D);	 // Gray Scale Level 30
		DATA(0x30);	 // Gray Scale Level 31
		DATA(0x33);	 // Gray Scale Level 32
		DATA(0x36);	 // Gray Scale Level 33
		DATA(0x39);	 // Gray Scale Level 34
		DATA(0x3C);	 // Gray Scale Level 35
		DATA(0x3F);	 // Gray Scale Level 36
		DATA(0x42);	 // Gray Scale Level 37
		DATA(0x45);	 // Gray Scale Level 38
		DATA(0x48);	 // Gray Scale Level 39
		DATA(0x4C);	 // Gray Scale Level 40
		DATA(0x50);	 // Gray Scale Level 41
		DATA(0x54);	 // Gray Scale Level 42
		DATA(0x58);	 // Gray Scale Level 43
		DATA(0x5C);	 // Gray Scale Level 44
		DATA(0x60);	 // Gray Scale Level 45
		DATA(0x64);	 // Gray Scale Level 46
		DATA(0x68);	 // Gray Scale Level 47
		DATA(0x6C);	 // Gray Scale Level 48
		DATA(0x70);	 // Gray Scale Level 49
		DATA(0x74);	 // Gray Scale Level 50
		DATA(0x78);	 // Gray Scale Level 51
		DATA(0x7D);	 // Gray Scale Level 52
		DATA(0x82);	 // Gray Scale Level 53
		DATA(0x87);	 // Gray Scale Level 54
		DATA(0x8C);	 // Gray Scale Level 55
		DATA(0x91);	 // Gray Scale Level 56
		DATA(0x96);	 // Gray Scale Level 57
		DATA(0x9B);	 // Gray Scale Level 58
		DATA(0xA0);	 // Gray Scale Level 59
		DATA(0xA5);	 // Gray Scale Level 60
		DATA(0xAA);	 // Gray Scale Level 61
		DATA(0xAF);	 // Gray Scale Level 62
		DATA(0xB4);	 // Gray Scale Level 63
		*/
	/*

	   DATA(0x00);
	   DATA(0x01);
	   DATA(0x02);
	   DATA(0x03);
	   DATA(0x04);
	   DATA(0x05);
	   DATA(0x06);
	   DATA(0x07);
	   DATA(0x08);
	   DATA(0x09);
	   DATA(0x0a);
	   DATA(0x0b);
	   DATA(0x0c);
	   DATA(0x0d);
	   DATA(0x0e);
	   DATA(0x0f);
	   DATA(0x10);
	   DATA(0x16);
	   DATA(0x18);
	   DATA(0x1a);
	   DATA(0x1b);
	   DATA(0x1C);
	   DATA(0x1D);
	   DATA(0x1F);
	   DATA(0x21);
	   DATA(0x23);
	   DATA(0x25);
	   DATA(0x27);
	   DATA(0x2A);
	   DATA(0x2D);
	   DATA(0x30);
	   DATA(0x33);
	   DATA(0x36);
	   DATA(0x39);
	   DATA(0x3C);
	   DATA(0x3F);
	   DATA(0x42);
	   DATA(0x45);
	   DATA(0x48);
	   DATA(0x4C);
	   DATA(0x50);
	   DATA(0x54);
	   DATA(0x58);
	   DATA(0x5C);
	   DATA(0x60);
	   DATA(0x64);
	   DATA(0x68);
	   DATA(0x6C);
	   DATA(0x70);
	   DATA(0x74);
	   DATA(0x78);
	   DATA(0x7D);
	   DATA(0x82);
	   DATA(0x87);
	   DATA(0x8C);
	   DATA(0x91);
	   DATA(0x96);
	   DATA(0x9B);
	   DATA(0xA0);
	   DATA(0xA5);
	   DATA(0xAA);
	   DATA(0xAF);
	   DATA(0xB4);
	   */
	// Clear screen
	lcdFillRGB(0,0,0);

	// Turn the display on
	CMD(SSD1351_CMD_SLEEPMODE_DISPLAYON);  
	return;
}



/**************************************************************************/
/*! 
  @brief  Draws a single pixel at the specified X/Y location
  */
/**************************************************************************/

void setLedXY(uint8_t x, uint8_t y, uint8_t r,uint8_t g, uint8_t b)
{
	if ((x >= ssd1351Properties.width) || (y >= ssd1351Properties.height))
		return;

	ssd1351SetCursor((uint8_t)x, (uint8_t)y);

#ifdef C262K
	DATA((r&0xFC)>>2);
	DATA((g&0xFC)>>2);
	DATA((b&0xFC)>>2);
#endif
#ifndef C262K
	DATA( (r&0xF8) | (g>>5) );
	DATA( (b>>3) | ((g>>2)<<5) );
#endif
}


void lcdFillRGB(uint8_t r,uint8_t g,uint8_t b)
{
	uint8_t x,y;
	ssd1351SetCursor(0, 0);

#ifdef C262K
	for (x=1; x<=ssd1351Properties.width ;x++)
	{
		for (y=1; y<= ssd1351Properties.height;y++)
		{
			DATA(r);
			DATA(g);
			DATA(b);
		}
	}
#endif
#ifndef C262K
	uint8_t data1 = (r&0xF8) | (g>>5);
	uint8_t data2 = (b>>3) | ((g>>2)<<5);

	for (x=1; x<=ssd1351Properties.width ;x++)
	{
		for (y=1; y<= ssd1351Properties.height;y++)
		{
			DATA(data1);
			DATA(data2);
		}
	}
#endif
}

