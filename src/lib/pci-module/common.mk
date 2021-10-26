# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION=PCIE hardware module
endef

NAME=pci_hw-qemu-virt

USEFILE=

PSTAG_64 = .64

INSTALLDIR=lib/dll/pci

CCFLAG_64 = -D_PADDR_BITS=64
CCFLAGS_gcc_ = -shared
CCFLAGS_gcc_qcc = -shared -Wl

include $(MKFILES_ROOT)/qmacros.mk
include $(MKFILES_ROOT)/qtargets.mk

#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../install
   USE_INSTALL_ROOT=1
##############################################################
