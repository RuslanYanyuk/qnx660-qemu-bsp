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

#ifdef DEFN
	#define EXT
	#define INIT1(a)				= { a }
#else
	#define EXT extern
	#define INIT1(a)
#endif

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <termios.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <sys/iomsg.h>
#include <atomic.h>
#include <hw/inout.h>
#include <ioctl.h>
#include "public/arm/io-char.h"
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include "pl01x-qemu.h"

#define _OBAND_SER_MS			0x20

#if defined(VARIANT_pa6t) || defined(VARIANT_tegra2)
	// enable workarounds for bugs in the UARTS of early PA6T-1682's
	/* Note: The implementation fo the 8250 on the nVIDIA Tegra2
             behave the same way as the workaround for the PA6T.
             We will reuse the same code unless there is some
             difference between both implementation.
    */
	#define PA6T_WORKAROUND
#endif

#if defined(VARIANT_mpc8540)
	#define MPC850_WORKAROUND
#endif

#if defined(VARIANT_jace5)
	#define TL16C752B_WORKAROUND
#endif

#ifndef DEV_8250			/* Can be defined in variant.h to override default */
typedef struct dev_8250 {
	TTYDEV			tty;
	struct dev_8250	*next;
	unsigned		intr;
	unsigned		clk;
	unsigned		div;
	unsigned char	rx_fifo; /* rx fifo size */
	unsigned char	tx_fifo; /* tx fifo size */
	unsigned char   fcr;	 /* FCR is write-only register, so keep local copy for read-modify-write */
#if defined(PA6T_WORKAROUND)	
	unsigned char   irr_fiddle;			/* for PA6T-1682 workaround */
	unsigned char	tx_empty_disable;	/* for PA6T-1682 workaround */
#endif
	uintptr_t		port[PL011_TOTAL_REG_NUM];
} DEV_8250;
#endif

struct dev_list {
	struct dev_list	*next;
	DEV_8250		*device;
	int				iid;
};

extern struct dev_list			 *devices;

#define MAX_DEVICES  16
#define FIFO_SIZE 16

DEV_8250 *create_device(TTYINIT *dip, unsigned unit);
void ser_stty(DEV_8250 *dev);
void sys_ttyinit(TTYINIT *dip);
void *query_default_device(TTYINIT *dip, void *link);
int enable_device(DEV_8250 *dev);

const struct sigevent *ser_intr(void *area, int id);

unsigned options(int argc, char *argv[]);


#ifndef IE_SET_ALL
    #define IE_SET_ALL ( IE_RX | IE_TX | IE_LS | IE_MS )
#endif

#ifndef IE_CLR_ALL
    #define IE_CLR_ALL 0x0
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/ser8250/externs.h $ $Rev: 680332 $")
#endif
