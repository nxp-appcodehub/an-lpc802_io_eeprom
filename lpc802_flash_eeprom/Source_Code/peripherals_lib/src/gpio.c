/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "LPC8xx.h"			/* LPC8xx Peripheral Registers */
#include "gpio.h"
#include "syscon.h"


/*****************************************************************************
** Function name:	GPIOInit
**
** Description:		Enable clock, then reset the GPIO module.
**                      See syscon.h for register bit definitions.
**
** Parameters:		None
**
** Returned value:	None
*****************************************************************************/
void GPIOInit( void )
{
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL[0] |= (GPIO0 | GPIO1);
	
  /* Peripheral reset to the GPIO module. '0' asserts, '1' deasserts reset. */
  LPC_SYSCON->PRESETCTRL[0] &= (GPIO0_RST_N & GPIO1_RST_N);
  LPC_SYSCON->PRESETCTRL[0] |= ~(GPIO0_RST_N & GPIO1_RST_N);
  return;
}


/*****************************************************************************
** Function name:		GPIOGetPinValue
**
** Description:		  Read Current state of port pin, PIN register value
**
** parameters:			port number, bit position
** Returned value:	The value, obviously
**
*****************************************************************************/
uint32_t GPIOGetPinValue( uint32_t portNum, uint32_t bitPosi )
{
  return (LPC_GPIO_PORT->PIN[portNum] & (1<<bitPosi));
}





/*****************************************************************************
** Function name:		GPIOSetBitValue
**
** Descriptions:		Set/clear a bit in a specific position
**
** parameters:			port num, bit position, bit value
** 						
** Returned value:		None
**
*****************************************************************************/
void GPIOSetBitValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal )
{
  if ( bitVal )
  {
		LPC_GPIO_PORT->SET[portNum] = 1<<bitPosi;
  }
  else
  {
		LPC_GPIO_PORT->CLR[portNum] = 1<<bitPosi;
  }
  return;
}


/*****************************************************************************
** Function name:		GPIOSetDir
**
** Descriptions:		Set the direction in GPIO port
**
** parameters:			portNum, bit position, direction (1 out, 0 input)
** 						
** Returned value:		None
**
*****************************************************************************/
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir )
{
  if( dir )
  {
		LPC_GPIO_PORT->DIRSET[portNum] = 1<<bitPosi;
  }
  else
  {
		LPC_GPIO_PORT->DIRCLR[portNum] = 1<<bitPosi;
  }
  return;
}




/******************************************************************************
**                            End Of File
******************************************************************************/
