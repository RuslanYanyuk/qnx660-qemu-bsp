/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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

#include "startup.h"

const struct debug_device debug_devices[] =
        {
                {
                        "PL011",
                        {
                                "0x9000000^2.115200.10000000"	// Base address, register size (^2 means 32 bit), baud rate, clock
                        },
                        init_pl011,
                        put_pl011,
                        {
                            &display_char_pl011,
                            &poll_key_pl011,
                            &break_detect_pl011
                        }
                }
        };

int ddr_size = 0;

void init_raminfo_qemu() {
    add_ram(0x40000000, 0x80000000);
}

/*
 * main()
 *	Startup program executing out of RAM
 *
 * 1. It gathers information about the system and places it in a structure
 *	called the system page. The kernel references this structure to
 *	determine everything it needs to know about the system. This structure
 *	is also available to user programs (read only if protection is on)
 *	via _syspage->.
 *
 * 2. It (optionally) turns on the MMU and starts the next program
 *	in the image file system.
 */
int main(int argc, char **argv, char **envv)
{
    select_debug(debug_devices, sizeof(debug_devices));


    //debug_flag = 4;
    cpu_freq = 1000000;

	/* add reboot callout */
	//add_callout_array(callouts, sizeof(callouts));

	/*
	 * Collect information on all free RAM in the system
	 */
    lsp.syspage.p->num_cpu = 1;

    init_hwinfo();

	init_raminfo_qemu();

    init_mmu();

    init_intrinfo();

    init_qtime_v7gt(6144000, 27);

    init_cpuinfo();

    init_nanospin();

    add_typed_string(_CS_MACHINE, "QEMU-VIRT");
    add_typed_string(_CS_HOSTNAME, "QNX660");

    /*
	 * Load bootstrap executables in the image file system and Initialise
	 * various syspage pointers. This must be the _last_ initialisation done
	 * before transferring control to the next program.
	 */
    init_system_private();

    /*
	 * This is handy for debugging a new version of the startup program.
	 * Commenting this line out will save a great deal of code.
	 */
    //print_syspage();

    return 0;
}
