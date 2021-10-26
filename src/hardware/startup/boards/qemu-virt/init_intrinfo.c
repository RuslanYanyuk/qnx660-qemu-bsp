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

/*
 * TI DRA72X interrupt controller support.
 */

#include "startup.h"

/*

extern struct callout_rtn interrupt_id_omap4_sdma;
extern struct callout_rtn interrupt_eoi_omap4_sdma;
extern struct callout_rtn interrupt_mask_omap4_sdma;
extern struct callout_rtn interrupt_unmask_omap4_sdma;

extern struct callout_rtn interrupt_id_edma;
extern struct callout_rtn interrupt_eoi_edma;
extern struct callout_rtn interrupt_mask_edma;
extern struct callout_rtn interrupt_unmask_edma;

extern struct callout_rtn interrupt_id_omap4_gpio;
extern struct callout_rtn interrupt_eoi_omap4_gpio;
extern struct callout_rtn interrupt_mask_omap4_gpio;
extern struct callout_rtn interrupt_unmask_omap4_gpio;

#define TBL_SIZE(tbl)		(sizeof(tbl)/sizeof(tbl[0]))

#define GPIO_IRQ_BASE_VECTOR	1000
*/

#define QEMU_GPIO1_BASE 0x09030000;

const int QEMU_NUM_INTERNAL_IRQS = 64;

/*static gpio_info_t		gpio_info_tbl[] = {
	{ .base = (paddr_t)QEMU_GPIO1_BASE, .irq = QEMU_NUM_INTERNAL_IRQS + 1 }
};*/
/*
// Define System DMA module base address
static paddr_t sdma_base = (paddr_t)DRA72X_SDMA_BASE;
static paddr_t edma_base = (paddr_t)(DRA72X_EDMA_BASE + DRA72X_EDMA_SHADOW0_OFF);
*/


// Adding main ARM GIC Controller
const struct startup_intrinfo intrs[] = {
	{
		.vector_base		= _NTO_INTR_CLASS_EXTERNAL,
		.num_vectors		= 256 + 64,	// Include ARM first interrupt + SoC interrupt map.
		.cascade_vector		= _NTO_INTR_SPARE,
		.cpu_intr_base		= 0,
		.cpu_intr_stride	= 0,
		.flags				= 0,
		.id					= { INTR_GENFLAG_LOAD_SYSPAGE, 0, &interrupt_id_gic},
		.eoi				= { INTR_GENFLAG_LOAD_SYSPAGE | INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_gic},
		.mask				= &interrupt_mask_gic,
		.unmask				= &interrupt_unmask_gic,
		.config				= &interrupt_config_gic,
		.patch_data			= NULL,
	}
};

/*

// Adding System DMA interrupt cascaded into
startup_intrinfo sdmaintrs[] = {
	{
		.vector_base		= SDMA_IRQ_BASE_VECTOR,
		.num_vectors		= 32,
		.cascade_vector		= DRA72X_SDMA_IRQ_0,
		.cpu_intr_base		= 0,
		.cpu_intr_stride	= 0,
		.flags				= 0,
		.id					= { 0, 0, &interrupt_id_omap4_sdma },
		.eoi				= { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_omap4_sdma },
		.mask				= &interrupt_mask_omap4_sdma,
		.unmask				= &interrupt_unmask_omap4_sdma,
		.config				= 0,
		.patch_data			= &sdma_base,
	},
};
*/

