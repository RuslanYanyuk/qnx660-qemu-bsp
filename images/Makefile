ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

HOST_MKIFS := mkifs

SUFFIXES := .build .ifs .raw

.PHONY: all clean

all: ifs-qemu-virt-arm-a15.bin 

clean: 
	$(RM_HOST) ifs-qemu-virt-arm-a15.bin
	$(RM_HOST) a15.build

ifs-qemu-virt-arm-a15.bin:
	$(HOST_MKIFS) a15.build -r../install -v $(MKIFSFLAGS) $^ $@
