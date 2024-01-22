#include <sam.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "setup.h"
#include "usart_sercom1.h"

static char inputBuffer[USART_SERCOM1_SIZE];
static char WriteHex[USART_SERCOM1_SIZE<<1];
static uint32_t first       = 0;
static uint32_t last        = 0;
static uint32_t nb_message  = 0;
static uint32_t overflow    = 0;


static int Ascii2Hex(char *ascii, char *hex,uint32_t len );

void  sercom1_init(uint32_t baud)
{
    // Setup Port
    port_set_pinmux(USART_SERCOM1_TX_PIN); //TX
    port_set_pinmux(USART_SERCOM1_RX_PIN); //RX

   // Enabling Clock on BUS
   MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_SERCOM1_Msk;
  
   // Generic Clock Multiplexer SERCOM0 get GCLK 2
   GCLK_REGS->GCLK_PCHCTRL[SERCOM1_GCLK_ID_CORE] = GCLK_PCHCTRL_CHEN(1) | // Enable the Generic Clock
                                                   GCLK_PCHCTRL_GEN(2)  ; // Generic Clock Generator 2 is the source

   //reset USART peripheral
   SERCOM1_REGS->USART_INT.SERCOM_CTRLA &= ~SERCOM_USART_INT_CTRLA_ENABLE_Msk;
   while (SERCOM1_REGS->USART_INT.SERCOM_SYNCBUSY);
  
   SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;
   while (SERCOM1_REGS->USART_INT.SERCOM_SYNCBUSY);

   SERCOM1_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE(SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val); // internal clock
    
   // Enable TX and RX
   SERCOM1_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_TXEN_Msk | SERCOM_USART_INT_CTRLB_RXEN_Msk;
   while ((SERCOM1_REGS->USART_INT.SERCOM_SYNCBUSY&SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk)==SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk); // Wait Synchro
    
   SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_DORD(1);
   SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_TXPO(0); // TX on PAD[0]
   SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_RXPO(1); // RX on PAD[0]
    
    //baurate
    SERCOM1_REGS->USART_INT.SERCOM_BAUD =compute_baud(baud,USART_SERCOM1_GLCK_HZ, 0); 
    
    //Enable interrupt on data RX
    SERCOM1_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk;

    NVIC_EnableIRQ(SERCOM1_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM1_2_IRQn);
    NVIC_SetPriority(SERCOM1_2_IRQn,0);

    //enable USART peripheral
    SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);
    while (SERCOM1_REGS->USART_INT.SERCOM_SYNCBUSY); 
}


void sercom1_write(char *buffer, uint32_t n) 
{
    uint32_t size=Ascii2Hex(buffer, WriteHex,n);
    for(uint32_t i=0; i<size; i++) {
      while(!(SERCOM1_REGS->USART_INT.SERCOM_INTFLAG&SERCOM_USART_INT_INTFLAG_DRE_Msk));
      SERCOM1_REGS->USART_INT.SERCOM_DATA=WriteHex[i]++;
    }
}

// SERCOM2 - Serial Communication Interface 2(1) 
// 0 DRE    SERCOM2_0_IRQn
// 1 TXC    SERCOM2_1_IRQn
// 2 RXC    SERCOM2_2_IRQn
// 3 RXS    SERCOM2_3_IRQn
// 4 CTSIC  SERCOM2_3_IRQn
// 5 RXBRK  SERCOM2_3_IRQn
// 7 ERROR  SERCOM2_3_IRQn
void SERCOM1_2_Handler(void) 
{
  //SERCOM1_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk
  if (overflow > 0) {
      SERCOM1_REGS->USART_INT.SERCOM_DATA;
  } 
  else {
    inputBuffer[last] = SERCOM1_REGS->USART_INT.SERCOM_DATA;
    if (inputBuffer[last] == '\r') {
      nb_message++;
    }
    if (((last + 1) % USART_SERCOM1_SIZE) == first) {
      overflow = 1;
      inputBuffer[last] = '\r';
    }
    last = (last + 1) % USART_SERCOM1_SIZE;
  }
}

int sercom1_ismessage(void)
{
  return nb_message;
}

int sercom1_isoverflow(void)
{
  return overflow;
}

int sercom1_read(char *message)
{
  int32_t count = 0;
  int8_t exit=0;
  // If there is no new message
  if((nb_message>0)||(overflow==1))  {
    do {
      count++;
      *message=inputBuffer[first];
      if(inputBuffer[first]=='\r') {
        *message=0;
        exit=1;
      }
      message++;
      first=(first+1)%USART_SERCOM1_SIZE;
    }
    while(!exit);
    if(nb_message!=0) {
      nb_message--; 
    }
    overflow=0; // Overflow remove once Buffer is emtpy
  }
  return count-1;
}

int sercom1_decode(char *message)
{
  char c0,c1,ascii,count;
  for (uint32_t i = 0; (message[i]!='\0')&&(i<USART_SERCOM1_SIZE<<1)  ; i+=2) {
    ascii=0;
    c0 = message[i];
    if ((c0 >= '0' && c0 <= '9') || (c0 >= 'a' && c0 <= 'f')) {
      ascii = (c0 >= 'a') ? c0 - 'a' + 10 : c0 - '0';
    }
    ascii=ascii<<4;
    c1 = message[i+1];
    if ((c1 >= '0' && c1 <= '9') || (c1 >= 'a' && c1 <= 'f')) {
       ascii |= (c1 >= 'a') ? c1 - 'a' + 10 : c1 - '0';
    }
    message[count++]=ascii;
  }
  message[count]='\0';
  return count;
}


static int Ascii2Hex(char *ascii, char *hex, uint32_t len ) 
{
  int i;
  char chHex[] = "0123456789abcdef";

  for (i = 0; i < len; i++) {
    hex[i * 2] = chHex[(ascii[i]) >> 4];
    hex[i * 2 + 1] = chHex[(ascii[i]) & 0xf];
  }
  hex[len * 2] = '\r';
  return len * 2+1;
}
