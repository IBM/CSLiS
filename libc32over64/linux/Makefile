#
# Copyright 2022 - IBM Inc. All rights reserved
# SPDX-License-Identifier: LGPL-2.1
# 
include .config_mk

PRELDOBJS = getpmsg.o getmsg.o putpmsg.o putmsg.o \
	  fattach.o fdetach.o isastream.o # mknod.o poll.o

LIBOBJS = $(PRELDOBJS) pipe.o

CP_OPTS	= -f
XOPTS  += -D__KERNEL_SYSCALLS__
ifeq ($(ARCH),s390x)
XOPTS  += -DBLD32OVER64 -m31
LDOPTS = -melf_s390
endif
ifeq ($(ARCH),ppc64)
XOPTS  = -D__KERNEL_SYSCALLS__
XOPTS  += -DBLD32OVER64 -m32 -msoft-float -pipe -ffixed-r2 -mmultiple -mstring
XOPTS  += -Wall -Wstrict-prototypes $(PROBLEMS)
XOPTS  += -fno-strict-aliasing -Wno-sign-compare -fno-common
XOPTS  += -D__powerpc__ -fno-builtin -pipe -D_PPC_LIS_
XOPTS  += -D__SMP__
XOPTS  += -I$(LIS_INCL)
XOPTS  += -I$(KINCL) $(KINCL_MACH_GENERIC) $(KINCL_MACH_DEFAULT)
LDOPTS = -m elf32ppc
endif
ifeq ($(ARCH),x86_64)
XOPTS  = -D__KERNEL_SYSCALLS__
XOPTS  += -DBLD32OVER64 -m32 -O2 -fomit-frame-pointer
XOPTS  += -D__SMP__
XOPTS  += -I$(LIS_INCL)
XOPTS  += -I$(KINCL) $(KINCL_MACH_GENERIC) $(KINCL_MACH_DEFAULT)
LDOPTS = -m elf_i386
endif

include $(LIBDIR32OVER64)/mk.bdy


#
# Dependency stuff
#

-include .depend

dep:
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(CC) -M -DDEP $(LIBOBJS:%.o=$(LIBDIR)/%.c) >.depend
