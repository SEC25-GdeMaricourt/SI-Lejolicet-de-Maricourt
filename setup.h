#include <sam.h>

#ifndef _SETUP_
#define _SETUP_

// PC18 GPIO2 PDEC and User LED
#define LED PORT_PC18
#define LED_Set() (PORT_REGS->GROUP[2].PORT_OUTSET = LED)
#define LED_Clear() (PORT_REGS->GROUP[2].PORT_OUTCLR = LED)
#define LED_Toggle() (PORT_REGS->GROUP[2].PORT_OUTTGL = LED)
#define LED_OutputEnable() (PORT_REGS->GROUP[2].PORT_DIRSET = LED)
#define LED_InputEnable() (PORT_REGS->GROUP[2].PORT_DIRCLR = LED)
#define LED_Get() (((PORT_REGS->GROUP[2].PORT_IN >> 18U)) & 0x01U)

// PB31 GPIO3 User SW0
#define BUTTON_get() (((PORT_REGS->GROUP[1].PORT_IN >> 31U)) & 0x01U)
#define BUTTON_Enable()                                                                \
  {                                                                                    \
    PORT_REGS->GROUP[1].PORT_PINCFG[31] = PORT_PINCFG_PULLEN(1) | PORT_PINCFG_INEN(1); \
    PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB31;                                       \
  }


// Define Pin For Sercom 2 Virtual COM Port Connections

// PB24 SERCOM2 PAD[1] UART RXD (SAM E54 RX line)
// PINMUX_PB24D_SERCOM2_PAD1 

// PB25 SERCOM2 PAD[0] UART TXD (SAM E54 TX line)
// PORT_PB25D_SERCOM2_PAD0 


typedef struct {
  uint64_t reserved1 : 3;
  uint64_t reserved2 : 12;
  uint64_t reserved3 : 12;
  uint64_t ADC_LINEARITY : 8;
  uint64_t ADC_BIASCAL : 3;
  uint64_t OSC32K_CAL : 7;
  uint64_t reserved4 : 13;
  uint64_t DFLL48M_COARSE_CAL : 6;
} calibrationRow_t;

#define CALROW ((calibrationRow_t *)0x00806020U)

void setup(void);
void setup_gclk0(void);
void setup_gclk1(void);
void setup_gclk2(void);
void setup_gclk3(void);
void setup_gclk4(void);
void setup_gclk5(void);

void set_capture(void);

void port_set_pinmux(uint32_t pinmux);


uint16_t compute_baud(uint32_t fbaud, uint32_t fref, uint16_t SAMPR);
uint16_t compute_baud_frac(uint32_t fbaud, uint32_t fref, uint32_t Sperbit);

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif /* _SETUP_ */