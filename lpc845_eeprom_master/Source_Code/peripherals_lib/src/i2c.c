/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * i2c.c
 *
 *  Created on: Apr 5, 2016
 *
 */

#include "LPC8xx.h"
#include "utilities.h"
#include "i2c.h"


/*****************************************************************************
** Function name:		WaitI2CMasterState
**
** Description:		    Waits for I2C master pending, then compares the master
**                      state to the state parameter. If compare fails, enter
**                      a while(1) loop. If compare passes, return.
**
** parameters:
**                      ptr_LPC_I2C: A pointer to an I2C instance
**                      state: One of the 3-bit Master function state codes of the I2C
** Returned value:		None
**
*****************************************************************************/
void WaitI2CMasterState(LPC_I2C_TypeDef * ptr_LPC_I2C, uint32_t state) {

  while(!(ptr_LPC_I2C->STAT & STAT_MSTPEND));            // Wait for MSTPENDING bit set in STAT register
  if((ptr_LPC_I2C->STAT & MASTER_STATE_MASK) != state) { // If master state mismatch ...
    LED_Off(LED_GREEN);
    LED_On(LED_RED);
    while(1);                                            // die here and debug the problem
  }
  return;                                                // If no mismatch, return

}




/*****************************************************************************
** Function name:		WaitI2CSlaveState
**
** Description:		    Waits for I2C slave pending, then compares the slave
**                      state to the state parameter. If compare fails, enter
**                      a while(1) loop. If compare passes, return.
**
** parameters:
**                      ptr_LPC_I2C: A pointer to an I2C instance
**                      state: One of the 2-bit slave function state codes of the I2C
** Returned value:		None
**
*****************************************************************************/
void WaitI2CSlaveState(LPC_I2C_TypeDef * ptr_LPC_I2C, uint32_t state) {

  while(!(ptr_LPC_I2C->STAT & STAT_SLVPEND));         // Wait for SLVPENDING bit in STAT register
  if((ptr_LPC_I2C->STAT & SLAVE_STATE_MASK) != state) // If state mismatches
    while(1);                                         // Die here, and debug the problem
  return;                                             // Otherwise, return

}

void i2c_stop()
{
     while(!(LPC_I2C0->STAT & STAT_MSTPEND)) {}; 
    LPC_I2C0->MSTCTL = CTL_MSTSTOP;                    // Send a stop to end the transaction
}

int i2c_start(uint8_t addr)
{
    while(!(LPC_I2C0->STAT & STAT_MSTPEND)); 
    
    LPC_I2C0->MSTDAT = addr;    // Address with 0 for RWn bit (WRITE)
    LPC_I2C0->MSTCTL = CTL_MSTSTART;                   // Start the transaction by setting the MSTSTART bit to 1 in the Master control register.
    while(!(LPC_I2C0->STAT & STAT_MSTPEND)); 
    if((LPC_I2C0->STAT & MASTER_STATE_MASK) == STAT_MSTNACKADDR)
    {
        return 1;
    }
    return 0;
}

void i2c_send_byte(uint8_t data)
{
    LPC_I2C0->MSTDAT = data;                           // Send the data to the slave
    LPC_I2C0->MSTCTL = CTL_MSTCONTINUE;                // Continue the transaction
    WaitI2CMasterState(LPC_I2C0, I2C_STAT_MSTST_TX);   // Wait for the data to be ACK'd
}

uint8_t i2c_read_byte(bool last)
{
    uint8_t data;
    //WaitI2CMasterState(LPC_I2C0, I2C_STAT_MSTST_IDLE); // Wait the master state to be idle
    while(!(LPC_I2C0->STAT & STAT_MSTPEND)) {}; 
    if((LPC_I2C0->STAT & MASTER_STATE_MASK) == STAT_MSTRX)
    {
        data = LPC_I2C0->MSTDAT;
        if(last)
        {
            LPC_I2C0->MSTCTL = CTL_MSTSTOP;
        }
        else
        {
            LPC_I2C0->MSTCTL = CTL_MSTCONTINUE;
        }
        
    }
//    return LPC_I2C0->MSTDAT;
//        switch((stat & I2C_STAT_MSTSTATE_MASK) >> I2C_STAT_MSTSTATE_SHIFT)
//        {
//            case 0: /* idle */
//                break;
//            case 1: /* RX ready */
//                *(buf++) = I2CBases[instance]->MSTDAT;
//                (--len)?(I2CBases[instance]->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK):(I2CBases[instance]->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK);
//                break;
//            case 3: /* Nack */
//            case 4:
//                LIB_TRACE("NanK\r\n");
//                ret = CH_ERR;
//                break;
//        }
    return data;
}

