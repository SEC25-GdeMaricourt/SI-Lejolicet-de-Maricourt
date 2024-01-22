# 0 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
# 91 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
        .syntax unified
# 119 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
        .global reset_handler
        .global Reset_Handler
        .equ reset_handler, Reset_Handler
        .section .init.Reset_Handler, "ax"
        .balign 2
        .thumb_func
Reset_Handler:






        movs R0, #8
        ldr R0, [R0]
        nop
        nop





        bl SystemInit
# 188 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
        movw R0, 0xED88
        movt R0, 0xE000
        ldr R1, [R0]
        orrs R1, R1, #(0xf << 20)
        str R1, [R0]




        bl _start





        .weak SystemInit





        .section .init_array, "aw"
        .balign 4
        .word SystemCoreClockUpdate
# 234 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
        .weak HardFault_Handler
        .section .init.HardFault_Handler, "ax"
        .balign 2
        .thumb_func
HardFault_Handler:



        ldr R1, =0xE000ED2C
        ldr R2, [R1]
        cmp R2, #0

.LHardFault_Handler_hfLoop:
        bmi .LHardFault_Handler_hfLoop
# 262 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
        tst LR, #4
        ite eq
        mrseq R0, MSP
        mrsne R0, PSP
# 276 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/SAME54/Source/SAME54_Startup.s"
        orr R2, R2, #0x80000000
        str R2, [R1]




        ldr R1, [R0, #24]
        adds R1, #2
        str R1, [R0, #24]

        bx LR
