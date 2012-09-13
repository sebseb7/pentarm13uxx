#include "main.h"


/*

	data sheet  : http://www.nxp.com/documents/data_sheet/LPC1315_16_17_45_46_47.pdf
	user manual : http://www.nxp.com/documents/user_manual/UM10524.pdf


	led1 : pin 33 (0_12)
	led2 : pin 35 (0_14)

*/



volatile uint32_t msTicks = 0;

void SysTick_Handler(void) {
	msTicks++;
}

void delay_ms(uint32_t ms) {
	uint32_t now = msTicks;
	while ((msTicks-now) < ms);
}

int main(void) {
	
	
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

//	LPC_GPIO0->DIR = (1<<12);	
	LPC_IOCON->TMS_PIO0_12  &= ~0x07;
	LPC_IOCON->TMS_PIO0_12  |= 0x01;

	while(1)
	{
//		LPC_GPIO0->DATA |= (1<<12);
		delay_ms(200);
//		LPC_GPIO0->DATA = 0;
		delay_ms(200);
	}
}

