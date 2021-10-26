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

#include "public/arm/devc-primecell.h"

/*
 * Process data in a line status register
 */
unsigned
process_lsr(DEV_8250 *dev, unsigned char lsr) {
    unsigned key = 0;

    // Return immediately if no errors.
    if ((lsr & (PL011_RSR_BE | PL011_RSR_FE | PL011_RSR_OE | PL011_RSR_PE)) == 0) {
        return (0);
    }

    // Save the error as out-of-band data which can be retrieved via devctl().
    dev->tty.oband_data |= lsr & 0x0f;
    atomic_set(&dev->tty.flags, OBAND_DATA);

    if (lsr & PL011_RSR_BE) {
        key |= TTI_BREAK;
    } else {
        if (lsr & PL011_RSR_OE)
            key |= TTI_OVERRUN;
        else if (lsr & PL011_RSR_FE)
            key |= TTI_FRAME;
        else if (lsr & PL011_RSR_PE)
            key |= TTI_PARITY;
    }
    return (key);
}

/*
 * Serial interrupt handler
 */
const struct sigevent *
ser_intr(void *area, int id) {
    struct dev_list *list = area;
    int status = 0;
    unsigned char lsr;
    DEV_8250 *dev;
    struct sigevent *event = NULL;
    unsigned c;

    for (dev = list->device; dev != NULL; dev = dev->next) {
        unsigned iir;
        uintptr_t *port = dev->port;
        status = 0;

        iir = pl01x_device.masked_interrupt_status.read(port);

        if (iir & PL011_MIS_RXMIS) {
            /* Some UARTs will generate a LS interrupt when there is an
             * error anywhere in the RX FIFO, and will clear this interrupt
             * only when there are no more errors remaining in the FIFO. The
             * error bits in REG_LS (BI/PR/FE/OE) always represent the error
             * status for the received character at the top of the Rx FIFO.
             * Reading the Rx FIFO updates these bits to the appropriate status
             * of the new character. This means that it is possible to get an
             * LS interrupt with none of the error status bits set, in order
             * to clear the LS interrupt we must read out all of the characters
             * in the FIFO until we find and handle the erronous character.
             */
            while (((lsr = pl01x_device.flag.read(port)) & PL011_FR_RXFE) == 0) {
                c = pl01x_device.data.read(port);
                c |= process_lsr(dev, pl01x_device.receive_status.read(port));
                status |= tti(&dev->tty, c);
            }
        } else if (iir & PL011_MIS_TXMIS) {
            dev->tty.un.s.tx_tmr = 0;
            // Send evnet to io-char, tto() will be processed at thread time
            atomic_set(&dev->tty.flags, EVENT_TTO);
            status |= 1;
        } else if (iir & PL011_MIS_CTSRMIS) {
            if (pl01x_device.flag.read(port) & PL011_FR_DCD)
                status |= tti(&dev->tty, (iir & PL011_RIS_DCDRMIS) ? TTI_CARRIER : TTI_HANGUP);

            if ((iir & PL011_MIS_CTSRMIS) && (dev->tty.c_cflag & OHFLOW))
                status |= tti(&dev->tty, (iir & PL011_MIS_CTSRMIS) ? TTI_OHW_CONT : TTI_OHW_STOP);

            // OBAND notification of Modem status change
            dev->tty.oband_data |= _OBAND_SER_MS;
            atomic_set(&dev->tty.flags, OBAND_DATA);
            atomic_set(&dev->tty.flags, EVENT_NOTIFY_OBAND);
            status |= 1;
        } else {
            // Not suppoerted IRQ
            pl01x_device.interrupt_clear.write(port, iir);
            continue;
        }

        if (status) {
            if ((dev->tty.flags & EVENT_QUEUED) == 0) {
                event = &ttyctrl.event;
                dev_lock(&ttyctrl);
                ttyctrl.event_queue[ttyctrl.num_events++] = &dev->tty;
                atomic_set(&dev->tty.flags, EVENT_QUEUED);
                dev_unlock(&ttyctrl);
            }
        }
    }
    return (event);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/ser8250/intr.c $ $Rev: 680332 $")
#endif
