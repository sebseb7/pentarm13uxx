#include <math.h>
#include "i2c.h"
#include "bmp085.h"
#include "libs/reverse.h"

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;


#define CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define CAL_AC3           0xAE  // R   Calibration data (16 bits)    
#define CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define CAL_B1            0xB6  // R   Calibration data (16 bits)
#define CAL_B2            0xB8  // R   Calibration data (16 bits)
#define CAL_MB            0xBA  // R   Calibration data (16 bits)
#define CAL_MC            0xBC  // R   Calibration data (16 bits)
#define CAL_MD            0xBE  // R   Calibration data (16 bits)
#define CONTROL_OUTPUT    0xF6  // R   Output registers 0xF6=MSB, 0xF7=LSB, 0xF8=XLSB
#define CONTROL           0xF4  // W   Control register 
#define READ_TEMPERATURE        0x2E 
#define READ_PRESSURE           0x34 
#define MODE_ULTRA_LOW_POWER    0 //oversampling=0, internalsamples=1, maxconvtimepressure=4.5ms, avgcurrent=3uA, RMSnoise_hPA=0.06, RMSnoise_m=0.5
#define MODE_STANDARD           1 //oversampling=1, internalsamples=2, maxconvtimepressure=7.5ms, avgcurrent=5uA, RMSnoise_hPA=0.05, RMSnoise_m=0.4
#define MODE_HIGHRES            2 //oversampling=2, internalsamples=4, maxconvtimepressure=13.5ms, avgcurrent=7uA, RMSnoise_hPA=0.04, RMSnoise_m=0.3
#define MODE_ULTRA_HIGHRES      3 //oversampling=3, internalsamples=8, maxconvtimepressure=25.5ms, avgcurrent=12uA, RMSnoise_hPA=0.03, RMSnoise_m=0.25

int16_t ac1,ac2,ac3,b1,b2,mb,mc,md;               // cal data  
uint16_t ac4,ac5,ac6;                     // cal data
int32_t b5;                                      // temperature data
uint32_t pressure;


static void i2cWrite(uint8_t reg, uint8_t value)
{
	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = reverse8(0x77);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = value;
	I2CEngine();
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
/*	draw_number_8x6(0,0, ac1, 6,0,255,255,0);
	draw_number_8x6(0,10, ac2, 6,0,255,255,0);
	draw_number_8x6(0,20, ac3, 6,0,255,255,0);
	draw_number_8x6(0,30, ac4, 6,0,255,255,0);
	draw_number_8x6(0,40, ac5, 6,0,255,255,0);
	draw_number_8x6(0,50, ac6, 6,0,255,255,0);
	draw_number_8x6(0,60, b1, 6,0,255,255,0);
	draw_number_8x6(0,70, b2, 6,0,255,255,0);
	draw_number_8x6(0,80, mb, 6,0,255,255,0);
	draw_number_8x6(0,90, mc, 6,0,255,255,0);
	draw_number_8x6(0,100, md, 6,0,255,255,0);*/
}

void BMP085_readTemperature(void){

	int32_t ut,x1a,x2a;

	//read Raw Temperature
	i2cWrite(CONTROL, READ_TEMPERATURE);
	delay_ms(7);                          // min. 4.5ms read Temp delay
	ut = i2cRead16(0xF6);

//	draw_number_8x6(60,00, ut, 6,0,255,255,0);

	// calculate temperature
	x1a = ((int32_t)ut - ac6) * ac5 >> 15;
	x2a = ((int32_t)mc << 11) / (x1a + md);
	b5 = x1a + x2a;
}

uint16_t BMP085_getTemperatrue(void)
{
	return ((b5 + 8) >> 4);
}

void BMP085_readPressure(void)
{
	int32_t up=0,x1,x2,x3,b3,b6,p;
	uint32_t b4,b7;

	//read Raw Pressure
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
/*	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	i2cWrite(CONTROL, READ_PRESSURE+(MODE_ULTRA_HIGHRES << 6));
	delay_ms(26);
	up += i2cReads24(0xF6) >> (8-MODE_ULTRA_HIGHRES);
	
	up = up >> 3;
*/
	// calculate true pressure
	b6 = b5 - 4000;             // b5 is updated by BMP085_readTemperature().
	x1 = (b2* (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((ac1*4+x3) << MODE_ULTRA_HIGHRES)+2) >> 2;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (uint32_t) (x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000 >> MODE_ULTRA_HIGHRES);
	p = b7 < 0x80000000 ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	pressure = p + ((x1 + x2 + 3791) >> 4);
}
uint32_t BMP085_getPressure(void)
{
	return pressure;
}
uint32_t BMP085_getAltitude(uint32_t pressureAtSeaLevel)
{
	return 4433000 * (1 - pow((pressure / (float)pressureAtSeaLevel), 0.1903f));  
}

uint32_t BMP085_getPressureAtSeaLevel(uint32_t altitude)
{
	return pressure / (float)pow( 1-(altitude/ 4433000.0f), 5.255f);  
}
