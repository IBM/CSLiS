#
# Copyright 2022 - IBM Inc. All rights reserved
# SPDX-License-Identifier: LGPL-2.1
#
include .config_mk

PRELDOBJS = getpmsg.o getmsg.o putpmsg.o putmsg.o \
	  fattach.o fdetach.o isastream.o # mknod.o poll.o

LIBOBJS = $(PRELDOBJS) pipe.o

CP_OPTS	= -f
CCUSEROPT  += -D__KERNEL_SYSCALLS__

ifeq ($(ARCH), s390x)
#
# For 64-bit zSeries, specify use of position independent code
CCUSEROPT  += -fPIC
endif
# For 64-bit ppc64, specify use of position independent code
ifeq ($(ARCH), ppc64le)
CCUSEROPT  += -fPIC -m64 
endif
# For 64-bit x86_64, specify use of position independent code
ifeq ($(ARCH), x86_64)
ifneq ($(LINUX_TYPE),RPM)	
#  No -fPIC on the Ubuntu x86_64 arch	
CCUSEROPT  += -m64 
else
CCUSEROPT  += -m64 -fPIC
endif
endif



include $(LIBDIR)/mk.bdy


#
# Dependency stuff
#

-include .depend

dep:
	$(Q_ECHO) $(qtag_DEP)$(relpwd)
	$(Q)$(CC) -M -DDEP $(LIBOBJS:%.o=$(LIBDIR)/%.c) >.depend

