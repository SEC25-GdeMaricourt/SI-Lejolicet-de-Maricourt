#include <sam.h>
#include <stdio.h>
#include <stdarg.h>
#include "setup.h"
#include "usart_sercom2.h"
#include "__libc.h"

static char inputBuffer[USART_SERCOM2_SIZE];

static uint32_t first       = 0;
static uint32_t last        = 0;
static uint32_t nb_message  = 0;
static uint32_t overflow    = 0;




void sercom2_init(uint32_t baud)
{
    // Setup Port
    port_set_pinmux(USART_SERCOM2_TX_PIN); //TX
    port_set_pinmux(USART_SERCOM2_RX_PIN); //RX

    // Enabling Clock on BUS
    MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_SERCOM2_Msk;

   
    // Generic Clock Multiplexer SERCOM2 get GCLK 2
    GCLK_REGS->GCLK_PCHCTRL[SERCOM2_GCLK_ID_CORE] = GCLK_PCHCTRL_CHEN(1) | // Enable the Generic Clock
                                                    GCLK_PCHCTRL_GEN(2)  ; // Generic Clock Generator 2 is the source


   //reset USART peripheral
   SERCOM2_REGS->USART_INT.SERCOM_CTRLA &= ~SERCOM_USART_INT_CTRLA_ENABLE_Msk;
   while (SERCOM2_REGS->USART_INT.SERCOM_SYNCBUSY);
  
   SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;
   while (SERCOM2_REGS->USART_INT.SERCOM_SYNCBUSY);

   SERCOM2_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE(SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val); // internal clock
    
    // Enable TX and RX
   SERCOM2_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_TXEN_Msk | SERCOM_USART_INT_CTRLB_RXEN_Msk;
   while ((SERCOM2_REGS->USART_INT.SERCOM_SYNCBUSY&SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk)==SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk); // Wait Synchro
    
   SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_DORD(1);
   SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_TXPO(0); // TX on PAD[0]
   SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_RXPO(1); // RX on PAD[1]
 
    //baurate
    SERCOM2_REGS->USART_INT.SERCOM_BAUD =compute_baud(baud,USART_SERCOM2_GLCK_HZ, 0); 
    
    SERCOM2_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk;

    NVIC_EnableIRQ(SERCOM2_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM2_2_IRQn);
    NVIC_SetPriority(SERCOM2_2_IRQn,0);

    //enable USART peripheral
    SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);
    while (SERCOM2_REGS->USART_INT.SERCOM_SYNCBUSY);
   
}


void sercom2_write(char *buffer, uint32_t n) 
{
    while(n--) {
    while(!(SERCOM2_REGS->USART_INT.SERCOM_INTFLAG&SERCOM_USART_INT_INTFLAG_DRE_Msk));
    SERCOM2_REGS->USART_INT.SERCOM_DATA=*buffer++;
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
void SERCOM2_2_Handler(void) {
  //SERCOM2_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk
  if (overflow > 0) {
    SERCOM2_REGS->USART_INT.SERCOM_DATA;
  } 
  else {
    inputBuffer[last] = SERCOM2_REGS->USART_INT.SERCOM_DATA;
    // new Message on \r
    if (inputBuffer[last] == '\r') {
      nb_message++;
    }
    // Only Printabel Car from 32 to 126
    else if ((inputBuffer[last]<32)||(inputBuffer[last]>126)) {
      inputBuffer[last]='X';
    }
    if (((last + 1) % USART_SERCOM2_SIZE) == first) {
      overflow = 1;
      inputBuffer[last] = '\r';
    }
    SERCOM2_REGS->USART_INT.SERCOM_DATA = inputBuffer[last];
    last = (last + 1) % USART_SERCOM2_SIZE;
  }
}

int sercom2_ismessage(void)
{
  return nb_message;
}

int sercom2_isoverflow(void)
{
  return overflow;
}

int sercom2_read(char *message)
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
      first=(first+1)%USART_SERCOM2_SIZE;
    }
    while(!exit);
    if(nb_message!=0) {
      nb_message--; 
    }
    overflow=0; // Overflow remove once Buffer is emtpy
  }
  return count-1;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// https://wiki.segger.com/Embedded_Studio_Library_IO
// https://www.segger.com/products/development-tools/runtime-library/
int __SEGGER_RTL_X_file_write (__SEGGER_RTL_FILE *__stream, const char *__s, unsigned __len) 
{
  while(__len--) {
    while (!(SERCOM2_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk)) ;
    SERCOM2_REGS->USART_INT.SERCOM_DATA = *__s++;
  }
  return 0;
}