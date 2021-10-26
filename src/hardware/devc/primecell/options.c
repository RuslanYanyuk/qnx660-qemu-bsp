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


/*
#ifdef __USAGE
%C - Serial driver for 8250's

%C [options] [port[^shift][,irq]] &
Options:
 -b number    Define initial baud rate (default 57600)
 -c clk[/div] Set the input clock rate and divisor
 -C number    Size of canonical input buffer (default 256)
 -e           Set options to "edit" mode
 -E           Set options to "raw" mode (default)
 -I number    Size of raw input buffer (default 2048)
 -f           Enable hardware flow control (default)
 -F           Disable hardware flow control
 -O number    Size of output buffer (default 2048)
 -s           Enable software flow control
 -S           Disable software flow control (default)
 -t number    Set receive FIFO trigger level (default 8)
 -T number    Set transmit FIFO size (default 14)
 -u unit      Set serial unit number (default 1)
 -o opt[,opt] string options:
              pmm_parent=path (pmm_parent power pathname, default NULL)
              pm_noflw (Disable flow control on power down, default enabled)
              highwater=value (RX watermark for input flow control (bytes))
              disable=rx (Disable receiver on startup)
 -v           Increase verbosity

Example:
devc-primecell -b115200 -F -ohighwater=1500,disable=rx
#endif
*/
#include "public/arm/devc-primecell.h"
#include "hw/sysinfo.h"
#include "drvr/hwinfo.h"

