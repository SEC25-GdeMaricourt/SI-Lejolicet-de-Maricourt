#include <sam.h>
#include "setup.h"

#define MASK_OUPUT 1


void port_set_pinmux(uint32_t pinmux)
{
    uint32_t function = ( pinmux &0xFF);
    uint32_t port_nb  =((pinmux>>16)&0xFF) / 32;
    uint32_t pin      =((pinmux>>16)&0xFF) % 32;
    
    if (pin & 1) {
        PORT_REGS->GROUP[port_nb].PORT_PMUX[pin >> 1] &= ~PORT_PMUX_PMUXO_Msk;
        PORT_REGS->GROUP[port_nb].PORT_PMUX[pin >> 1] |= PORT_PMUX_PMUXO(function); // Odd-numbered pin
    }
    else {
        PORT_REGS->GROUP[port_nb].PORT_PMUX[pin >> 1] &= ~PORT_PMUX_PMUXE_Msk;
        PORT_REGS->GROUP[port_nb].PORT_PMUX[pin >> 1] |= PORT_PMUX_PMUXE(function); // Even-numbered pin
    }
    PORT_REGS->GROUP[port_nb].PORT_PINCFG[pin] |= PORT_PINCFG_PMUXEN(1); // Enable port mux
}

void setup(void)
{
  // GCLK 1 need for DPLL, need to be started 1st
  setup_gclk1();

  setup_gclk0();

  setup_gclk2();
  setup_gclk3();
  setup_gclk4();
  setup_gclk5();
}

void setup_gclk0(void)
{

#if (MASK_OUPUT==0)
  //Output GLCK[0] PB14 can be check on Extension Header EXT2 Pin 7
 port_set_pinmux(PINMUX_PB14M_GCLK_IO0);
 // Need 120Mhz Output, So Signal Need to be force
 PORT_REGS->GROUP[1].PORT_PINCFG[14] |= PORT_PINCFG_DRVSTR(1);

#endif

 //setup device at 120Mhz
 //XOSC32K Init
 if((OSC32KCTRL_REGS->OSC32KCTRL_STATUS & OSC32KCTRL_STATUS_XOSC32KRDY_Msk) != OSC32KCTRL_STATUS_XOSC32KRDY_Msk) {
    while(1) {
      // WARNING : XOSC32 not started
      __WFI();
    }
 }
 
 //FDPLL0 Init for 32kHz -> 120MHz
 OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLRATIO = OSCCTRL_DPLLRATIO_LDR(3661) | OSCCTRL_DPLLRATIO_LDRFRAC(4);
 OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLCTRLB = OSCCTRL_DPLLCTRLB_FILTER(0x0) | OSCCTRL_DPLLCTRLB_REFCLK(OSCCTRL_DPLLCTRLB_REFCLK_XOSC32_Val) | OSCCTRL_DPLLCTRLB_LBYPASS_Msk;
 OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLCTRLA = OSCCTRL_DPLLCTRLA_ENABLE_Msk;
 while((OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk) == OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk);
 while((OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSTATUS   & OSCCTRL_DPLLSTATUS_LOCK_Msk) != OSCCTRL_DPLLSTATUS_LOCK_Msk);
 while((OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSTATUS   & OSCCTRL_DPLLSTATUS_CLKRDY_Msk) != OSCCTRL_DPLLSTATUS_CLKRDY_Msk);
 
 //FDPLL0 on GCLK0
 GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DPLL0_Val) | 
                              GCLK_GENCTRL_IDC(1)                          |
                              GCLK_GENCTRL_OE(1)                           |
                              GCLK_GENCTRL_GENEN(1);

 while(GCLK_REGS->GCLK_SYNCBUSY != 0);

}


