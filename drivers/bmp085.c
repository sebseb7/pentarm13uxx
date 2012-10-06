#include <math.h>
#include "i2c.h"
#include "bmp085.h"
#include "libs/reverse.h"

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;




static void i2cWrite(uint8_t reg, uint8_t value)
{
	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = reverse8(0x77);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = value;
	I2CEngine();
}
static uint8_t i2cRead(uint8_t reg)
{
	I2CSlaveBuffer[0]=0;
	I2CWriteLength = 2;
	I2CReadLength = 1;
	I2CMasterBuffer[0] = reverse8(0x77);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = reverse8(0x77) | RD_BIT;
	I2CEngine();
	return I2CSlaveBuffer[0];
}
static uint16_t i2cRead16(uint8_t reg)
{
	I2CSlaveBuffer[0]=0;
	I2CSlaveBuffer[1]=0;
	I2CWriteLength = 2;
	I2CReadLength = 2;
	I2CMasterBuffer[0] = reverse8(0x77);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = reverse8(0x77) | RD_BIT;
	I2CEngine();
	return I2CSlaveBuffer[0] << 8 | I2CSlaveBuffer[1];
}
static int16_t i2cReads16(uint8_t reg)
{
	I2CSlaveBuffer[0]=0;
	I2CSlaveBuffer[1]=0;
	I2CWriteLength = 2;
	I2CReadLength = 2;
	I2CMasterBuffer[0] = reverse8(0x77);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = reverse8(0x77) | RD_BIT;
	I2CEngine();
	return I2CSlaveBuffer[0] << 8 | I2CSlaveBuffer[1];
}
static int32_t i2cReads24(uint8_t reg)
{
	I2CSlaveBuffer[0]=0;
	I2CSlaveBuffer[1]=0;
	I2CSlaveBuffer[2]=0;
	
	I2CWriteLength = 2;
	I2CReadLength = 3;
	I2CMasterBuffer[0] = reverse8(0x77);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = reverse8(0x77) | RD_BIT;
	I2CEngine();
	return I2CSlaveBuffer[0] << 16 | I2CSlaveBuffer[1] << 8 | I2CSlaveBuffer[2];
}

void BMP085_getCalData(void) {
	ac1 = i2cReads16(CAL_AC1);
	ac2 = i2cReads16(CAL_AC2);
	ac3 = i2cReads16(CAL_AC3);
	ac4 = i2cRead16(CAL_AC4);
	ac5 = i2cRead16(CAL_AC5);
	ac6 = i2cRead16(CAL_AC6);
	b1 = i2cReads16(CAL_B1);
	b2 = i2cReads16(CAL_B2);
	mb = i2cReads16(CAL_MB);
	mc = i2cReads16(CAL_MC);
	md = i2cReads16(CAL_MD);
}

void BMP085_readTemperature(void){

	long ut,x1,x2;

	//read Raw Temperature
	i2cWrite(CONTROL, READ_TEMPERATURE);
	delay_ms(5);                          // min. 4.5ms read Temp delay
	ut = i2cReads16(0xF6);
	//draw_number_8x6(60,20, ut, 6,0,255,255,255);

	// calculate temperature
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long)mc << 11) / (x1 + md);
	b5 = x1 + x2;
}


void BMP085_readPressure(void) {
	long up,x1,x2,x3,b3,b6,p;
	unsigned long b4,b7;
	int32_t tmp; 

	//read Raw Pressure
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	uint8_t r1 = i2cRead(0xF6);
	uint8_t r2 = i2cRead(0xF7);
	uint8_t r3 = i2cRead(0xF8);


/*	up = ((r1<<16) + (r2<<8)+ r3) >> (8-MODE_ULTRA_HIGHRES);
	//draw_number_8x6(60,70, up, 6,0,255,255,0);


	// calculate true pressure
	b6 = b5 - 4000;             // b5 is updated by calcTrueTemperature().
	x1 = (b2* (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	tmp = ac1;
	tmp = (tmp * 4 + x3) << MODE_ULTRA_HIGHRES;
	b3 = (tmp + 2) >> 2;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (uint32_t) (x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000 >> MODE_ULTRA_HIGHRES);
	p = b7 < 0x80000000 ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	uint32_t pressure = p + ((x1 + x2 + 3791) >> 4);
	int32_t centimeters = 0;
	centimeters = 4433000 * (1 - pow((pressure / (float)99612), 0.1903f));  
	lcdFillRGB(0,0,0);
	draw_number_8x6(60,80, pressure, 6,0,255,255,0);
	draw_number_8x6(60,90, centimeters, 6,0,255,0,255);

/
	int centimeters
	samples[index++] = centimeters;
	index %= 10;
	int average = 0;
	for (int i = 0; i < 10; i++) {
		average += samples[i];
	}
	average /= 10;
	draw_number_8x6(60,100, average, 6,0,255,0,255);
*/
}

/*
		BMP085_readTemperature();
		draw_number_8x6(60,10, temperature, 6,0,0,0,0);
		temperature = ((b5 + 8) >> 4);
		draw_number_8x6(60,10, temperature, 6,0,255,255,255);
*/
