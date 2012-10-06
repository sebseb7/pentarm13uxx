#include "drivers/i2c.h"
#include "libs/reverse.h"
#include "drivers/n35p112.h"

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;

void i2cWrite(uint8_t reg, uint8_t value)
{
	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = reverse8(0x41);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = value;
	I2CEngine();
}
uint8_t i2cRead(uint8_t reg)
{
	I2CWriteLength = 2;
	I2CReadLength = 1;
	I2CMasterBuffer[0] = reverse8(0x41);
	I2CMasterBuffer[1] = reg;
	I2CMasterBuffer[2] = reverse8(0x41) | RD_BIT;
	I2CEngine();
	return I2CSlaveBuffer[0];
}

uint8_t offset_x = 0;
uint8_t offset_y = 0;


int n35p112_init(void) {

	i2cWrite(0x2b,0x00);
	i2cWrite(0x2c,0x06);
	i2cWrite(0x2d,0x06);

	delay_ms(10);

	uint16_t syncx =0;
	uint16_t syncy =0;
	for(uint8_t i = 0; i < 16; i++)
	{
		syncx += i2cRead(0x10);
		syncy += i2cRead(0x11);

		delay_ms(20);
	}

	syncx/=16;
	syncy/=16;

	offset_x = syncx;
	offset_y = syncy;

	return 0;
}


int get_n35p112(uint8_t *x,uint8_t *y)
{
	*x = 255-(i2cRead(0x10)-offset_x+128);
	*y = i2cRead(0x11)-offset_y+128;
	
	return 0;
}

