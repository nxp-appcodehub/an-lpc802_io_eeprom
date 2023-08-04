/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <string.h>
#include "LPC8xx.h"
#include "syscon.h"
#include "swm.h"
#include "syscon.h"
#include "utilities.h"
#include "uart.h"
#include "i2c.h"
#include "mq.h"
#include "common.h"
#include "flash.h"
#include "app.h"

#define SELF_SAVE_ADDRESS       (0x50)
#define BUFFER_SIZE             (64 + 2) /* page size plus 2 byte address */
#define EEPORM_TO_FLASH_OFFSET  (4*1024)

enum
{
    kI2C_SLAVE,
    kI2C_MASTER_TEST,
};

typedef struct
{
    uint8_t chip_addr;
    uint32_t data_addr;
    uint16_t len;
    uint8_t buf[BUFFER_SIZE];
}i2c_msg_t;

static i2c_msg_t i2c_msg;
static bool busy = false;


static uint16_t address_parse(uint16_t i2c_addr)
{
    return (i2c_addr % (12*1024)) + EEPORM_TO_FLASH_OFFSET;
}

int main(void)
{
    volatile msg_t *pMsg;
    int i;
    static uint8_t temp_buf[64];
    
    DelayInit();
    
    // Enable clocks to relevant peripherals
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (UART0 | SWM | I2C0);

    FLASH_Init();
    app_uart_init();
    app_i2c_slave_init(SELF_SAVE_ADDRESS);
    
    mq_init();

    LIB_TRACE("lpc802 eeporm frimware\r\n");
    while(1)
    {
        if(mq_exist())
        {
            pMsg = mq_pop();
            if(pMsg->cmd == kI2C_SLAVE)
            {
                /* it's a write operation  */
                if((i2c_msg.chip_addr & 0x01) == 0x00)
                {
                    
                    LIB_TRACE("write operation: chip addr: 0x%X\r\n", i2c_msg.chip_addr);
                    
                    i2c_msg.data_addr = address_parse(i2c_msg.buf[0] + (i2c_msg.buf[1]<<8));
                    
                    uint16_t len;
                    ((i2c_msg.len - 2) <= 64)?(len = i2c_msg.len - 2):(len = 64);
                    memcpy(i2c_msg.buf, i2c_msg.buf + 2, len);
                    
                    /* copy page data */
                    uint32_t page_base_addr = RT_ALIGN_DOWN(i2c_msg.data_addr, 64);
                    memcpy(temp_buf, (void*)page_base_addr, 64);
                    //printf("page_base_addr:0x%X\r\n", page_base_addr);
                    
                    /* replace data */
                    memcpy(temp_buf + i2c_msg.data_addr - page_base_addr, i2c_msg.buf, len);
                    
                    for(i=0; i<len; i++)
                    {
                        LIB_TRACE("%02X ", i2c_msg.buf[i]);
                    }
                    LIB_TRACE("\r\n");
                    
                    
                    LIB_TRACE("flash addr: 0x%X 0x%X\r\n", i2c_msg.data_addr, page_base_addr);
                    FLASH_ErasePage(page_base_addr);
                    FLASH_WritePage(page_base_addr, temp_buf);
                    busy = false;
                    
                }
                else
                {
                    LIB_TRACE("msg: a read operation\r\n");
                }
            }
            
            if(pMsg->cmd == kI2C_MASTER_TEST)
            {
                LIB_TRACE("ch:%c\r\n", pMsg->len);
                if(pMsg->len == 'w')
                {
                    //app_i2c_master_write_test();
                }
                
                if(pMsg->len == 'r')
                {
                    //app_i2c_master_read_test();
                }
            }

        }

    }

}



void I2C0_IRQHandler(void)
{
    uint32_t data;
    static uint32_t slv_rx_counter = 0;
    static uint8_t *p;
    static msg_t msg;
    
    if(LPC_I2C0->STAT & STAT_SLVDESEL)
    {
        LPC_I2C0->STAT |= STAT_SLVDESEL;
        
        i2c_msg.len = slv_rx_counter;
        if(i2c_msg.len != 0)
        {
            busy = true;
            msg.cmd = kI2C_SLAVE;
            mq_push(msg);
            LIB_TRACE("slv SLVDESE, push msg\r\n");
        }

        p = NULL;
        slv_rx_counter = 0;
        return;
    }
    
    if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVADDR)
    {
        if(busy == false)
        {
            LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;                     // ACK the address
            slv_rx_counter = 0;
            i2c_msg.chip_addr = LPC_I2C0->SLVDAT;
            
            /* setup read */
            if(i2c_msg.chip_addr & 0x01)
            {
                i2c_msg.data_addr = address_parse(i2c_msg.buf[0] + (i2c_msg.buf[1]<<8));
                LIB_TRACE("a read is required addr:0x%X\r\n", i2c_msg.data_addr);
                p = (uint8_t *)i2c_msg.data_addr;
            }
        }
        else
        {
            LPC_I2C0->SLVCTL = CTL_SLVNACK;
        }
        return;
    }

    /* salve sending data */
    if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVTX)
    {
        if((uint32_t)p >= EEPORM_TO_FLASH_OFFSET)
        {
            LPC_I2C0->SLVDAT = *p++;
        }
        else
        {
            LPC_I2C0->SLVDAT = 0;
        }
        LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;
        return;
    }
    
    if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVRX)
    {
        if(slv_rx_counter < BUFFER_SIZE)
        {
            data = LPC_I2C0->SLVDAT;                                // Read the data
            i2c_msg.buf[slv_rx_counter ++ ] = data;
            LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;                     // ACK the data
            return;
        }
        else
        {
            LIB_TRACE("OVERFLOW\r\n");
            slv_rx_counter = 0;
        }
        return;
    }

    LIB_TRACE("ERROR!\r\n");
    return;
}

void UART0_IRQHandler(void)
{
    uint8_t data;
    msg_t msg;
    msg.cmd = kI2C_MASTER_TEST;
    
    if(LPC_USART0->STAT & RXRDY)
    {
        data = LPC_USART0->RXDAT;
        msg.len = data;
        mq_push(msg);
    }
}

void HardFault_Handler(void)
{
    LIB_TRACE("HardFault_Handler\r\n");
    while(1);
}