unsigned
options(int argc, char *argv[]) {
	int opt, i, enabled_cnt = 0, numports = 0;
	char *cp;
	void *link;
	unsigned unit;
	unsigned fifo_tx, fifo_rx;
   DEV_8250 *dev_list[MAX_DEVICES];
	static TTYINIT devinit ={
		0, 0, 0, 115200,
		2048, 2048, 256,
		0, 0, 0, 0, 0xe8, 0, 0,       /* Default Tx fifo 14, Rx fifo 8 */
		"/dev/ser",
      NULL, 0, 0, 0
  };

	// Initialize the devinit to raw mode
	ttc(TTC_INIT_RAW, &devinit, 0);

	sys_ttyinit(&devinit);
	unit = 1;
	/* Getting the UART Clock from the Hwinfo Section if available */
	{
		unsigned hwi_off = hwi_find_device("uart", 0);
		if(hwi_off != HWI_NULL_OFF){
			hwi_tag *tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_inputclk, 0);
			if(tag){
				devinit.clk = ((tag->inputclk.clk) / (tag->inputclk.div));
			}
		}
	}
	while(optind < argc) {
		// Process dash options.
		while((opt = getopt(argc, argv, IO_CHAR_SERIAL_OPTIONS "c:t:T:u:")) != -1) {	
			switch(ttc(TTC_SET_OPTION, &devinit, opt)) {
			case 'c':
				devinit.clk = strtoul(optarg, &optarg, 0);
				if((cp = strchr(optarg, '/'))) {
					devinit.div = strtoul(cp + 1, NULL, 0);
				}
				break;
			case 't':
				fifo_rx = strtoul(optarg, NULL, 0);
#if defined(VARIANT_xingu8)
				/* Default Tx fifo 14, Rx fifo 16 */
				if ( !((fifo_rx == 1) || (fifo_rx == 8) || (fifo_rx == 16) || (fifo_rx == 32)) )
				{
					fprintf(stderr,"Illegal rx fifo trigger. \n");
					fprintf(stderr,"Trigger number must be 1, 8, 16 or 32. \n");
					fprintf(stderr,"Rx trigger will not be enabled.\n\n");
					fifo_rx = 0;
				}
				else
				{
					/* Map level to register mask */
					switch (fifo_rx)
					{
						case 1:
							fifo_rx = 1;		// map REG_FC to 0x00
							break;
						case 8:
							fifo_rx = 4;		// map REG_FC to 0x40
							break;
						case 16:
							fifo_rx = 8;		// map REG_FC to 0x80
							break;
						case 32:
							fifo_rx = 14;		// map REG_FC to 0xc0
							break;
						default:
							fifo_rx = 0;		// map REG_FC to 0x00
							break;
					}
					devinit.fifo &= 0xf0;
					devinit.fifo |= fifo_rx;
				}
#else
				if( !((fifo_rx == 1) || (fifo_rx == 4) || (fifo_rx == 8) || (fifo_rx == 14)) ) {
					fprintf(stderr,"Illegal rx fifo trigger. \n");
					fprintf(stderr,"Trigger number must be 1, 4, 8 or 14. \n");
					fprintf(stderr,"Rx trigger will not be enabled.\n\n");
					fifo_rx = 0;
				}
            else
            {
               devinit.fifo &= 0xf0;
               devinit.fifo |= fifo_rx;
            }
#endif
				break;
			case 'T':
				fifo_tx = strtoul(optarg, NULL, 0);
				if( !((fifo_tx == 1) || (fifo_tx == 4) || (fifo_tx == 8) || (fifo_tx == 14)) ) {
					fprintf(stderr,"Illegal tx fifo size. \n");
					fprintf(stderr,"Tx fifo size must be 1, 4, 8 or 14. \n");
					fprintf(stderr,"Tx fifo will not be enabled.\n\n");
					fifo_tx = 0;
				}
            else
            {
               devinit.fifo &= 0x0f;
               devinit.fifo |= (fifo_tx << 4);
               
            }
                break;
			case 'u':
				unit = strtoul(optarg, NULL, 0);
				break;
			}
		}

		// Process ports and interrupts.
		while(optind < argc  &&  *(optarg = argv[optind]) != '-') {

			devinit.port = strtoull(optarg, &optarg, 16);
			if(*optarg == '^') {
				devinit.port_shift = strtoul(optarg + 1, &optarg, 0);
			}
			if(*optarg == ',') {
				devinit.intr = strtoul(optarg + 1, NULL, 0);
			}
			if ((dev_list[numports] = create_device(&devinit, unit++)) == NULL)
         {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Initialization of /dev/ser%d (port 0x%llx) failed", unit - 1, devinit.port);
            fprintf(stderr, "io-char: Initialization of port 0x%llx failed\n", devinit.port);
         }
         else
   			++numports;
			++optind;
		}
	}
	if(numports == 0) {
		link = NULL;
		for( ;; ) {
			link = query_default_device(&devinit, link);
			if(link == NULL) break;
			if ((dev_list[numports] = create_device(&devinit, unit++)) == NULL)
         {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Initialization of /dev/ser%d (port 0x%llx) failed", unit - 1, devinit.port);
            fprintf(stderr, "io-char: Initialization of port 0x%llx failed\n", devinit.port);
         }
         else
   			++numports;
		}
	}
   /* Enable all ports (Attach ISR and pathname entry) */ 
   for (i=0; i<numports; i++)
   {
/*      if(dev_list[i]->tty.verbose)
      {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Port .......................... %s (0x%x)", dev_list[i]->tty.name, dev_list[i]->port[0]);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "IRQ ........................... 0x%x", dev_list[i]->intr);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Input Clock ................... %d", dev_list[i]->clk);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Clock Divisor ................. %d", dev_list[i]->div);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Tx fifo size .................. %d", (dev_list[i]->tty.fifo >> 4));
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Rx fifo trigger ............... %d", (dev_list[i]->tty.fifo & 0x0f));
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Input buffer size ............. %d", dev_list[i]->tty.ibuf.size);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Input flow control highwater .. %d", dev_list[i]->tty.highwater);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Output buffer size ............ %d", dev_list[i]->tty.obuf.size);
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Canonical buffer size ......... %d\n", dev_list[i]->tty.cbuf.size);
      }*/
      if (enable_device(dev_list[i]) != -1)
         enabled_cnt++;
   }
   return(enabled_cnt);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/ser8250/options.c $ $Rev: 680332 $")
#endif
