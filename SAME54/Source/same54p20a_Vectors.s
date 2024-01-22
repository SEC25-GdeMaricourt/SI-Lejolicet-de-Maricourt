/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2014 - 2021 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* - Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File      : same54p20a_Vectors.s
Purpose   : Exception and interrupt vectors for same54p20a devices.

Additional information:
  Preprocessor Definitions
    __NO_EXTERNAL_INTERRUPTS
      If defined,
        the vector table will contain only the internal exceptions
        and interrupts.
    __VECTORS_IN_RAM
      If defined,
        an area of RAM, large enough to store the vector table,
        will be reserved.

    __OPTIMIZATION_SMALL
      If defined,
        all weak definitions of interrupt handlers will share the
        same implementation.
      If not defined,
        all weak definitions of interrupt handlers will be defined
        with their own implementation.
*/
        .syntax unified

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/

//
// Directly place a vector (word) in the vector table
//
.macro VECTOR Name=
        .section .vectors, "ax"
        .code 16
        .word \Name
.endm

//
// Declare an exception handler with a weak definition
//
.macro EXC_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        .word \Name
        //
        // Insert dummy handler in init section
        //
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
.endm

//
// Declare an interrupt handler with a weak definition
//
.macro ISR_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        .word \Name
        //
        // Insert dummy handler in init section
        //
#if defined(__OPTIMIZATION_SMALL)
        .section .init, "ax"
        .weak \Name
        .thumb_set \Name,Dummy_Handler
#else
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
#endif
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED
        .section .vectors, "ax"
        .word 0
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED_DUMMY
        .section .vectors, "ax"
        .word Dummy_Handler
.endm

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
        .extern __stack_end__
        .extern Reset_Handler
        .extern HardFault_Handler

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*  Setup of the vector table and weak definition of interrupt handlers
*
*/
        .section .vectors, "ax"
        .code 16
        .balign 1024
        .global _vectors
_vectors:
        //
        // Internal exceptions and interrupts
        //
        VECTOR __stack_end__
        VECTOR Reset_Handler
        EXC_HANDLER NMI_Handler
        VECTOR HardFault_Handler
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        EXC_HANDLER SVC_Handler
        ISR_RESERVED
        ISR_RESERVED
        EXC_HANDLER PendSV_Handler
        EXC_HANDLER SysTick_Handler
        //
        // External interrupts
        //
