#ifndef _USART_SERCOM2_
#define _USART_SERCOM2_

#define USART_SERCOM2_STARTUP_MESSAGE "\r\nBuild "__DATE__" at "__TIME__"\r\n"

#define USART_SERCOM2_SIZE      80
                             
#define USART_SERCOM2_TX_PIN    PINMUX_PB25D_SERCOM2_PAD0

#define USART_SERCOM2_RX_PIN    PINMUX_PB24D_SERCOM2_PAD1
#define USART_SERCOM2_GLCK_NUM  2
#define USART_SERCOM2_GLCK_HZ   12000000

void  sercom2_init(uint32_t baud);
int   sercom2_ismessage(void);
int   sercom2_isoverflow(void);
int   sercom2_read(char *message);
void  sercom2_write(char *buffer, uint32_t n);

#endif