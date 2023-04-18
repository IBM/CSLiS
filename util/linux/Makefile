# 
#  Copyright 2022 - IBM Inc. All rights reserved
#  SPDX-License-Identifier: LGPL-2.1
#
include .config_mk

OPT	= -DLINUX
ifeq ($(ARCH),ppc64le)
	XUTILS  = test test2 fattach fdetach oc rwf mkfifo psfd pafd thrtst
else
ifeq ($(ARCH),x86_64)
	XUTILS  = test test2 fattach fdetach oc rwf mkfifo psfd pafd thrtst
else
	XUTILS  = test test2 fattach fdetach oc rwf mkfifo psfd pafd thrtst
endif
endif

XINST	= install_xutils
XUNINST	= uninstall_xutils
XCLEAN	= makenodes.c
ULOPTS += -lpthread

include $(UTILDIR)/mk.bdy

default:
	$(nothing)

#thrtst:	thrtst.c
#	$(CC) -o $@ $^ $(ULOPTS) -lpthread
	
#strtst:	../strtst.c
#	$(CC) -o $@ $^ $(ULOPTS) -lpthread
	
ifeq ($(ARCH),ppc64le)
install_xutils:	fattach fdetach
else
ifeq ($(ARCH),x86_64)
install_xutils:	fattach fdetach
else
install_xutils:	fattach fdetach
endif
endif
	$(Q)install -d $(DESTDIR)$(sbindir)
	$(Q)install $^ $(DESTDIR)$(sbindir)

uninstall_xutils:
	-$(Q)(cd $(DESTDIR)$(sbindir) && \
		rm -f fattach fdetach)
