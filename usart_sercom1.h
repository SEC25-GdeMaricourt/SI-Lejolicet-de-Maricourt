#ifndef _USART_SERCOM1_
#define _USART_SERCOM1_

#define USART_SERCOM1_STARTUP_MESSAGE "\r\nBuild "__DATE__" at "__TIME__"\r\n"

#define USART_SERCOM1_SIZE      160
// EXT3  pin  14 [USART_TX] PC22 SERCOM1 PAD[0] UART TX
#define USART_SERCOM1_TX_PIN    PINMUX_PC22C_SERCOM1_PAD0
// EXT3  pin  13 [USART_RX] PC23 SERCOM1 PAD[1] UART RX
#define USART_SERCOM1_RX_PIN    PINMUX_PC23C_SERCOM1_PAD1
#define USART_SERCOM1_GLCK_NUM  2
#define USART_SERCOM1_GLCK_HZ   12000000

void  sercom1_init(uint32_t baud);
int   sercom1_ismessage(void);
int   sercom1_isoverflow(void);
int   sercom1_read(char *message);
int   sercom1_decode(char *message);
void  sercom1_write(char *buffer, uint32_t n);

#endif