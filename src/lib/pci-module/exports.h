/**
 * Author: Ruslan Yaniuk
 * Date: October 26 2021
 */

#include <stdint.h>

#define EXPORTED __attribute__((__visibility__("default")))

extern EXPORTED int hwmod_init(uintptr_t *arg);

typedef struct {
    uintptr_t self;
    uintptr_t mod_version;
    uintptr_t mod_compat;
    uintptr_t cfg_rd;
    uintptr_t cfg_wr;
    uintptr_t hw_alloc_irq;
    uintptr_t hw_free_irq;
    uintptr_t hw_alloc_as;
    uintptr_t hw_resv_as;
    uintptr_t hw_free_as;
    uintptr_t hw_map_as;
    uintptr_t hw_add_device_hold_off;
    uintptr_t hw_remove_device_hold_off;
    uintptr_t hw_initiate_hold_off;
    uintptr_t hw_release_hold_off;
    uintptr_t extcfg_find_csd_assignment;
    uintptr_t hw_reset;
    uintptr_t extcfg_check_rbar_override;
} HW_MOD_API;

extern EXPORTED HW_MOD_API hwmod_access;
