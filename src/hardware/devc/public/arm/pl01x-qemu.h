/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdint.h>

#ifndef	__ARM_PL011_INC__
#define	__ARM_PL011_INC__

#define	PL011_SIZE		0x100

/*
 * --------------------------------------------------------------------------
 * PL011 UART Registers
 * --------------------------------------------------------------------------
 */
#define	PL011_DR		0	/* Data register */
#define	PL011_RSR		1	/* Receive status register */
#define	PL011_ECR		1	/* Error Clear register */
//#define	PL011_DMAWM		0x08	/* DMA Water Mark register */
//#define	PL011_TIMEOUT	0x0C	/* Timeour register */
#define	PL011_FR		6	/* Flag register */
//#define	PL011_LCR_R		0x1C	/* Receive Line control register */
#define	PL011_ILPR		8	/* IrDA low-power counter register */
#define	PL011_IBRD		9	/* Integer baud rate register */
#define	PL011_FBRD		10	/* Fractional baud rate register */
#define	PL011_LCR_H		11	/* TX Line control register */
#define	PL011_CR		12	/* Control register */
#define	PL011_IFLS		13	/* Interrupt FIFO level select register */
#define	PL011_IMSC		14	/* Interrupt mask set/clear register */
#define	PL011_RIS		15	/* Raw interrupt status register */
#define	PL011_MIS		16	/* Masked interrupt status register */
#define	PL011_ICR		17	/* Interrupt clear register */
#define	PL011_DMACR		18	/* DMA control register */
#define PL011_TOTAL_REG_NUM 19
/*
 * DR register bits
 */
#define	PL011_DR_OE			(1 << 11)	/* Overrun error */
#define	PL011_DR_BE			(1 << 10)	/* Break error */
#define	PL011_DR_PE			(1 <<  9)	/* Parity error */
#define	PL011_DR_FE			(1 <<  8)	/* Frame error */

/*
 * RSR register bits
 */
#define	PL011_RSR_OE		(1 << 3)	/* Overrun error */
#define	PL011_RSR_BE		(1 << 2)	/* Break error */
#define	PL011_RSR_PE		(1 << 1)	/* Parity error */
#define	PL011_RSR_FE		(1 << 0)	/* Frame error */

/*
 * ECR register bits
 */
#define	PL011_ECR_OE		(1 << 3)	/* Overrun error */
#define	PL011_ECR_BE		(1 << 2)	/* Break error */
#define	PL011_ECR_PE		(1 << 1)	/* Parity error */
#define	PL011_ECR_FE		(1 << 0)	/* Frame error */

/*
 * FR register bits
 */
#define	PL011_FR_TXFE		(1 << 7)	/* Transmit FIFO empty */
#define	PL011_FR_RXFF		(1 << 6)	/* Receive FIFO full */
#define	PL011_FR_TXFF		(1 << 5)	/* Transmit FIFO full */
#define	PL011_FR_RXFE		(1 << 4)	/* Receive FIFO empty */
#define	PL011_FR_BUSY		(1 << 3)	/* UART Busy */
#define	PL011_FR_DCD		(1 << 2)	/* Data carrier detect */
#define	PL011_FR_DSR		(1 << 1)	/* Data set ready */
#define	PL011_FR_CTS		(1 << 0)	/* Clear to send */


/*
 * LCR_H register bits
 */
#define	PL011_LCR_H_SPS		(1 << 7)	/* Stick parity select */
#define	PL011_LCR_H_WLEN8	(3 << 5)	/* 8 bit len */
#define	PL011_LCR_H_WLEN7	(2 << 5)	/* 7 bit len */
#define	PL011_LCR_H_WLEN6	(1 << 5)	/* 6 bit len */
#define	PL011_LCR_H_WLEN5	(0 << 5)	/* 5 bit len */
#define	PL011_LCR_H_FEN		(1 << 4)	/* Enable FIFOs */
#define	PL011_LCR_H_STP2	(1 << 3)	/* 2 Stop bits select */
#define	PL011_LCR_H_EPS		(1 << 2)	/* Even parity select */
#define	PL011_LCR_H_PEN		(1 << 1)	/* Parity enable */
#define	PL011_LCR_H_BRK		(1 << 0)	/* Send break */


/*
 * CR register bits
 */
