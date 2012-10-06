#include "usb/mw_usbd_rom_api.h"
#include "usb/power_api.h"
#include "LPC13Uxx.h"
#include <string.h>


extern uint8_t VCOM_DeviceDescriptor[];
extern uint8_t VCOM_StringDescriptor[];
extern uint8_t VCOM_ConfigDescriptor[];
USBD_API_T* pUsbApi;
#define VCOM_BUFFERS    4
#define VCOM_BUF_EMPTY_INDEX  (0xFF)
#define VCOM_BUF_FREE   0
#define VCOM_BUF_ALLOC  1
#define VCOM_BUF_USBTXQ  2
#define VCOM_BUF_UARTTXQ  3
#define VCOM_BUF_ALLOCU  4

struct VCOM_DATA;
typedef void (*VCOM_SEND_T) (struct VCOM_DATA* pVcom);

typedef struct VCOM_DATA {
	USBD_HANDLE_T hUsb;
	USBD_HANDLE_T hCdc;
	uint8_t* rxBuf;
	uint8_t* txBuf;
	volatile uint8_t ser_pos;
	volatile uint16_t rxlen;
	volatile uint16_t txlen;
	VCOM_SEND_T send_fn;
	volatile uint32_t sof_counter;
	volatile uint32_t last_ser_rx;
	volatile uint16_t break_time;
	volatile uint16_t usbrx_pend;
} VCOM_DATA_T; 
VCOM_DATA_T g_vCOM;
void USB_pin_clk_init(void)
{
	/* Enable AHB clock to the GPIO domain. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

	/* Enable AHB clock to the USB block and USB RAM. */
	LPC_SYSCON->SYSAHBCLKCTRL |= ((0x1<<14)|(0x1<<27));

	/* Pull-down is needed, or internally, VBUS will be floating. This is to
	   address the wrong status in VBUSDebouncing bit in CmdStatus register.  */
	LPC_IOCON->PIO0_3   &= ~0x1F; 
	//  LPC_IOCON->PIO0_3   |= ((0x1<<3)|(0x01<<0));	/* Secondary function VBUS */
	LPC_IOCON->PIO0_3   |= (0x01<<0);			/* Secondary function VBUS */
//	LPC_IOCON->PIO0_6   &= ~0x07;
//	LPC_IOCON->PIO0_6   |= (0x01<<0);			/* Secondary function SoftConn */

	return;
}
void VCOM_usb_send(VCOM_DATA_T* pVcom)
{
	/* data received send it back */
	pVcom->txlen -= pUsbApi->hw->WriteEP (pVcom->hUsb, USB_CDC_EP_BULK_IN, 
			pVcom->txBuf, pVcom->txlen);   
}

void usb_send(uint8_t byte)
{
	VCOM_DATA_T* pVcom = &g_vCOM;
	/* data received send it back */
	pVcom->txBuf[0]=byte;
	pVcom->txlen -= pUsbApi->hw->WriteEP (pVcom->hUsb, USB_CDC_EP_BULK_IN, 
			pVcom->txBuf, 1);   
}
void usb_send_str(char *text)
{
	VCOM_DATA_T* pVcom = &g_vCOM;
	/* data received send it back */
	
	int len = strlen(text);

	pVcom->txlen -= pUsbApi->hw->WriteEP (pVcom->hUsb, USB_CDC_EP_BULK_IN, 
			text, len);   
}

ErrorCode_t VCOM_SendBreak (USBD_HANDLE_T hCDC, uint16_t mstime)
{
	VCOM_DATA_T* pVcom = &g_vCOM;
	uint8_t lcr = LPC_USART->LCR;

	if ( mstime) {
		lcr |= (1 << 6);
	} else {
		lcr &= ~(1 << 6);
	}

	pVcom->break_time = mstime;
	LPC_USART->LCR = lcr;

	return LPC_OK;
}

ErrorCode_t VCOM_bulk_in_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event) 
{
	//  VCOM_DATA_T* pVcom = (VCOM_DATA_T*) data;
	//  Not needed as WriteEP() is called in VCOM_usb_send() immediately.  
	//  if (event == USB_EVT_IN) {
	//  }
	return LPC_OK;
}

