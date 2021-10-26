/**
 * Author: Ruslan Yaniuk
 * Date: October 26 2021
 */

#include "exports.h"
#include <hw/inout.h>
#include <sys/rsrcdbmgr.h>
#include <sys/rsrcdbmsg.h>
#include <sys/slog.h>
#include <pci/pci.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/neutrino.h>

#define QEMU_VIRT_PCI_ECAM_BASE 0x3f000000
#define QEMU_VIRT_PCI_REG_SIZE 0xFFFFFF

uintptr_t get_config_address_mapped(pci_bdf_t bdf, int size_in_bytes, unsigned register_offset) {
    unsigned bus = _PCI_BDF_BUS(bdf);
    unsigned device = _PCI_BDF_DEV(bdf);
    unsigned function = _PCI_BDF_FUNC(bdf);
    unsigned physical_offset = ((( bus * 256 ) + ( device * 8 ) + function ) * 4096)
            + QEMU_VIRT_PCI_ECAM_BASE + register_offset;

    ThreadCtl(_NTO_TCTL_IO, 0);
    return mmap_device_io(size_in_bytes, physical_offset);
}

int mod_version() {
    return -1;
}

int mod_compat() {
    return 1;
}

int config_read(pci_bdf_t bdf, unsigned register_offset, int size_in_bytes, unsigned *config_store, int a5) {
    slogf(17, _SLOG_INFO, "TRACE::: config_read %x %x %x %x", bdf, register_offset, size_in_bytes, config_store);
    uintptr_t mapped = get_config_address_mapped(bdf, size_in_bytes, register_offset);

    switch (size_in_bytes) {
        case 1: out8(config_store, in8(mapped)); break;
        case 2: out16(config_store, in16(mapped)); break;
        case 4: out32(config_store, in32(mapped)); break;
        default: return -1;
    }
    return 0;
}

int config_write(pci_bdf_t bdf, unsigned register_offset, int size_in_bytes, unsigned *config_store, int a5) {
    slogf(17, _SLOG_INFO, "TRACE::: config_read %x %x %x %x", bdf, register_offset, size_in_bytes, config_store);
    uintptr_t mapped = get_config_address_mapped(bdf, size_in_bytes, register_offset);

    switch (size_in_bytes) {
        case 1: out8(mapped, *config_store); break;
        case 2: out16(mapped, *config_store); break;
        case 4: out32(mapped, *config_store); break;
        default: return -1;
    }
    return 0;
}

int allocate_irq(pci_bdf_t bdf, int a2, int cap, unsigned *irq, unsigned a5[]) {
    register int lr asm("lr");
    slogf(17, _SLOG_INFO, "TRACE::: allocate_irq %u %u %u %x %x %x", bdf, a2, cap, irq, a5, lr);
    return 0;
}

int free_irq(int a1, int a2, int a3, int a4) {
    slogf(17, _SLOG_INFO, "TRACE::: free_irq %u %u %u %u", a1, a2, a3, a4);
    return 0;
}

int allocate_as(int a1, int a2, int a3, int a4, int a5, int a6) {
    register int lr asm("lr");
    slogf(17, _SLOG_INFO, "TRACE::: allocate_as %u %u %u %x %u %u %x", a1, a2, a3, a4, a5, a6, lr);
    return 0;
}

int reserve_as(int a1, int a2) {
    slogf(17, _SLOG_INFO, "TRACE::: reserve_as %u %u ", a1, a2);
    return 0;
}

int free_as(int a1, int a2) {
    slogf(17, _SLOG_INFO, "TRACE::: free_as %u %u ", a1, a2);
    return 0;
}

int map_as(pci_bdf_t bdf, const pci_ba_t * const as, pci_ba_t *as_xlate) {
    slogf(17, _SLOG_INFO, "TRACE::: map_as %u", bdf);
    as_xlate->addr = as->addr;
    as_xlate->size = as->size;
    as_xlate->type = as->type;
    as_xlate->attr = as->attr;
    as_xlate->bar_num = as->bar_num;
    return 0;
}

int add_device_hold_off(int a1) {
    slogf(17, _SLOG_INFO, "TRACE::: add_device_hold_off %u", a1);
    return 0;
}

int remove_device_hold_off(int a1) {
    slogf(17, _SLOG_INFO, "TRACE::: remove_device_hold_off %u", a1);
    return 0;
}

int initiate_hold_off(int a1, int a2, int a3, int a4) {
    slogf(17, _SLOG_INFO, "TRACE::: initiate_hold_off %u %u %u %u", a1, a2, a3, a4);
    return 0;
}

int release_hold_off() {
    slogf(17, _SLOG_INFO, "TRACE::: release_hold_off");
    return 0;
}

// CSD - chasi/slot/device. chasi is always 0 so is not encoded
int extcfg_find_csd_assignment(int previous_sd, int vid, int did, int a4) {
    slogf(17, _SLOG_INFO, "TRACE::: extcfg_find_csd_assignment %u %u %u %u", previous_sd, vid, did, a4);

    // if previous_sd = 0x00020001 then
    // 0x0002 is a device index (starts with 0)
    // 0x0001 is a slot number (starts with 1)
    if (previous_sd == -1) {
        return 1;
    } else if (previous_sd == 1) {
        return 0x10002;
    } else if (previous_sd == 0x10002) {
        return 0x20003;
    }
    return -1;
}

int reset(int a1, int a2, int a3) {
    slogf(17, _SLOG_INFO, "TRACE::: reset %u %u %u", a1, a2, a3);
    return 0;
}

int extcfg_check_rbar_override(int a1, int a2, int *a3) {
    slogf(17, _SLOG_INFO, "TRACE::: extcfg_check_rbar_override %u %u %u", a1, a2, *a3);
    return 0;
}

uintptr_t p = &hwmod_access;

HW_MOD_API hwmod_access = {
        &p,
        &mod_version,
        &mod_compat,
        &config_read,
        &config_write,
        &allocate_irq,
        &free_irq,
        &allocate_as,
        &reserve_as,
        &free_as,
        &map_as,
        &add_device_hold_off,
        &remove_device_hold_off,
        &initiate_hold_off,
        &release_hold_off,
        &extcfg_find_csd_assignment,
        &reset,
        &extcfg_check_rbar_override
};

int hwmod_init(uintptr_t *arg) {
    slogf(17, _SLOG_INFO, "TRACE::: **********PCI module is loaded***********");
    const rsrc_alloc_t rsrs[] = {
    {
            .start = QEMU_VIRT_PCI_ECAM_BASE,
            .end = QEMU_VIRT_PCI_ECAM_BASE + QEMU_VIRT_PCI_REG_SIZE - 1,
            .flags = RSRCDBMGR_PCI_MEMORY
    }};

    rsrcdbmgr_create((rsrc_alloc_t*)rsrs, 1);
    *arg = p;
	return 0;
}
