#ifndef __USB_H__
#define __USB_H__

void usb_init(void);
void usb_send(uint8_t byte);
void usb_send_str(char *text);


#endif
