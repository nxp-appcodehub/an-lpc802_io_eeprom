/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "LPC8xx.h"
#include "syscon.h"
#include "uart.h"
#include "LPC8xx.h"
#include "swm.h"
#include "common.h"

int Putc(uint8_t data)
{
    while (!((LPC_USART0->STAT) & TXRDY)); // Wait for TX Ready
    LPC_USART0->TXDAT  = data;             // Write the character to the TX buffer
    return 0;
}

int Getc(void)
{
    while (!((LPC_USART0->STAT) & RXRDY));   // Wait for RX Ready
    return LPC_USART0->RXDAT ;  
}


void app_uart_init(void)
{
  // Connect UART0 TXD, RXD signals to port pins
  ConfigSWM(U0_TXD, P0_14);
  ConfigSWM(U0_RXD, P0_0);

  // Configure UART0 for 9600 baud, 8 data bits, no parity, 1 stop bit.
  // For asynchronous mode (UART mode) the formula is:
  // (BRG + 1) * (1 + (m/256)) * (16 * baudrate Hz.) = FRG_in Hz.
  // We proceed in 2 steps.
  // Step 1: Let m = 0, and round (down) to the nearest integer value of BRG for the desired baudrate.
  // Step 2: Plug in the BRG from step 1, and find the nearest integer value of m, (for the FRG fractional part).
  //
  // Step 1 (with m = 0)
  // BRG = ((FRG_in Hz.) / (16 * baudrate Hz.)) - 1
  //     = (15,000,000/(16 * 9600)) - 1
  //     = 96.66
  //     = 97 (rounded)
  //
  // Step 2.
  // m = 256 * [-1 + {(FRG_in Hz.) / (16 * baudrate Hz.)(BRG + 1)}]
  //   = 256 * [-1 + {(15,000,000) / (16*9600)(98)}]
  //   = -0.004
  //   = 0 (rounded)

  // Configure FRG0
  LPC_SYSCON->FRG0MULT = 0; 
  LPC_SYSCON->FRG0DIV = 255;

  // Select main_clk as the source for FRG0
  LPC_SYSCON->FRG0CLKSEL = FRGCLKSEL_MAIN_CLK;

  // Select frg0clk as the source for fclk0 (to UART0)
  LPC_SYSCON->UART0CLKSEL = FCLKSEL_FRG0CLK;
	
  // Give USART0 a reset
  LPC_SYSCON->PRESETCTRL0 &= (UART0_RST_N);
  LPC_SYSCON->PRESETCTRL0 |= ~(UART0_RST_N);

  // Configure the USART0 baud rate generator
  LPC_USART0->BRG = 7;

  // Configure the USART0 CFG register:
  // 8 data bits, no parity, one stop bit, no flow control, asynchronous mode
  LPC_USART0->CFG = DATA_LENG_8 | PARITY_NONE | STOP_BIT_1;

  // Configure the USART0 CTL register (nothing to be done here)
  // No continuous break, no address detect, no Tx disable, no CC, no CLRCC
  LPC_USART0->CTL = 0;

  // Clear any pending flags, just in case
  LPC_USART0->STAT = 0xFFFF;

  // Enable USART0
  LPC_USART0->CFG |= UART_EN;

    // Enable the USART0 RX Ready Interrupt
    LPC_USART0->INTENSET = RXRDY;
    NVIC_EnableIRQ(UART0_IRQn);

  
    SetConsole(Putc, Getc);
}











