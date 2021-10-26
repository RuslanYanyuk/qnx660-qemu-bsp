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
#include <stdio.h>

TTYCTRL                  ttyctrl;
struct dev_list			 *devices;

int
main(int argc, char *argv[]) {

	ttyctrl.max_devs = MAX_DEVICES;
	ttc(TTC_INIT_PROC, &ttyctrl, 24);

    if(options(argc, argv) == 0) {
		fprintf(stderr, "%s: No serial ports found\n", argv[0]);
		exit(0);
    }

	ttc(TTC_INIT_START, &ttyctrl, 0);
	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/ser8250/main.c $ $Rev: 680332 $")
#endif