ErrorCode_t VCOM_bulk_out_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event) 
{
	VCOM_DATA_T* pVcom = (VCOM_DATA_T*) data;

	switch (event) {
		case USB_EVT_OUT:

			LPC_GPIO->B0[12] ^= 1;
			pVcom->rxlen = pUsbApi->hw->ReadEP(hUsb, USB_CDC_EP_BULK_OUT, pVcom->rxBuf);
			//pVcom->rxBuf[0] = pVcom->rxlen;
			pUsbApi->hw->WriteEP (pVcom->hUsb, USB_CDC_EP_BULK_IN, pVcom->rxBuf, pVcom->rxlen);     
			//	if (pVcom->rxlen == 0) {
			//        pVcom->rxlen = pUsbApi->hw->ReadEP(hUsb, USB_CDC_EP_BULK_OUT, pVcom->rxBuf);
			//        pVcom->send_fn(pVcom);
			//      } else {
			//        /* indicate bridge write buffer pending in USB buf */
			//        pVcom->usbrx_pend = 1;
			//      }
			break;
		default: 
			break;
	}
	return LPC_OK;
}
void USB_IRQHandler(void)
{
	pUsbApi->hw->ISR(g_vCOM.hUsb);
}


void usb_init(void) {

	USBD_API_INIT_PARAM_T usb_param;
	USBD_CDC_INIT_PARAM_T cdc_param;
	USB_CORE_DESCS_T desc;
	USBD_HANDLE_T hUsb, hCdc;
	ErrorCode_t ret = LPC_OK;
	uint32_t ep_indx;
	

	/* get USB API table pointer */
	pUsbApi = (USBD_API_T*)((*(ROM **)(0x1FFF1FF8))->pUSBD);

	/* enable clocks and pinmux */
	USB_pin_clk_init();

	/* initilize call back structures */
	memset((void*)&usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.mem_base = 0x10001000;
	usb_param.mem_size = 0x1000;
	usb_param.max_num_ep = 3;

	/* init CDC params */
	memset((void*)&cdc_param, 0, sizeof(USBD_CDC_INIT_PARAM_T));
	memset((void*)&g_vCOM, 0, sizeof(VCOM_DATA_T));

	/* user defined functions */
	//#if defined(UART_BRIDGE)
	//  cdc_param.SetLineCode = VCOM_SetLineCode; 
	//  usb_param.USB_SOF_Event = VCOM_sof_event; 
	//#endif
	cdc_param.SendBreak = VCOM_SendBreak;

	/* Initialize Descriptor pointers */
	memset((void*)&desc, 0, sizeof(USB_CORE_DESCS_T));
	desc.device_desc = (uint8_t *)&VCOM_DeviceDescriptor[0];
	desc.string_desc = (uint8_t *)&VCOM_StringDescriptor[0];
	desc.full_speed_desc = (uint8_t *)&VCOM_ConfigDescriptor[0];
	desc.high_speed_desc = (uint8_t *)&VCOM_ConfigDescriptor[0];

	/* USB Initialization */
	ret = pUsbApi->hw->Init(&hUsb, &desc, &usb_param);  
	if (ret == LPC_OK) {

		// init CDC params
		cdc_param.mem_base = usb_param.mem_base;
		cdc_param.mem_size = usb_param.mem_size;
		cdc_param.cif_intf_desc = (uint8_t *)&VCOM_ConfigDescriptor[USB_CONFIGUARTION_DESC_SIZE];
		cdc_param.dif_intf_desc = (uint8_t *)&VCOM_ConfigDescriptor[USB_CONFIGUARTION_DESC_SIZE + \
								  USB_INTERFACE_DESC_SIZE + 0x0013 + USB_ENDPOINT_DESC_SIZE ];

		ret = pUsbApi->cdc->init(hUsb, &cdc_param, &hCdc);

		if (ret == LPC_OK) {
			/* store USB handle */
			g_vCOM.hUsb = hUsb;
			g_vCOM.hCdc = hCdc;
			g_vCOM.send_fn = VCOM_usb_send;

			/* allocate transfer buffers */
			g_vCOM.rxBuf = (uint8_t*)(cdc_param.mem_base + (0 * USB_HS_MAX_BULK_PACKET));
			g_vCOM.txBuf = (uint8_t*)(cdc_param.mem_base + (1 * USB_HS_MAX_BULK_PACKET));
			cdc_param.mem_size -= (4 * USB_HS_MAX_BULK_PACKET);

			/* register endpoint interrupt handler */
			ep_indx = (((USB_CDC_EP_BULK_IN & 0x0F) << 1) + 1);
			ret = pUsbApi->core->RegisterEpHandler (hUsb, ep_indx, VCOM_bulk_in_hdlr, &g_vCOM);
			if (ret == LPC_OK) {
				/* register endpoint interrupt handler */
				ep_indx = ((USB_CDC_EP_BULK_OUT & 0x0F) << 1);
				ret = pUsbApi->core->RegisterEpHandler (hUsb, ep_indx, VCOM_bulk_out_hdlr, &g_vCOM);
				if (ret == LPC_OK) {

					/* enable IRQ */
					NVIC_EnableIRQ(USB_IRQ_IRQn); //  enable USB0 interrrupts 
					/* USB Connect */
					pUsbApi->hw->Connect(hUsb, 1);
				}
			}
		}    
	}

}

