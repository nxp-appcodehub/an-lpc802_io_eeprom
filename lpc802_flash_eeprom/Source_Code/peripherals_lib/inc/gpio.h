/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef __LPC8XX_GPIO_H 
#define __LPC8XX_GPIO_H


#define INPUT  0
#define OUTPUT 1

#define PORT0			0
#define PORT1			1

#define Bit0            0
#define Bit1            1
#define Bit2            2
#define Bit3            3
#define Bit4            4
#define Bit5            5
#define Bit6            6
#define Bit7            7
#define Bit8            8
#define Bit9            9
#define Bit10           10
#define Bit11           11
#define Bit12           12
#define Bit13           13
#define Bit14           14
#define Bit15           15
#define Bit16           16
#define Bit17           17
#define Bit18           18
#define Bit19           19
#define Bit20           20
#define Bit21           21
#define Bit22           22
#define Bit23           23
#define Bit24           24
#define Bit25           25
#define Bit26           26
#define Bit27           27
#define Bit28           28
#define Bit29           29
#define Bit30           30
#define Bit31           31


void PININT_Handler ( uint32_t irq_num );
void PININT0_IRQHandler(void);
void PININT1_IRQHandler(void);
void PININT2_IRQHandler(void);
void PININT3_IRQHandler(void);
void PININT4_IRQHandler(void);
void PININT5_IRQHandler(void);
void PININT6_IRQHandler(void);
void PININT7_IRQHandler(void);

void GPIOInit( void );
uint32_t GPIOGetPinValue( uint32_t portNum, uint32_t bitPosi );
void GPIOSetBitValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal );
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir );

#endif /* end __LPC8XX_GPIO_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
