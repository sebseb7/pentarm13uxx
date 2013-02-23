#include "main.h"

/*

	data sheet  : http://www.nxp.com/documents/data_sheet/LPC1315_16_17_45_46_47.pdf
	user manual : http://www.nxp.com/documents/user_manual/UM10524.pdf


	led1 : pin 33 (0_12)
	led2 : pin 35 (0_14)

*/
#include <stdio.h>

#include "drivers/usb_cdc.h"
#include "drivers/buttons.h"
#include "drivers/armmath.h"
#include "core/inc/lpc13uxx_timer32.h"

volatile uint32_t msTicks = 0;
static uint16_t buttonsInitialized = 0;
static uint8_t mode  = 0;

void SysTick_Handler(void) {
	msTicks++;

	if(buttonsInitialized)
	{
		buttons_sample();
	}
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
	LPC_IOCON->SWCLK_PIO0_10  &= ~0x07;
	LPC_IOCON->SWCLK_PIO0_10  |= 0x01;

	//data direction: output
	LPC_GPIO->DIR[0] |= (1<<10);
	LPC_GPIO->DIR[0] |= (1<<12);
	LPC_GPIO->DIR[0] |= (1<<14);



	//ADC pin 015 ADC4
	LPC_IOCON->SWDIO_PIO0_15 = 0x02;
	LPC_SYSCON->PDRUNCFG &= ~(1 << 4);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
	LPC_ADC->CR = (1<< 4) | (4<< 8) | (1<< 22);

	buttons_init();
	buttonsInitialized=1;




	LPC_GPIO->B0[12] = 1;
	LPC_GPIO->B0[10] = 1;


//	usb_init();

		volatile uint32_t period = 0x15F900; //48Khz PWM frequency

		init_timer32PWM(0, period, MATCH1);
		setMatch_timer32PWM (0, 1, period-72000); // PIO 1.10
		//setMatch_timer32PWM (1, 3, 100); // PIO 1.9
		enable_timer32(0);

	buttons_get_press( KEY_C|KEY_B|KEY_A );


	uint8_t led_counter = 0;

	uint16_t sin1_counter = 0;
	uint16_t sin2_counter = 0;

	while(1)
	{
		led_counter++;
		sin1_counter+=50;
		sin2_counter+=3;

		if(led_counter == 64)
		{
			led_counter = 0;
		}

		if(buttons_get_press( KEY_A|KEY_B|KEY_C ))
		{
			mode ++;
			if(mode == 4)
			{
				mode = 0;
			}
		}
		
		uint32_t pwm=0;
		uint16_t ADC_Data=0;
	
		if(mode == 0)
		{
			if(led_counter < 3)
			{
				LPC_GPIO->B0[12] = 0;
				LPC_GPIO->B0[10] = 1;
			}
			else
			{
				LPC_GPIO->B0[12] = 1;
				LPC_GPIO->B0[10] = 0;
			}
		}

		if(mode == 1)
		{
			LPC_GPIO->B0[12] = 0;
			LPC_GPIO->B0[10] = 1;

		
			LPC_ADC->CR |= (1 << 24);              /* start A/D convert */
			while( (LPC_ADC->GDR & (1 << 31)) == 0 ); /* wait until end of A/D convert */

			LPC_ADC->CR &= 0xF8FFFFFF;      /* stop ADC now (START = 000)*/   

			ADC_Data = ( LPC_ADC->GDR >> 4 ) & 0xFFF;
		
			if(ADC_Data < 100)
			{
				pwm  = 0;
			}
			else
			{
				pwm = 13000 + ((ADC_Data-100) * 2.5f);
			}
		}

		if(mode == 2)
		{
			if(led_counter & 8)
			{
				LPC_GPIO->B0[12] = 0;
				LPC_GPIO->B0[10] = 1;
			}
			else
			{
				LPC_GPIO->B0[12] = 1;
				LPC_GPIO->B0[10] = 0;
			}

			pwm = 13000 + ( (sini(sin1_counter)>>4) * 2.5f);
			LPC_GPIO->B0[14] = 1;
		}
		
		if(mode == 3)
		{
			if(led_counter & 4)
			{
				LPC_GPIO->B0[12] = 0;
				LPC_GPIO->B0[10] = 1;
			}
			else
			{
				LPC_GPIO->B0[12] = 1;
				LPC_GPIO->B0[10] = 0;
			}

			pwm = 13000 + ( (sini(sin2_counter)>>4) * 2.5f);
			
			LPC_GPIO->B0[14] = 0;
		}
	
		setMatch_timer32PWM (0, 1, period-(72000+pwm)); // PIO 1.10
		//setMatch_timer32PWM (0, 1, period-(72000+(ADC_Data*17.578125f))); // PIO 1.10

		char text[200];
		sprintf(text,"%u %u \n",(uint16_t)ADC_Data, (uint16_t)pwm );
		
	//	usb_send_str(text);
				
		LPC_GPIO->NOT[0] = 1<<14;

		delay_ms(40);
	}
}

