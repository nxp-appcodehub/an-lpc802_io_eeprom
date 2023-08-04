/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include "LPC8xx.h"
#include "i2c.h"
#include "swm.h"
#include "syscon.h"
#include "uart.h"
#include "utilities.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#define SELF_SAVE_ADDRESS       (0x50)

void setup_debug_uart(void);


void app_i2c_master_wait_busy(void)
{
    int ret;
    while(1)
    {
        ret = i2c_start((SELF_SAVE_ADDRESS << 1) | 0);
        if(ret == 0)
        {
            i2c_stop();
            break;
        }
        i2c_stop();
    }
}

void app_i2c_master_write_eeporm(uint16_t addr, uint8_t *buf, uint16_t len)
{
    i2c_start((SELF_SAVE_ADDRESS << 1) | 0);
    i2c_send_byte((addr >> 0) & 0xFF);
    i2c_send_byte((addr >> 8) & 0xFF);
    while(len--)
    {
        i2c_send_byte(*buf++);
    }
    i2c_stop();
    
    app_i2c_master_wait_busy();
}



void app_i2c_master_read_eeporm(uint16_t addr, uint8_t *buf, uint16_t len)
{
    int i;
    
    
    i2c_start((SELF_SAVE_ADDRESS << 1) | 0);
    i2c_send_byte((addr >> 0) & 0xFF);
    i2c_send_byte((addr >> 8) & 0xFF);
    i2c_start((SELF_SAVE_ADDRESS << 1) | 1);
    for(i=0; i<len-1; i++)
    {
        buf[i] = i2c_read_byte(false);
    }
    buf[i] = i2c_read_byte(true);
    
    i2c_stop();
}

uint32_t app_i2c_master_eeporm_page_test(uint16_t addr)
{
    int i;
    
    uint8_t buf[64];
    for(i=0; i<64; i++)
    {
        buf[i] = i;
    }
    
    app_i2c_master_write_eeporm(addr, buf, 64);
    
    for(i=0; i<64; i++)
    {
        buf[i] = 0;
    }
    
    
    app_i2c_master_read_eeporm(addr, buf, 64);
    for(i=0; i<64; i++)
    {
        if(buf[i] != i)
        {
            return 1;
        }
    }
    return 0;
}

void app_i2c_master_read_test(void)
{
    int i, ret;
    for(i=0; i<11*1024; i+=64)
    {
        ret = app_i2c_master_eeporm_page_test(i);
        if(ret)
        {
            printf("addr:0x%X page:%d, ret:%s\r\n", i, i/64, (ret == 0)?("succ"):("fail"));
        }
    }
    
}

void i2c_master_init(void)
{
    // Provide main_clk as function clock to I2C0
    LPC_SYSCON->I2C0CLKSEL = FCLKSEL_MAIN_CLK;
  

    // Configure the SWM
    // On the LPC824, LPC84x I2C0_SDA and I2C0_SCL are fixed pin functions which are enabled / disabled in the pinenable0 register
    // On the LPC812, I2C0_SDA and I2C0_SCL are movable functions which are assigned using the pin assign registers
    LPC_SWM->PINENABLE0 &= ~(I2C0_SCL|I2C0_SDA); // Use for LPC824 and LPC84x
    //ConfigSWM(I2C0_SCL, P0_10);                // Use for LPC812
    //ConfigSWM(I2C0_SDA, P0_11);                // Use for LPC812

    // Give I2C0 a reset
    LPC_SYSCON->PRESETCTRL0 &= (I2C0_RST_N);
    LPC_SYSCON->PRESETCTRL0 |= ~(I2C0_RST_N);

    // Configure the I2C0 clock divider
    // Desired bit rate = Fscl = 100,000 Hz (1/Fscl = 10 us, 5 us low and 5 us high)
    // Use default clock high and clock low times (= 2 clocks each)
    // So 4 I2C_PCLKs = 100,000/second, or 1 I2C_PCLK = 400,000/second
    // I2C_PCLK = SystemClock = 30,000,000/second, so we divide by 30000000/400000 = 75
    // Remember, value written to DIV divides by value+1
    SystemCoreClockUpdate(); // Get main_clk frequency
    LPC_I2C0->DIV = 3;

    // Configure the I2C0 CFG register:
    // Master enable = true
    // Slave enable = true
    // Monitor enable = false
    // Time-out enable = false
    // Monitor function clock stretching = false
    //
    LPC_I2C0->CFG = CFG_MSTENA;
}

#define TICK_BEGIN  SysTick->LOAD = 0xFFFFFF;SysTick->VAL = 0; time = SysTick->VAL;
#define TICK_END    time = time - SysTick->VAL;


int main(void)
{
    uint8_t buf[64];
    uint32_t time;
    
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
    setup_debug_uart();

    printf("I2C master test for lpc802 eeporm demo CoreClock:%dHz\r\n", main_clk);
	
    // Enable bus clocks to I2C0, SWM
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (I2C0 | SWM);
    i2c_master_init();

    printf("chip test begin....\r\n");
    app_i2c_master_read_test();
    printf("chip test finish....\r\n");
    
    TICK_BEGIN;
    app_i2c_master_write_eeporm(0x0000, buf, 64);
    TICK_END;
    printf("write page(64byte) time:%d us\r\n", time / (main_clk/(1000*1000)));
    
    TICK_BEGIN;
    app_i2c_master_read_eeporm(0x0000, buf, 64);
    TICK_END;
    printf("read page(64byte) time:%d us\r\n", time / (main_clk/(1000*1000)));
    
    
    while(1)
    {


    }
}
