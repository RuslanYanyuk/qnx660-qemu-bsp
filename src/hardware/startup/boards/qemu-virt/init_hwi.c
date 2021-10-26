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


#include <startup.h>
#include "hwinfo_private.h"
#include <drvr/hwinfo.h>

#define QEMU_VIRT_PCI_BASE 0x10000000
#define QEMU_VIRT_PCI_REG_SIZE 0x2eff0000

void init_hwinfo()
{
    hwiattr_pci_t pci_attr = HWIATTR_PCI_T_INITIALIZER;

    //HWIATTR_I2C_SET_NUM_IRQ(&pci_attr, 36);

    HWIATTR_PCI_SET_LOCATION(&pci_attr, QEMU_VIRT_PCI_BASE, QEMU_VIRT_PCI_REG_SIZE, 0, hwi_find_as(QEMU_VIRT_PCI_BASE, 1));
    unsigned hwi_off = hwibus_add_pci(HWI_NULL_OFF, &pci_attr);
    ASSERT(hwi_find_unit(hwi_off) == 0);
    //hwitag_set_inputclk(hwi_off, 0, &clksrc);
    hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_PCI, 0), -1, 36);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/startup/boards/dra72x/evm/hwi_dra72x.c $ $Rev: 766791 $")
#endif
