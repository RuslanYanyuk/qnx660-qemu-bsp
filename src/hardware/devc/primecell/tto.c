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

int
tto(TTYDEV *ttydev, int action, int arg1) {
    TTYBUF *bup = &ttydev->obuf;
    DEV_8250 *dev = (DEV_8250 *) ttydev;
    uintptr_t *port = dev->port;
    unsigned char c;
    int nbytes_tx = 0;
    int tmp = 0;

    switch (action) {
        case TTO_STTY:
            ser_stty(dev);
            return (0);
        case TTO_CTRL:
            if (arg1 & _SERCTL_BRK_CHG) {
                tmp = pl01x_device.line_control.read(port);
                pl01x_device.line_control.write(port, arg1 & _SERCTL_BRK
                                                        ? PL011_LCR_H_BRK & tmp : tmp & (PL011_LCR_H_BRK ^ 1));
            }
            if (arg1 & _SERCTL_DTR_CHG) {
                tmp = pl01x_device.control.read(port);
                pl01x_device.control.write(port, arg1 & _SERCTL_DTR ? PL011_CR_DTR : tmp & (PL011_CR_DTR ^ 1));
            }
            if (arg1 & _SERCTL_RTS_CHG) {
                tmp = pl01x_device.control.read(port);
                pl01x_device.control.write(port, arg1 & _SERCTL_RTS ? PL011_CR_RTS : tmp & (PL011_CR_RTS ^ 1));
            }
            return (0);
        case TTO_LINESTATUS:
            return ((pl01x_device.flag.read(port) << 8) | pl01x_device.control.read(port)) & 0xf003;
        case TTO_DATA:
        case TTO_EVENT:
            break;
        default:
            return (0);
    }



    /* With FIFOs enable LSR_TXRDY is set when the last byte is moved from
     * the TX fifo to the TX shift register (TX FIFO empty). LSR_TXRDY is cleared
     * when a byte is added to the TX FIFO. Since there is no TX trigger level
     * for 16550 style UARTs, we only ever write when the FIFO is empty to ensure
     * we do not overrun the TX fifo.
     * NOTE: Do not confuse the LSR_TXDRY with the II_TX which is cleared on read
     *       of the II register and/or when a byte is written to the TX Fifo
     */
    if (pl01x_device.flag.read(port) & PL011_FR_TXFE) {
        nbytes_tx = 0;
        while (bup->cnt > 0 && nbytes_tx < dev->tx_fifo) {
            /*
             * If the OSW_PAGED_OVERRIDE flag is set then allow
             * transmit of character even if output is suspended via
             * the OSW_PAGED flag. This flag implies that the next
             * character in the obuf is a software flow control
             * charater (STOP/START).
             * Note: tx_inject sets it up so that the contol
             *       character is at the start (tail) of the buffer.
             *
             */
            if (dev->tty.flags & (OHW_PAGED | OSW_PAGED) && !(dev->tty.xflags & OSW_PAGED_OVERRIDE))
                break;

            /* Get character from obuf and do any output processing */
            dev_lock(&dev->tty);
            c = tto_getchar(&dev->tty);
            dev_unlock(&dev->tty);

            // Print the character
            dev->tty.un.s.tx_tmr = 3;  /* Timeout */
            pl01x_device.data.write(port, c);
            nbytes_tx++;
            /* Clear the OSW_PAGED_OVERRIDE flag as we only want
             * one character to be transmitted in this case.
             */
            if (dev->tty.xflags & OSW_PAGED_OVERRIDE) {
                atomic_clr(&dev->tty.xflags, OSW_PAGED_OVERRIDE);
                break;
            }
        }
    }

    /* Check the client lists for notify conditions */
    return (tto_checkclients(&dev->tty));
}

void
ser_stty(DEV_8250 *dev) {
    unsigned lcr = 0;
    uintptr_t *port = dev->port;
    unsigned value;

    // Set Baud rate
    value = (dev->tty.baud == 0) ? 0 : (dev->clk / (dev->tty.baud * dev->div));
    dev_lock(&dev->tty);
    pl01x_device.integer_baud_rate.write(port, value);
    dev_unlock(&dev->tty);

    if (dev->tty.c_cflag & CREAD && !pl01x_device.masked_interrupt_status.read(port)) {
        pl01x_device.enable(port, 0);
    } else if (!(dev->tty.c_cflag & CREAD) && pl01x_device.masked_interrupt_status.read(port)) {
        /* Disable receiver */
        pl01x_device.reset(port);
    }

    // Set data bits
    switch (dev->tty.c_cflag & CSIZE) {
        case CS8:
            lcr |= PL011_LCR_H_WLEN8;
        case CS7:
            lcr |= PL011_LCR_H_WLEN7;
        case CS6:
            lcr |= PL011_LCR_H_WLEN6;
    }

    // Set stop bits
    if (dev->tty.c_cflag & CSTOPB)
        lcr |= PL011_LCR_H_STP2;

    // Set parity bits
    if (dev->tty.c_cflag & PARENB) {
        lcr |= PL011_LCR_H_PEN;
        if (dev->tty.c_cflag & PARODD)
            lcr &= ~(PL011_LCR_H_EPS);     /* Clear even bit if odd parity */
        else
            lcr |= PL011_LCR_H_EPS;
        if (dev->tty.c_cflag & PARSTK)    /* Check if Mark/Space parity enabled */
            lcr |= PL011_LCR_H_SPS;
        else
            lcr &= ~(PL011_LCR_H_SPS);
    } else
        lcr &= ~(PL011_LCR_H_PEN);

    pl01x_device.line_control.write(port, lcr);
}

int drain_check(TTYDEV *ttydev, uintptr_t *count) {
    TTYBUF *bup = &ttydev->obuf;
    DEV_8250 *dev = (DEV_8250 *) ttydev;
    uintptr_t *port = dev->port;

    // if the device has DRAINED, return 1
    if ((bup->cnt == 0) && (pl01x_device.flag.read(port) & PL011_FR_TXFE)) return 1;

    // if the device has not DRAINED, set a timer based on 50ms counts
    // wait for the time it takes for one character to be transmitted
    // out the shift register.  We do this dynamically since the
    // baud rate can change.
    if (count != NULL)
        *count = (ttydev->baud == 0) ? 0 : ((IO_CHAR_DEFAULT_BITSIZE * 20) / ttydev->baud) + 1;
    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/ser8250/tto.c $ $Rev: 680332 $")
#endif
