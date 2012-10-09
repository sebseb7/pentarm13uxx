#ifndef __BMP085_H__
#define __BMP085_H__

#include "main.h"

void BMP085_getCalData(void); 								//has to be called once for initialization

void BMP085_readTemperature(void);
void BMP085_readPressure(void);
uint16_t BMP085_getTemperatrue(void);						//BMP085_readTemperature has to be called before this
uint32_t BMP085_getPressure(void);							//BMP085_readTemperature/readPressure has to be called before this
uint32_t BMP085_getAltitude(uint32_t pressureAtSeaLevel); 	// mean: 101325  & BMP085_readTemperature/readPressure has to be called before this
uint32_t BMP085_getPressureAtSeaLevel(uint32_t altitude); 	//BMP085_readTemperature/readPressure has to be called before this



#endif