// GPIO x interrupt structure
/*startup_intrinfo gpiointrs[] = {
	{
		.vector_base		= 7,	// Actually it's based on GPIO_IRQ_BASE_VECTOR
		.num_vectors		= 1,
		.cascade_vector		= 0,	// Actually they are cascaded on GPIOx_IRQ_1
		.cpu_intr_base		= 0,
		.cpu_intr_stride	= 0,
		.flags				= 0,
		.id					= { 0, 0, &interrupt_id_omap4_gpio },
		.eoi				= { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_omap4_gpio },
		.mask				= &interrupt_mask_omap4_gpio,
		.unmask				= &interrupt_unmask_omap4_gpio,
		.config				= 0,
		.patch_data			= NULL,
	},
};*/
/*
// Adding EDMA interrupt
startup_intrinfo edmaintrs[] = {
	{	.vector_base		= EDMA_IRQ_BASE_VECTOR,
		.num_vectors		= 64,
		.cascade_vector		= DRA72X_EDMA_IRQ,
		.cpu_intr_base		= 0,
		.cpu_intr_stride	= 0,
		.flags				= 0,
		.id					= {INTR_GENFLAG_LOAD_SYSPAGE, 0, &interrupt_id_edma},
		.eoi				= {INTR_GENFLAG_LOAD_SYSPAGE | INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_edma},
		.mask				= &interrupt_mask_edma,
		.unmask				= &interrupt_unmask_edma,
		.config				= 0,
		.patch_data			= &edma_base
	},
};
*/

void init_intrinfo()
{
/*	unsigned	cbar;
	unsigned	idx, base;*/
/*

	__asm__ __volatile__("mrc	p15, 4, %0, c15, c0, 0" : "=r"(cbar));
*/

	// Add ARM GIC & SDMA interrupt callouts
	//arm_gic_init(cbar + 0x1000, cbar + 0x2000);

    arm_gic_init(0x8000000, 0x8010000);

	add_interrupt_array(intrs, sizeof(intrs));
/*
	// configure SDMA operation
	base = DRA72X_SDMA_BASE;
	out32(base + DRA72X_DMA4_GCR, in32(base + DRA72X_DMA4_GCR) | (GCR_ARBITRATION_RATE | DMA_DEFAULT_FIFO_DEPTH));

	// Config SDMA interrupt
	out32(base + DRA72X_DMA4_OCP_SYSCONFIG, DMA4_OCP_SYSCONFIG_AUTOIDLE_AUTO
						| DMA4_OCP_SYSCONFIG_MIDLEMODE_SMART
						| DMA4_OCP_SYSCONFIG_SIDLEMODE_SMART);

	*//*
	 * Disable all sDMA logical Channel interrupts and clear all channel statuses
	 * Note: This must be done before clearing the DMA4_IRQSTATUS register.
	 *//*
	for (idx = 0; idx < 32; idx++) {
		out32(base + DRA72X_DMA4_CCRi + (0x60 * idx), 0);
		out32(base + DRA72X_DMA4_CICRi + (0x60 * idx), 0);
		out32(base + DRA72X_DMA4_CSRi + (0x60 * idx), DRA72X_DMA4_CSR_MSK);
	}

	for (idx = 0; idx < 4; idx++, base += 4) {
		out32(base + DRA72X_DMA4_IRQENABLE_Lj, 0);
		out32(base + DRA72X_DMA4_IRQSTATUS_Lj, 0xffffffff);
	}

	// Add interrupts.
	add_interrupt_array(sdmaintrs, sizeof(sdmaintrs));
*/
	/*// Add all GPIO interrupts
	for (idx = 0; idx < TBL_SIZE(gpio_info_tbl); idx++) {
		// Disable & clear all interrupts
		out32((uint32_t)gpio_info_tbl[idx].base + GPIO_IRQSTATUS_CLR_0, 0xffffffff);
		out32((uint32_t)gpio_info_tbl[idx].base + GPIO_IRQSTATUS_0, 0xffffffff);

		// Configure vertor_base, cascade_vector & patch address
		gpiointrs[0].vector_base = GPIO_IRQ_BASE_VECTOR + (idx * 32);
		gpiointrs[0].cascade_vector = gpio_info_tbl[idx].irq;
		gpiointrs[0].patch_data	= &gpio_info_tbl[idx].base;

		// Add interrupt callouts
		add_interrupt_array(gpiointrs, sizeof(gpiointrs));
	}*/
/*
	// Add interrupts.
	add_interrupt_array(edmaintrs, sizeof(edmaintrs));*/
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/startup/boards/dra72x/init_intrinfo.c $ $Rev: 758169 $")
#endif