#ifndef __NO_EXTERNAL_INTERRUPTS
        ISR_HANDLER PM_Handler
        ISR_HANDLER MCLK_Handler
        ISR_HANDLER OSCCTRL_XOSC0_Handler
        ISR_HANDLER OSCCTRL_XOSC1_Handler
        ISR_HANDLER OSCCTRL_DFLL_Handler
        ISR_HANDLER OSCCTRL_DPLL0_Handler
        ISR_HANDLER OSCCTRL_DPLL1_Handler
        ISR_HANDLER OSC32KCTRL_Handler
        ISR_HANDLER SUPC_OTHER_Handler
        ISR_HANDLER SUPC_BODDET_Handler
        ISR_HANDLER WDT_Handler
        ISR_HANDLER RTC_Handler
        ISR_HANDLER EIC_EXTINT_0_Handler
        ISR_HANDLER EIC_EXTINT_1_Handler
        ISR_HANDLER EIC_EXTINT_2_Handler
        ISR_HANDLER EIC_EXTINT_3_Handler
        ISR_HANDLER EIC_EXTINT_4_Handler
        ISR_HANDLER EIC_EXTINT_5_Handler
        ISR_HANDLER EIC_EXTINT_6_Handler
        ISR_HANDLER EIC_EXTINT_7_Handler
        ISR_HANDLER EIC_EXTINT_8_Handler
        ISR_HANDLER EIC_EXTINT_9_Handler
        ISR_HANDLER EIC_EXTINT_10_Handler
        ISR_HANDLER EIC_EXTINT_11_Handler
        ISR_HANDLER EIC_EXTINT_12_Handler
        ISR_HANDLER EIC_EXTINT_13_Handler
        ISR_HANDLER EIC_EXTINT_14_Handler
        ISR_HANDLER EIC_EXTINT_15_Handler
        ISR_HANDLER FREQM_Handler
        ISR_HANDLER NVMCTRL_0_Handler
        ISR_HANDLER NVMCTRL_1_Handler
        ISR_HANDLER DMAC_0_Handler
        ISR_HANDLER DMAC_1_Handler
        ISR_HANDLER DMAC_2_Handler
        ISR_HANDLER DMAC_3_Handler
        ISR_HANDLER DMAC_OTHER_Handler
        ISR_HANDLER EVSYS_0_Handler
        ISR_HANDLER EVSYS_1_Handler
        ISR_HANDLER EVSYS_2_Handler
        ISR_HANDLER EVSYS_3_Handler
        ISR_HANDLER EVSYS_OTHER_Handler
        ISR_HANDLER PAC_Handler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER RAMECC_Handler
        ISR_HANDLER SERCOM0_0_Handler
        ISR_HANDLER SERCOM0_1_Handler
        ISR_HANDLER SERCOM0_2_Handler
        ISR_HANDLER SERCOM0_OTHER_Handler
        ISR_HANDLER SERCOM1_0_Handler
        ISR_HANDLER SERCOM1_1_Handler
        ISR_HANDLER SERCOM1_2_Handler
        ISR_HANDLER SERCOM1_OTHER_Handler
        ISR_HANDLER SERCOM2_0_Handler
        ISR_HANDLER SERCOM2_1_Handler
        ISR_HANDLER SERCOM2_2_Handler
        ISR_HANDLER SERCOM2_OTHER_Handler
        ISR_HANDLER SERCOM3_0_Handler
        ISR_HANDLER SERCOM3_1_Handler
        ISR_HANDLER SERCOM3_2_Handler
        ISR_HANDLER SERCOM3_OTHER_Handler
        ISR_HANDLER SERCOM4_0_Handler
        ISR_HANDLER SERCOM4_1_Handler
        ISR_HANDLER SERCOM4_2_Handler
        ISR_HANDLER SERCOM4_OTHER_Handler
        ISR_HANDLER SERCOM5_0_Handler
        ISR_HANDLER SERCOM5_1_Handler
        ISR_HANDLER SERCOM5_2_Handler
        ISR_HANDLER SERCOM5_OTHER_Handler
        ISR_HANDLER SERCOM6_0_Handler
        ISR_HANDLER SERCOM6_1_Handler
        ISR_HANDLER SERCOM6_2_Handler
        ISR_HANDLER SERCOM6_OTHER_Handler
        ISR_HANDLER SERCOM7_0_Handler
        ISR_HANDLER SERCOM7_1_Handler
        ISR_HANDLER SERCOM7_2_Handler
        ISR_HANDLER SERCOM7_OTHER_Handler
        ISR_HANDLER CAN0_Handler
        ISR_HANDLER CAN1_Handler
        ISR_HANDLER USB_OTHER_Handler
        ISR_HANDLER USB_SOF_HSOF_Handler
        ISR_HANDLER USB_TRCPT0_Handler
        ISR_HANDLER USB_TRCPT1_Handler
        ISR_HANDLER GMAC_Handler
        ISR_HANDLER TCC0_OTHER_Handler
        ISR_HANDLER TCC0_MC0_Handler
        ISR_HANDLER TCC0_MC1_Handler
        ISR_HANDLER TCC0_MC2_Handler
        ISR_HANDLER TCC0_MC3_Handler
        ISR_HANDLER TCC0_MC4_Handler
        ISR_HANDLER TCC0_MC5_Handler
        ISR_HANDLER TCC1_OTHER_Handler
        ISR_HANDLER TCC1_MC0_Handler
        ISR_HANDLER TCC1_MC1_Handler
        ISR_HANDLER TCC1_MC2_Handler
        ISR_HANDLER TCC1_MC3_Handler
        ISR_HANDLER TCC2_OTHER_Handler
        ISR_HANDLER TCC2_MC0_Handler
        ISR_HANDLER TCC2_MC1_Handler
        ISR_HANDLER TCC2_MC2_Handler
        ISR_HANDLER TCC3_OTHER_Handler
        ISR_HANDLER TCC3_MC0_Handler
        ISR_HANDLER TCC3_MC1_Handler
        ISR_HANDLER TCC4_OTHER_Handler
        ISR_HANDLER TCC4_MC0_Handler
        ISR_HANDLER TCC4_MC1_Handler
        ISR_HANDLER TC0_Handler
        ISR_HANDLER TC1_Handler
        ISR_HANDLER TC2_Handler
        ISR_HANDLER TC3_Handler
        ISR_HANDLER TC4_Handler
        ISR_HANDLER TC5_Handler
        ISR_HANDLER TC6_Handler
        ISR_HANDLER TC7_Handler
        ISR_HANDLER PDEC_OTHER_Handler
        ISR_HANDLER PDEC_MC0_Handler
        ISR_HANDLER PDEC_MC1_Handler
        ISR_HANDLER ADC0_OTHER_Handler
        ISR_HANDLER ADC0_RESRDY_Handler
        ISR_HANDLER ADC1_OTHER_Handler
        ISR_HANDLER ADC1_RESRDY_Handler
        ISR_HANDLER AC_Handler
        ISR_HANDLER DAC_OTHER_Handler
        ISR_HANDLER DAC_EMPTY_0_Handler
        ISR_HANDLER DAC_EMPTY_1_Handler
        ISR_HANDLER DAC_RESRDY_0_Handler
        ISR_HANDLER DAC_RESRDY_1_Handler
        ISR_HANDLER I2S_Handler
        ISR_HANDLER PCC_Handler
        ISR_HANDLER AES_Handler
        ISR_HANDLER TRNG_Handler
        ISR_HANDLER ICM_Handler
        ISR_HANDLER PUKCC_Handler
        ISR_HANDLER QSPI_Handler
        ISR_HANDLER SDHC0_Handler
        ISR_HANDLER SDHC1_Handler
#endif
        //
        .section .vectors, "ax"
_vectors_end:

#ifdef __VECTORS_IN_RAM
        //
        // Reserve space with the size of the vector table
        // in the designated RAM section.
        //
        .section .vectors_ram, "ax"
        .balign 1024
        .global _vectors_ram

_vectors_ram:
        .space _vectors_end - _vectors, 0
#endif

/*********************************************************************
*
*  Dummy handler to be used for reserved interrupt vectors
*  and weak implementation of interrupts.
*
*/
        .section .init.Dummy_Handler, "ax"
        .thumb_func
        .weak Dummy_Handler
        .balign 2
Dummy_Handler:
        1: b 1b   // Endless loop


/*************************** End of file ****************************/
