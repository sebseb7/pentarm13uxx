#ifndef __SSD1351_H__
#define __SSD1351_H__

#include "main.h"

#define LED_WIDTH 128
#define LED_HEIGHT 128

//#define I6800
//#define C262K



#ifdef C262K
#ifndef I6800
#error 262k color needs i6800
#endif
#endif

typedef struct
{
  uint16_t width;         // LCD width in pixels (default orientation)
  uint16_t height;        // LCD height in pixels (default orientation)
} lcdProperties_t;


/*
 *
 * 28  -> pin12 -> 127
 * 21  -> pin13 -> 120
 * scl -> pin15 -> 04
 * sda -> pin16 -> 05
 * 06  -> pin22 -> 06
 * 07  -> pin23 -> 07
 * 08  -> pin27 -> 08
 * 09  -> pin28 -> 09
 * 010 -> pin29 -> 010
 * 011 -> pin32 -> 022
 *
 *
 * 23  -> pin38 -> 122
 * 13  -> pin39 -> 015
 * 14  -> pin40 -> 016
 * 15  -> pin45 -> 017
 * rx  -> pin46 -> 018
 * tx  -> pin47 -> 019
 * 33  -> pin48 -> 116
 * 26  -> pin1  -> 125
 *
 */


#define SET_SCK	(LPC_GPIO->B0[6] = 1)	
#define CLR_SCK	(LPC_GPIO->B0[6] = 0)
#define SET_SID	(LPC_GPIO->B0[7] = 1)	
#define CLR_SID	(LPC_GPIO->B0[7] = 0)
#define SET_CS	(LPC_GPIO->B0[8] = 1)	
#define CLR_CS	(LPC_GPIO->B0[8] = 0)
#define SET_DC	(LPC_GPIO->B0[9] = 1)	
#define CLR_DC	(LPC_GPIO->B0[9] = 0)
#define SET_RST	(LPC_GPIO->B0[10] = 1)	
#define CLR_RST	(LPC_GPIO->B0[10] = 0)


#ifdef I6800
#define SET_E (LPC_GPIO->B1[27] = 1)
#define CLR_E (LPC_GPIO->B1[27] = 0)
#define SET_D2 (LPC_GPIO->B1[22] = 1)
#define CLR_D2 (LPC_GPIO->B1[22] = 0)
#define SET_D3 (LPC_GPIO->B0[15] = 1)
#define CLR_D3 (LPC_GPIO->B0[15] = 0)
#define SET_D4 (LPC_GPIO->B0[16] = 1)
#define CLR_D4 (LPC_GPIO->B0[16] = 0)
#define SET_D5 (LPC_GPIO->B0[17] = 1)
#define CLR_D5 (LPC_GPIO->B0[17] = 0)
#define SET_D6 (LPC_GPIO->B1[16] = 1)
#define CLR_D6 (LPC_GPIO->B1[16] = 0)
#define SET_D7 (LPC_GPIO->B1[25] = 1)
#define CLR_D7 (LPC_GPIO->B1[25] = 0)
#endif


/* clock 06
 * data  09
 *
 */


// SSD1351 Commands
enum
{
  SSD1351_CMD_SETCOLUMNADDRESS          = 0x15,
  SSD1351_CMD_SETROWADDRESS             = 0x75,
  SSD1351_CMD_WRITERAM                  = 0x5C, // Write data to GRAM and increment until another command is sent
  SSD1351_CMD_READRAM                   = 0x5D, // Read data from GRAM and increment until another command is sent
  SSD1351_CMD_COLORDEPTH                = 0xA0, // Numerous functions include increment direction ... see DS 
                                                // A0[0] = Address Increment Mode (0 = horizontal, 1 = vertical)
                                                // A0[1] = Column Address Remap (0 = left to right, 1 = right to left)
                                                // A0[2] = Color Remap (0 = ABC, 1 = CBA) - HW RGB/BGR switch
                                                // A0[4] = COM Scan Direction (0 = top to bottom, 1 = bottom to top)
                                                // A0[5] = Odd/Even Paid Split
                                                // A0[7:6] = Display Color Mode (00 = 8-bit, 01 = 65K, 10/11 = 262K, 8/16-bit interface only)
  SSD1351_CMD_SETDISPLAYSTARTLINE       = 0xA1,
  SSD1351_CMD_SETDISPLAYOFFSET          = 0xA2, 
  SSD1351_CMD_SETDISPLAYMODE_ALLOFF     = 0xA4, // Force entire display area to grayscale GS0
  SSD1351_CMD_SETDISPLAYMODE_ALLON      = 0xA5, // Force entire display area to grayscale GS63
  SSD1351_CMD_SETDISPLAYMODE_RESET      = 0xA6, // Resets the display area relative to the above two commands
  SSD1351_CMD_SETDISPLAYMODE_INVERT     = 0xA7, // Inverts the display contents (GS0 -> GS63, GS63 -> GS0, etc.)
  SSD1351_CMD_FUNCTIONSELECTION         = 0xAB, // Enable/Disable the internal VDD regulator
  SSD1351_CMD_SLEEPMODE_DISPLAYOFF      = 0xAE, // Sleep mode on (display off)
  SSD1351_CMD_SLEEPMODE_DISPLAYON       = 0xAF, // Sleep mode off (display on)
  SSD1351_CMD_SETPHASELENGTH            = 0xB1, // Larger capacitance may require larger delay to discharge previous pixel state
  SSD1351_CMD_ENHANCEDDRIVINGSCHEME     = 0xB2, 
  SSD1351_CMD_SETFRONTCLOCKDIV          = 0xB3, // DCLK divide ration fro CLK (from 1 to 16)
  SSD1351_CMD_SETSEGMENTLOWVOLTAGE      = 0xB4,
  SSD1351_CMD_SETGPIO                   = 0xB5,
  SSD1351_CMD_SETSECONDPRECHARGEPERIOD  = 0xB6,
  SSD1351_CMD_GRAYSCALELOOKUP           = 0xB8,
  SSD1351_CMD_LINEARLUT                 = 0xB9,
  SSD1351_CMD_SETPRECHARGEVOLTAGE       = 0xBB,
  SSD1351_CMD_SETVCOMHVOLTAGE           = 0xBE,
  SSD1351_CMD_SETCONTRAST               = 0xC1,
  SSD1351_CMD_MASTERCONTRAST            = 0xC7,
  SSD1351_CMD_SETMUXRRATIO              = 0xCA,
  SSD1351_CMD_NOP3                      = 0xD1,
  SSD1351_CMD_NOP4                      = 0xE3,
  SSD1351_CMD_SETCOMMANDLOCK            = 0xFD,
  SSD1351_CMD_HORIZONTALSCROLL          = 0x96,
  SSD1351_CMD_STOPMOVING                = 0x9E,
  SSD1351_CMD_STARTMOVING               = 0x9F  
};

void lcdInit(void);
void setLedXY(uint8_t x, uint8_t y, uint8_t r,uint8_t g, uint8_t b);
void lcdFillRGB(uint8_t r,uint8_t g,uint8_t b);
void ssd1351SetCursor(uint8_t x, uint8_t y);
void ssd1351SendByte(uint8_t byte);

#endif
