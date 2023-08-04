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
#include "syscon.h"
#include "utilities.h"
#include "i2c.h"

void app_i2c_slave_init(uint8_t slv_addr)
{
    ConfigSWM(I2C0_SDA, P0_10);
    ConfigSWM(I2C0_SCL, P0_16);
    
    /* using main clock */
    LPC_SYSCON->I2C0CLKSEL = 1;
    
    /* give I2C a reset */
    LPC_SYSCON->PRESETCTRL[0] &= (I2C0_RST_N);
    LPC_SYSCON->PRESETCTRL[0] |= ~(I2C0_RST_N);
    
    LPC_I2C0->DIV = 5;
    LPC_I2C0->CFG = CFG_MSTENA | CFG_SLVENA;
    
    LPC_I2C0->SLVADR0 = (slv_addr << 1) | 0;

    // Enable the I2C0 slave pending interrupt
    LPC_I2C0->INTENSET = STAT_SLVPEND | STAT_SLVDESEL;
    NVIC_EnableIRQ(I2C0_IRQn);
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
    
void i2c_stop()
{
     while(!(LPC_I2C0->STAT & STAT_MSTPEND)) {}; 
    LPC_I2C0->MSTCTL = CTL_MSTSTOP;                    // Send a stop to end the transaction
}

void i2c_start(uint8_t addr)
{
    while(!(LPC_I2C0->STAT & STAT_MSTPEND)); 
    
    LPC_I2C0->MSTDAT = addr;    // Address with 0 for RWn bit (WRITE)
    LPC_I2C0->MSTCTL = CTL_MSTSTART;                   // Start the transaction by setting the MSTSTART bit to 1 in the Master control register.
    while(!(LPC_I2C0->STAT & STAT_MSTPEND)); 
}

void WaitI2CMasterState(LPC_I2C_TypeDef * ptr_LPC_I2C, uint32_t state) {

  while(!(ptr_LPC_I2C->STAT & STAT_MSTPEND));            // Wait for MSTPENDING bit set in STAT register
  if((ptr_LPC_I2C->STAT & MASTER_STATE_MASK) != state) { // If master state mismatch ...
    LED_Off(LED_GREEN);
    LED_On(LED_RED);
    while(1);                                            // die here and debug the problem
  }
  return;                                                // If no mismatch, return

}



void WaitI2CSlaveState(LPC_I2C_TypeDef * ptr_LPC_I2C, uint32_t state) {

  while(!(ptr_LPC_I2C->STAT & STAT_SLVPEND));         // Wait for SLVPENDING bit in STAT register
  if((ptr_LPC_I2C->STAT & SLAVE_STATE_MASK) != state) // If state mismatches
    while(1);                                         // Die here, and debug the problem
  return;                                             // Otherwise, return

}

