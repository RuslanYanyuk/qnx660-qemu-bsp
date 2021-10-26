//
// Created by ruslan on 13.10.21.
//

#include "public/arm/devc-primecell.h"

#define read_write_data(_register, _supplier, _consumer, _mask) \
int read_##_register(uintptr_t port[]) {                        \
    return _supplier(port[_register]) & _mask;                  \
}                                                               \
                                                                \
void write_##_register(uintptr_t port[], int data) {            \
    _consumer(port[_register], data & _mask);                   \
}

#define read_data(_register, _supplier, _mask)                  \
int read_##_register(uintptr_t port[]) {                        \
    return _supplier(port[_register]) & _mask;                  \
}

read_write_data(PL011_DR, in8, out8, 0xff)
read_write_data(PL011_RSR, in8, out8, 0xf)
read_data(PL011_FR, in16, 0x1ff)
read_write_data(PL011_ILPR, in8, out8, 0xff)
read_write_data(PL011_IBRD, in16, out16, 0xffff)
read_write_data(PL011_FBRD, in8, out8, 0x3f)
read_write_data(PL011_LCR_H, in8, out8, 0xff)
read_write_data(PL011_CR, in16, out16, 0xffff)
read_write_data(PL011_IFLS, in8, out8, 0x3f)
read_write_data(PL011_IMSC, in16, out16, 0x7ff)
read_data(PL011_RIS, in16, 0x7ff)
read_data(PL011_MIS, in16, 0x7ff)
read_write_data(PL011_DMACR, in8, out8, 0x7)
read_write_data(PL011_ICR, in16, out16, 0x7ff)

#define read_write_register(_register) { \
    read_##_register,                    \
    write_##_register\
}                                        \

#define read_only_register(_register) { \
    read_##_register,                    \
}

void reset(uintptr_t port[]) {
    pl01x_device.interrupt_mask.write(port, 0);
    pl01x_device.DMA_control.write(port, 0);
    pl01x_device.line_control.write(port, 0);
    pl01x_device.control.write(port, 0);
    pl01x_device.data.write(port, 0);
    pl01x_device.receive_status.write(port, 0);
    pl01x_device.ILPR.write(port, 0);
    pl01x_device.integer_baud_rate.write(port, 0);
    pl01x_device.fractional_baud_rate.write(port, 0);
}

void enable(uintptr_t port[], int fifo_level) {
    if (fifo_level != 0) {
        pl01x_device.interrupt_FIFO_level_select.write(port, fifo_level);
    }
    pl01x_device.line_control.write(port, PL011_LCR_H_FEN);
    pl01x_device.control.write(port, PL011_CR_RXE | PL011_CR_TXE | PL011_CR_DTR);
    pl01x_device.interrupt_mask.write(port, PL011_IMSC_RXIM | PL011_IMSC_CTSRM);
}

PL01X pl01x_device = {
    read_write_register(PL011_DR),
    read_write_register(PL011_RSR),
    read_only_register(PL011_FR),
    read_write_register(PL011_ILPR),
    read_write_register(PL011_IBRD),
    read_write_register(PL011_FBRD),
    read_write_register(PL011_LCR_H),
    read_write_register(PL011_CR),
    read_write_register(PL011_IFLS),
    read_write_register(PL011_IMSC),
    read_only_register(PL011_RIS),
    read_only_register(PL011_MIS),
    read_write_register(PL011_DMACR),
    read_write_register(PL011_ICR),
    reset,
    enable
};