void setup_gclk1(void) 
{
#if (MASK_OUPUT==0)  
  // XOSC32K at 32768Hz output on PB15
  port_set_pinmux(PINMUX_PB15M_GCLK_IO1);
#endif
  

 //XOSC32K Init

 OSC32KCTRL_REGS->OSC32KCTRL_XOSC32K = OSC32KCTRL_XOSC32K_STARTUP(0x3) | 
                                       OSC32KCTRL_XOSC32K_EN32K_Msk    |
                                       OSC32KCTRL_XOSC32K_EN1K_Msk     |
                                       OSC32KCTRL_XOSC32K_WRTLOCK_Msk  |
                                       OSC32KCTRL_XOSC32K_CGM_HS       |
                                       OSC32KCTRL_XOSC32K_XTALEN_Msk   | 
                                       OSC32KCTRL_XOSC32K_ENABLE_Msk   ;

 while((OSC32KCTRL_REGS->OSC32KCTRL_STATUS & OSC32KCTRL_STATUS_XOSC32KRDY_Msk) != OSC32KCTRL_STATUS_XOSC32KRDY_Msk);
 
 // Select GCLK 1 and enable
 GCLK_REGS->GCLK_GENCTRL[1] =  GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_XOSC32K_Val)     | // Assign XOsc32K as input clock
                               GCLK_GENCTRL_OE(1)                                 | // Ouput enable
                               GCLK_GENCTRL_IDC(1)                                | // Generator duty cycle is 50/50
                               GCLK_GENCTRL_GENEN(1)                              ; // GLCK(1) Enable

// GENCTRL is Write-Synchronized...so wait for write to complete
while(GCLK_REGS->GCLK_SYNCBUSY != 0);

}

void setup_gclk2(void) 
{

#if (MASK_OUPUT==0)
  // External 12MHz Crystal (PB22 / PB23) XOSC1 putput on 
  port_set_pinmux(PINMUX_PB16M_GCLK_IO2);
  // Need 12Mhz Output, So Signal Need to be force
  PORT_REGS->GROUP[1].PORT_PINCFG[16] |= PORT_PINCFG_DRVSTR(1);
#endif

  OSCCTRL_REGS->OSCCTRL_XOSCCTRL[1] = OSCCTRL_XOSCCTRL_STARTUP(0x7)| 
                                      OSCCTRL_XOSCCTRL_IMULT(0x4)  | 
                                      OSCCTRL_XOSCCTRL_IPTAT(0x3)  | 
                                      OSCCTRL_XOSCCTRL_XTALEN(1)   | 
                                      OSCCTRL_XOSCCTRL_ENABLE(1)   ;

  // Update GCLK 2
  GCLK_REGS->GCLK_GENCTRL[2] =  GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_XOSC1_Val)     | // Assign XOsc32K as input clock
                                GCLK_GENCTRL_OE(1)                               | // Ouput enable
                                GCLK_GENCTRL_IDC(1)                              | // Generator duty cycle is 50/50
                                GCLK_GENCTRL_GENEN(1)                            ; // GLCK(2) Enable
  

   // GENCTRL is Write-Synchronized...so wait for write to complete
   while(GCLK_REGS->GCLK_SYNCBUSY != 0);

}

void setup_gclk3(void) 
{

#if (MASK_OUPUT==0)
  // External 120Mhs/15= 8Mhz Crystal EXT2 pin 13 -> PB17 
  port_set_pinmux(PINMUX_PB17M_GCLK_IO3);
#endif

 //FDPLL0 on GCLK0
 GCLK_REGS->GCLK_GENCTRL[3] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DPLL0_Val) | 
                              GCLK_GENCTRL_IDC(1)                          |
                              GCLK_GENCTRL_OE(1)                           |
                              GCLK_GENCTRL_DIVSEL(0)                       |
                              GCLK_GENCTRL_DIV(15)                         | // 120Mhz/15 >> 8Mhz
                              GCLK_GENCTRL_GENEN(1);

 while(GCLK_REGS->GCLK_SYNCBUSY != 0);
}

