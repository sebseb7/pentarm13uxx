#include "main.h"

/*

	data sheet  : http://www.nxp.com/documents/data_sheet/LPC1315_16_17_45_46_47.pdf
	user manual : http://www.nxp.com/documents/user_manual/UM10524.pdf


	led1 : pin 33 (0_12)
	led2 : pin 35 (0_14)

*/

#include "drivers/usb_cdc.h"
#include "core/inc/lpc13uxx_timer32.h"

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



	//ADC pin 015 ADC4
	LPC_IOCON->SWDIO_PIO0_15 = 0x02;
	LPC_SYSCON->PDRUNCFG &= ~(1 << 4);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
	LPC_ADC->CR = (1<< 4) | (4<< 8) | (1<< 22);





	LPC_GPIO->B0[12] = 1;


//	usb_init();
		volatile uint32_t period = 0x15F900; //48Khz PWM frequency

		init_timer32PWM(0, period, MATCH1);
		setMatch_timer32PWM (0, 1, period-72000); // PIO 1.10
		//setMatch_timer32PWM (1, 3, 100); // PIO 1.9
		enable_timer32(0);

	while(1)
	{
		LPC_ADC->CR |= (1 << 24);              /* start A/D convert */
		while( (LPC_ADC->GDR & (1 << 31)) == 0 ); /* wait until end of A/D convert */

		LPC_ADC->CR &= 0xF8FFFFFF;      /* stop ADC now (START = 000)*/   
//		if( LPC_ADC->GDR & (1 << 30) )  /* if overrun, return zero */
//			return 0 ;

		uint16_t ADC_Data = ( LPC_ADC->GDR >> 4 ) & 0xFFF;

		
		setMatch_timer32PWM (0, 1, period-(72000+(ADC_Data*17.578125f))); // PIO 1.10


//		usb_send(ADC_Data>>2);

		LPC_GPIO->NOT[0] = 1<<12;
		delay_ms(20);
		LPC_GPIO->NOT[0] = 1<<14;
		delay_ms(20);
	}
}

