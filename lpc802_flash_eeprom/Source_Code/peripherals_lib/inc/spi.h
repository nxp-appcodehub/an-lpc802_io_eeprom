/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/



#ifndef lpc8xx_SPI_H_
#define lpc8xx_SPI_H_


#include "LPC8xx.h"




#define CFG_ENABLE			(1 << 0)
#define CFG_MASTER			(1 << 2)
#define CFG_SLAVE				(0 << 2)
#define CFG_LSBF				(1 << 3)
#define CFG_CPHA				(1 << 4)
#define CFG_CPOL				(1 << 5)
#define CFG_MOSIDRV			(1 << 6)
#define CFG_LOOPBACK		(1 << 7)
#define CFG_SPOL            (1<<8)
//#define CFG_SPOL(s)			(1 << (8 + s))

#define DLY_PREDELAY(d)		((d) << 0)
#define DLY_POSTDELAY(d)	((d) << 4)
#define DLY_FRAMEDELAY(d)	((d) << 8)
#define DLY_INTERDELAY(d)	((d) << 12)

#define STAT_RXRDY 				(1 << 0)
#define STAT_TXRDY 				(1 << 1)
#define STAT_RXOVERRUN 		(1 << 2)
#define STAT_TXUNDERRUN 	(1 << 3)
#define STAT_SELNASSERT 	(1 << 4)
#define STAT_SELNDEASSERT (1 << 5)
#define STAT_CLKSTALL 		(1 << 6)
#define STAT_ET             (1 << 7)
#define STAT_MSTIDLE        (1<<8)
#define STAT_ERROR_MASK		(STAT_RXOVERRUN|STAT_TXUNDERRUN|STAT_SELNASSERT|STAT_SELNDEASSERT|STAT_CLKSTALL)

#define RXRDYEN             (1<<0)
#define TXRDYEN             (1<<1)
#define RXOVEN              (1<<2)
#define TXUREN              (1<<3)
#define SSAEN               (1<<4)
#define SSDEN               (1<<5)

#define SPIRXRDY             (1<<0)
#define SPITXRDY             (1<<1)
#define SPIRXOV              (1<<2)
#define SPITXUR              (1<<3)
#define SPISSA               (1<<4)
#define SPISSD               (1<<5)

#define CTL_TXSSELN   (1<<16)
#define CTL_EOT       (1<<20)
#define CTL_EOF       (1<<21)
#define CTL_RXIGNORE  (1<<22)
#define CTL_LEN(b)    (((b)-1)<<24)

#define TXDATCTL_SSELN(s) 	(s << 16)
#define TXDATCTL_EOT 				(1 << 20)
#define TXDATCTL_EOF 				(1 << 21)
#define TXDATCTL_RX_IGNORE	(1 << 22)
#define TXDATCTL_FSIZE(s)		((s) << 24)

#define RXDAT_SOT						(1 << 20)



#endif /* lpc8xx_SPI_H_ */