#define	PL011_CR_CTSEn		(1 << 15)	/* CTS hardware flow control enable */
#define	PL011_CR_RTSEn		(1 << 14)	/* RTS hardware flow control enable */
#define	PL011_CR_RTS		(1 << 11)	/* Request to send */
#define	PL011_CR_DTR		(1 << 10)	/* Data transmit ready */
#define	PL011_CR_RXE		(1 <<  9)	/* Receive enable */
#define	PL011_CR_TXE		(1 <<  8)	/* Transmit enable */
#define	PL011_CR_LBE		(1 <<  7)	/* Loop back enable */
#define	PL011_CR_OVSFACT	(1 <<  3)	/* UART Over Sampling Factor */
#define	PL011_CR_UARTEN		(1 <<  0)	/* UART enable */


/*
 * IMSC register bits
 */
#define	PL011_IMSC_TXFEIM	(1 << 12)	/* Transmit FIFO Empty interrupt */
#define	PL011_IMSC_OEIM		(1 << 10)	/* Overrun error interrupt mask */
#define	PL011_IMSC_RTIM		(1 <<  6)	/* Receive timeout interrupt mask */
#define	PL011_IMSC_TXIM		(1 <<  5)	/* Transmit interrupt mask */
#define	PL011_IMSC_RXIM		(1 <<  4)	/* Receive interrupt mask */
#define PL011_IMSC_CTSRM	(1 <<  1)	/*Clear To Send Modem Masked Interrupt Status */


/*
 * RIS register bits
 */
#define	PL011_RIS_TXRIS		(1 <<  5)	/* Transmit interrupt */
#define	PL011_RIS_RXRIS		(1 <<  4)	/* Receive interrupt */
#define	PL011_RIS_DSRRMIS	(1 <<  3)	/* Modem interrupt */
#define	PL011_RIS_DCDRMIS	(1 <<  2)	/* Modem interrupt */
#define	PL011_RIS_CTSRMIS	(1 <<  1)	/* Modem interrupt */
#define	PL011_RIS_RIRMIS	(1 <<  0)	/* Modem interrupt */


/*
 * MIS register bits
 */
#define	PL011_MIS_TXFES		(1 << 12)	/* Transmit FIFO Empty interrupt */
#define	PL011_MIS_RTMIS		(1 << 6)	/* Receive timeout masked interrupt status */
#define	PL011_MIS_TXMIS		(1 << 5)	/* Transmit masked interrupt status */
#define	PL011_MIS_RXMIS		(1 << 4)	/* Receive masked interrupt status */
#define PL011_MIS_CTSRMIS	(1 << 1)	/*Clear To Send Modem Interrupt Status */


/*
 * ICR register bits
 */
#define	PL011_ICR_RTIC		(1 << 6)	/* Receive timeout interrupt clear */
#define	PL011_ICR_TXIC		(1 << 5)	/* Transmit interrupt clear */
#define	PL011_ICR_RXIC		(1 << 4)	/* Receive interrupt clear */
#define	PL011_ICR_CTSMIC	(1 << 1)	/* Modem interrupt clear */

/*
 * DMACR register bits
 */
#define	PL011_DMACR_ONERR	(1 << 2)	/* Receive DMA on error */
#define	PL011_DMACR_TXE		(1 << 1)	/* Transmit DMA Enable */
#define	PL011_DMACR_RXE		(1 << 0)	/* Receive DMA Enable */

typedef void (*procedure_reset)(uintptr_t a[]);
typedef void (*procedure_enable)(uintptr_t a[], int fcr);
typedef int (*supplier)(uintptr_t a[]);
typedef void (*consumer)(uintptr_t a[],  int data);

typedef struct ro_register_struct {
    supplier read;
} ro_register;

typedef struct rw_register_struct {
    supplier read;
    consumer write;
} rw_register;

typedef struct pl01x {
    rw_register data;
    rw_register receive_status;
    ro_register flag;
    rw_register ILPR;
    rw_register integer_baud_rate;
    rw_register fractional_baud_rate;
    rw_register line_control;
    rw_register control;
    rw_register interrupt_FIFO_level_select;
    rw_register interrupt_mask;
    rw_register raw_interrupt_status;
    ro_register masked_interrupt_status;
    rw_register interrupt_clear;
    rw_register DMA_control;
    procedure_reset reset;
    procedure_enable enable;
} PL01X;

extern PL01X pl01x_device;


#endif		/* __ARM_PL011_INC__ */

/* __SRCVERSION("pl011.h $Rev: 716263 $"); */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/arm/pl011.h $ $Rev: 716263 $")
#endif