void setup_gclk4(void) 
{

#if (MASK_OUPUT==0)
  // XOSC32K at 1024Hz output on PB18 - > SD/SDIO Card Connection MCDA0 (DAT0)
  port_set_pinmux(PINMUX_PB18M_GCLK_IO4);
#endif
 
  //XOSC32K Init
  if((OSC32KCTRL_REGS->OSC32KCTRL_STATUS & OSC32KCTRL_STATUS_XOSC32KRDY_Msk) != OSC32KCTRL_STATUS_XOSC32KRDY_Msk) {
     while(1) {
       // WARNING : XOSC32 not started
       __WFI();
     }
  }
  
  // Select GCLK 4 
  GCLK_REGS->GCLK_GENCTRL[4] = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_XOSC32K_Val) | // Assign xOsc32K as input clock
                               GCLK_GENCTRL_OE(1)                             | // Ouput enable
                               GCLK_GENCTRL_DIVSEL(1)                         | // Set DIV selection to 1
                               GCLK_GENCTRL_DIV(4)                            | // Div by 2^(4+1) = 32
                               GCLK_GENCTRL_GENEN(1);       
   
   // GENCTRL is Write-Synchronized...so wait for write to complete
   while(GCLK_REGS->GCLK_SYNCBUSY != 0);

}

void setup_gclk5(void) 
{
#if (MASK_OUPUT==0)
  // External 12MHz/12 1Mhz output on PB19 - > SD/SDIO Card Connection MCDA1 (DAT1)
  port_set_pinmux(PINMUX_PB19M_GCLK_IO5);
#endif

  //XOSC1 Already Started ?
 if((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY1_Msk) != OSCCTRL_STATUS_XOSCRDY1_Msk) {
    while(1) {
      // WARNING : XOSC1 not started
      __WFI();
    }
 }

  // Update GCLK 2
  GCLK_REGS->GCLK_GENCTRL[5] =  GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_XOSC1_Val)     | // Assign XOsc32K as input clock
                                GCLK_GENCTRL_OE(1)                               | // Ouput enable
                                GCLK_GENCTRL_IDC(1)                              | // Generator duty cycle is 50/50
                                GCLK_GENCTRL_DIVSEL(0)                           | // Set DIV selection to 1
                                GCLK_GENCTRL_DIV(12)                             | // Div by 2^(4+1) = 32
                                GCLK_GENCTRL_GENEN(1)                            ; // GLCK(2) Enable
  

   // GENCTRL is Write-Synchronized...so wait for write to complete
   while(GCLK_REGS->GCLK_SYNCBUSY != 0);
}

void delay_us(uint32_t us) //The maximum delay is 349ms
{
  SysTick->VAL = 0;
  if (us > 349000)
    us = 349000;
  SysTick->LOAD = 48 * us; // 48Cycle at 48Mhz to have 1ms
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
  while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
    ;
  SysTick->CTRL = 0;
}

void delay_ms(uint32_t ms) {
  uint32_t i;
  i = ms / 349;
  while (i--)
    delay_us(349000);
  i = ms % 349;
  if (i)
    delay_us(i * 1000);
}


uint16_t compute_baud(uint32_t fbaud, uint32_t fref, uint16_t SAMPR)
{
    uint16_t BAUD = 0; // Register Value
    switch(SAMPR) {
    case 0:
        BAUD = (uint16_t)((float)65536.0*((float)1.0- ((float)16*((float)fbaud/(float)fref))));
        break;
    case 1:
        BAUD = compute_baud_frac(fbaud, fref, 16);
        break;
    case 2:
        BAUD = (uint16_t)((float)65536.0*((float)1.0- ((float)8*((float)fbaud/(float)fref))));
        break;
    case 3:
        BAUD = compute_baud_frac(fbaud, fref, 8);
        break;
    case 4:
        BAUD = (uint16_t)((float)65536.0*((float)1.0- ((float)3*((float)fbaud/(float)fref))));
        break;
    }
    return BAUD;
}

uint16_t compute_baud_frac(uint32_t fbaud, uint32_t fref, uint32_t Sperbit)
{
	uint16_t u16_baudrate_int;
	uint16_t u16_baudrate_fp;	
	uint16_t u16_baudrate_reg;		
	
	//compute integral and fractionnal part of baudrate reg
	float fTmp;
	fTmp = fref;
	fTmp = fTmp / fbaud;
	fTmp = fTmp / Sperbit;
	u16_baudrate_int = (uint16_t)fTmp;
	fTmp -= u16_baudrate_int;
	fTmp = fTmp*8;
	u16_baudrate_fp = (uint16_t)fTmp;
	u16_baudrate_reg = u16_baudrate_int | u16_baudrate_fp<<13;
	return(u16_baudrate_reg);
}