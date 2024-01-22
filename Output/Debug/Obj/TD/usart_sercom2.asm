	.cpu cortex-m4
	.arch armv7e-m
	.fpu fpv4-sp-d16
	.eabi_attribute 27, 1
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 38, 1
	.eabi_attribute 18, 4
	.file	"usart_sercom2.c"
	.text
.Ltext0:
	.file 1 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/usart_sercom2.c"
	.section	.text.__NVIC_EnableIRQ,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.type	__NVIC_EnableIRQ, %function
__NVIC_EnableIRQ:
.LFB120:
	.file 2 "/Users/amaury/Documents/Centrale Nantes/Cours/SEC2/SI/TD A faire/TD/CMSIS_5/CMSIS/Core/Include/core_cm4.h"
	.loc 2 1685 1
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #8
.LCFI0:
	mov	r3, r0
	strh	r3, [sp, #6]	@ movhi
	.loc 2 1686 6
	ldrsh	r3, [sp, #6]
	cmp	r3, #0
	blt	.L3
	.loc 2 1688 5
	.loc 2 1689 81
	ldrh	r3, [sp, #6]
	and	r2, r3, #31
	.loc 2 1689 9
	ldr	r1, .L4
	.loc 2 1689 18
	ldrsh	r3, [sp, #6]
	.loc 2 1689 34
	lsrs	r3, r3, #5
	.loc 2 1689 45
	movs	r0, #1
	lsl	r2, r0, r2
	.loc 2 1689 43
	str	r2, [r1, r3, lsl #2]
	.loc 2 1690 5
.L3:
	.loc 2 1692 1
	nop
	add	sp, sp, #8
.LCFI1:
	@ sp needed
	bx	lr
.L5:
	.align	2
.L4:
	.word	-536813312
.LFE120:
	.size	__NVIC_EnableIRQ, .-__NVIC_EnableIRQ
	.section	.text.__NVIC_ClearPendingIRQ,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.type	__NVIC_ClearPendingIRQ, %function
__NVIC_ClearPendingIRQ:
.LFB125:
	.loc 2 1776 1
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #8
.LCFI2:
	mov	r3, r0
	strh	r3, [sp, #6]	@ movhi
	.loc 2 1777 6
	ldrsh	r3, [sp, #6]
	cmp	r3, #0
	blt	.L8
	.loc 2 1779 81
	ldrh	r3, [sp, #6]
	and	r2, r3, #31
	.loc 2 1779 9
	ldr	r1, .L9
	.loc 2 1779 18
	ldrsh	r3, [sp, #6]
	.loc 2 1779 34
	lsrs	r3, r3, #5
	.loc 2 1779 45
	movs	r0, #1
	lsl	r2, r0, r2
	.loc 2 1779 43
	adds	r3, r3, #96
	str	r2, [r1, r3, lsl #2]
.L8:
	.loc 2 1781 1
	nop
	add	sp, sp, #8
.LCFI3:
	@ sp needed
	bx	lr
.L10:
	.align	2
.L9:
	.word	-536813312
.LFE125:
	.size	__NVIC_ClearPendingIRQ, .-__NVIC_ClearPendingIRQ
	.section	.text.__NVIC_SetPriority,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.type	__NVIC_SetPriority, %function
__NVIC_SetPriority:
.LFB127:
	.loc 2 1815 1
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #8
.LCFI4:
	mov	r3, r0
	str	r1, [sp]
	strh	r3, [sp, #6]	@ movhi
	.loc 2 1816 6
	ldrsh	r3, [sp, #6]
	cmp	r3, #0
	blt	.L12
	.loc 2 1818 48
	ldr	r3, [sp]
	uxtb	r2, r3
	.loc 2 1818 9
	ldr	r1, .L15
	.loc 2 1818 15
	ldrsh	r3, [sp, #6]
	.loc 2 1818 48
	lsls	r2, r2, #5
	uxtb	r2, r2
	.loc 2 1818 46
	add	r3, r3, r1
	strb	r2, [r3, #768]
	.loc 2 1824 1
	b	.L14
.L12:
	.loc 2 1822 48
	ldr	r3, [sp]
	uxtb	r2, r3
	.loc 2 1822 8
	ldr	r1, .L15+4
	.loc 2 1822 32
	ldrh	r3, [sp, #6]
	and	r3, r3, #15
	.loc 2 1822 40
	subs	r3, r3, #4
	.loc 2 1822 48
	lsls	r2, r2, #5
	uxtb	r2, r2
	.loc 2 1822 46
	add	r3, r3, r1
	strb	r2, [r3, #24]
.L14:
	.loc 2 1824 1
	nop
	add	sp, sp, #8
.LCFI5:
	@ sp needed
	bx	lr
.L16:
	.align	2
.L15:
	.word	-536813312
	.word	-536810240
.LFE127:
	.size	__NVIC_SetPriority, .-__NVIC_SetPriority
	.section	.bss.inputBuffer,"aw",%nobits
	.align	2
	.type	inputBuffer, %object
	.size	inputBuffer, 80
inputBuffer:
	.space	80
	.section	.bss.first,"aw",%nobits
	.align	2
	.type	first, %object
	.size	first, 4
first:
	.space	4
	.section	.bss.last,"aw",%nobits
	.align	2
	.type	last, %object
	.size	last, 4
last:
	.space	4
	.section	.bss.nb_message,"aw",%nobits
	.align	2
	.type	nb_message, %object
	.size	nb_message, 4
nb_message:
	.space	4
	.section	.bss.overflow,"aw",%nobits
	.align	2
	.type	overflow, %object
	.size	overflow, 4
overflow:
	.space	4
	.section	.text.sercom2_init,"ax",%progbits
	.align	1
	.global	sercom2_init
	.syntax unified
	.thumb
	.thumb_func
	.type	sercom2_init, %function
sercom2_init:
.LFB146:
	.loc 1 1