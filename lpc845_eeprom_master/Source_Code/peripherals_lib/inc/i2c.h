/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef lpc8xx_I2C_H_
#define lpc8xx_I2C_H_

#include "lpc8xx.h"
#include <stdbool.h>


#define CFG_MSTENA				(1 << 0)
#define CFG_SLVENA				(1 << 1)
#define CFG_MONENA				(1 << 2)
#define CFG_TIMEOUTENA			(1 << 3)
#define CFG_MONCLKSTR			(1 << 4)

#define CTL_MSTCONTINUE			(1 << 0)
#define CTL_MSTSTART			(1 << 1)
#define CTL_MSTSTOP 			(1 << 2)

#define CTL_SLVCONTINUE			(1 << 0)
#define CTL_SLVNACK				(1 << 1)

// Below matches the Niobe2 user manual
#define I2C_STAT_MSTSTATE   (0x7<<1)
#define I2C_STAT_MSTST_IDLE (0x0<<1)
#define I2C_STAT_MSTST_RX   (0x1<<1)
#define I2C_STAT_MSTST_TX   (0x2<<1)

#define STAT_MSTPEND  		(1 << 0)
#define MASTER_STATE_MASK	(0x7<<1)
#define STAT_MSTIDLE	 	(0x0 << 1)
#define STAT_MSTRX	 		(0x1 << 1)
#define STAT_MSTTX	 		(0x2 << 1)
#define STAT_MSTNACKADDR	(0x3 << 1)
#define STAT_MSTNACKTX		(0x4 << 1)
#define STAT_MSTARBLOSS		(1 << 4)
#define STAT_MSTSSERR	 	(1 << 6)

#define STAT_SLVPEND 		(1 << 8)
#define SLAVE_STATE_MASK	(0x3<<9)
#define STAT_SLVADDR		(0x0 << 9)
#define STAT_SLVRX  	 	(0x1 << 9)
#define STAT_SLVTX  	 	(0x2 << 9)
#define STAT_SLVNOTSTR		(1 << 11)
#define STAT_SLVSEL		 	(1 << 14)
#define STAT_SLVDESEL		(1 << 15)

#define STAT_MONOVERRUN 	(1 << 17)
#define STAT_MONACTIVE		(1 << 18)
#define STAT_MONIDLE		(1 << 19)

#define STAT_EVTIMEOUT		(1 << 24)
#define STAT_SCLTIMEOUT		(1 << 25)




void WaitI2CMasterState(LPC_I2C_TypeDef * ptr_LPC_I2C, uint32_t state);
void WaitI2CSlaveState(LPC_I2C_TypeDef * ptr_LPC_I2C, uint32_t state);
void i2c_send_byte(uint8_t data);
uint8_t i2c_read_byte(bool last);
void i2c_stop(void);
int i2c_start(uint8_t addr);

#endif /* lpc8xx_I2C_H_ */

