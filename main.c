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
	
	// clock to GPIO
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

	// configure the two LEDs PINs as GPIO (they default to jtag)
	LPC_IOCON->TMS_PIO0_12  &= ~0x07;
	LPC_IOCON->TMS_PIO0_12  |= 0x01;
	LPC_IOCON->TRST_PIO0_14  &= ~0x07;
	LPC_IOCON->TRST_PIO0_14  |= 0x01;

	//data direction: output
	LPC_GPIO->DIR[0] |= (1<<12);
	LPC_GPIO->DIR[0] |= (1<<14);

	while(1)
	{
		LPC_GPIO->B0[12] = 1;
		LPC_GPIO->B0[14] = 0;
		delay_ms(200);
		LPC_GPIO->B0[12] = 0;
		LPC_GPIO->B0[14] = 1;
		delay_ms(200);
	}
}

