#
# Copyright 2022 - IBM Inc. All rights reserved
# SPDX-License-Identifier: LGPL-2.1
#
KBUILD_CPPFLAGS += -DKM26 -DLIS_OBJNAME -DLiS -D__KERNEL__ -DVERSION_2 -Wall -Wstrict-prototypes  -D__KERNGLUE__ -DINLINE=inline -DSTATIC=static -D__SMP__ -DxLISMODVERS -I/usr/src/LiS/include -I/usr/include -DRCVOID -DUSE_KMEM_CACHE -DUSE_KMEM_TIMER -DLINUX -DUSE_KTHREAD -DMODULE
# Test for SLES 16 or later needed quotes on NAMESPACE
ifeq ($(QUOTEDNAMESPACE),y)
     KBUILD_CPPFLAGS += -DDEFAULT_SYMBOL_NAMESPACE=\"STREAMS\"
else
     KBUILD_CPPFLAGS += -DDEFAULT_SYMBOL_NAMESPACE=STREAMS
endif
CPPFLAGS += -DKM26 -DLIS_OBJNAME -DLiS -D__KERNEL__ -DVERSION_2 -Wall -Wstrict-prototypes  -D__KERNGLUE__ -DINLINE=inline -DSTATIC=static -D__SMP__ -DxLISMODVERS -I/usr/src/LiS/include -I/usr/include -DRCVOID -DUSE_KMEM_CACHE -DUSE_KMEM_TIMER -DLINUX -DUSE_KTHREAD -DMODULE
UNAME := $(shell uname -r)
ARCH := $(shell uname -m)
ifeq ($(ARCH),x86_64)
#  LINUX_TYPE set in buildLiS, Add Ubuntu ...x86_64-linux-gnu reference 
ifeq ($(LINUX_TYPE),DEBIAN)
KBUILD_CPPFLAGS += -I/usr/include/x86_64-linux-gnu/ -D_X86_64_LIS_ -Dccregparm=0
CPPFLAGS += -I/usr/include/x86_64-linux-gnu/ -D_X86_64_LIS_ -Dccregparm=0
else
KBUILD_CPPFLAGS += -DMODVERSIONS -DCONFIG_MODVERSIONS -D_X86_64_LIS_ -Dccregparm=0
CPPFLAGS += -DMODVERSIONS -DCONFIG_MODVERSIONS -D_X86_64_LIS_ -Dccregparm=0	
endif
endif
ifeq ($(ARCH),ppc64le)
ARCH := powerpc
#  Add Ubuntu ...powerpc64le-linux-gnu reference on ppc64le
ifeq ($(LINUX_TYPE),DEBIAN)
KBUILD_CPPFLAGS += -I/usr/include/powerpc64le-linux-gnu/ -D_PPC64_LIS_ -Dccregparm=0
CPPFLAGS += -I/usr/include/powerpc64le-linux-gnu/ -D_PPC64_LIS__ -Dccregparm=0
else
KBUILD_CPPFLAGS += -D_PPC64_LIS_ -Dccregparm=0
CPPFLAGS += -D_PPC64_LIS_ -Dccregparm=0
endif
endif
#  Add Ubuntu .../usr/include/s390x-linux-gnu reference on s390x
ifeq ($(ARCH),s390x)
ARCH := s390
ifeq ($(LINUX_TYPE),DEBIAN)
KBUILD_CPPFLAGS += -I/usr/include/s390x-linux-gnu/  -D_S390X_LIS_ -Dccregparm=0
CPPFLAGS += -I/usr/include/s390x-linux-gnu/ -D_S390X_LIS_ -Dccregparm=0
else
KBUILD_CPPFLAGS += -D_S390X_LIS_ -Dccregparm=0
KBUILD_CFLAGS += -fpic -D__PIC__
CPPFLAGS += -D_S390X_LIS_ -Dccregparm=0
endif
EXTRA_CFLAGS += -fpic -D__PIC__
endif

obj-m := streams.o streams-mtdrv.o streams-pipemod.o streams-relay3.o 

streams-objs := head.o dki.o msg.o buffcall.o mdep.o events.o msgutl.o queue.o safe.o stats.o stream.o strmdbg.o wait.o cmn_err.o version.o osif.o poll.o lispci.o lismem.o lislocks.o exports.o mod.o clone.o fifo.o loop.o minimux.o printk.o connld.o relay.o sad.o
streams-mtdrv-objs := mtdrv.o
streams-relay3-objs := relay3.o
streams-pipemod-objs := pipemod.o

setup:
	cp -p ../head/*.c ../head/linux/*.c ../head/modconf.inc .
	cp -p ../drivers/str/*.c ../drivers/str/linux/*.c .

install:
	../util/linux/makenodes
	cp -p ../util/linux/makenodes /usr/sbin/strmakenodes
	cp -p ../util/linux/streams /usr/sbin/streams
	mkdir -p /lib/modules/`uname -r`/misc
	cp -p streams*.ko /lib/modules/`uname -r`/misc
ifneq ($(wildcard /etc/modprobe.d),)
ifneq ($(wildcard /etc/modprobe.conf.streams),)
	mv /etc/modprobe.conf.streams /etc/modprobe.d/streams.conf
else
	cp ../modprobe.conf.streams /etc/modprobe.d/streams.conf
endif
endif
	chmod 755 ../scripts/clean-modprobe.conf
	../scripts/clean-modprobe.conf /etc/modprobe.conf
	../scripts/clean-modprobe.conf /etc/modprobe.conf.local 
	depmod

clean:
	rm -rf *.o *.ko *.c *.inc .*.cmd *.mod.c Module.symvers modules.order .tmp_versions streams* .exports.o.d *ur-safe .cache.mk

