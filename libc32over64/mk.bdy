#
# Copyright 2022 - IBM Inc. All rights reserved
# SPDX-License-Identifier: LGPL-2.1
# 
# common makefile for all environments
#
# This file is included from a target-specific makefile
# in a subdirectory below this directory.
# The makefile including this file should have defined:
#   - The LiS configuration.
#     config.mk in the top of the source tree should
#     have been included.
#   - LIBOBJS variable.
#     Contains the names of the object files that should be in the
#     library.
#

LIBFILE_SHARED = lib$(LIBBASE).so
LIBFILE_PSHARED = libp$(LIBBASE).so
LIBFILE_STATIC = lib$(LIBBASE).a

LIB_SHARED = $(LIBDIR32OVER64)/$(LIBFILE_SHARED)
LIB_PSHARED = $(LIBDIR32OVER64)/$(LIBFILE_PSHARED)
LIB_STATIC = $(LIBDIR32OVER64)/$(LIBFILE_STATIC)

ifeq ($(ARCH),s390x)
CC += -c $(XOPTS) -fPIC
endif
ifeq ($(ARCH),ppc64)
CC += -c $(XOPTS) -fPIC -m32 -msoft-float -pipe -ffixed-r2 -mmultiple -mstring
LD = ld -m elf32ppc
endif
ifeq ($(ARCH),x86_64)
CC += -c $(XOPTS) -fPIC -m32 -O2 -fomit-frame-pointer
LD = ld -m elf_i386
endif



all: compiler $(LIB_SHARED) $(LIB_PSHARED) $(LIB_STATIC)

compiler:
	$(Q_ECHO) $(qtag__WD_)$(relpwd)
	$(Q_ECHO) $(qtag__CC_)$(CC)

$(LIB_SHARED): $(LIBOBJS)
	$(Q_ECHO) $(qtag_LD)$(reltarget)
	$(Q)$(LD) $(LDOPTS) -shared -o $@ $^ -lc

$(LIB_PSHARED): $(PRELDOBJS)
	$(Q_ECHO) $(qtag_LD)$(reltarget)
	$(Q)$(LD) $(LDOPTS) -shared -o $@ $^ -lc

$(LIB_STATIC): $(LIBOBJS)
	-$(Q)rm -f $@
	$(Q_ECHO) $(qtag_AR)$(reltarget)
	$(Q)$(AR) r $@ $^
	$(Q_ECHO) $(qtag_RANLIB)$(reltarget)
	$(Q)$(RANLIB) $@

%.o: %.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $<

%.o: $(LIBDIR)/%.c
	$(Q_ECHO) $(qtag_CC)$(relpwdtarget)
	$(Q)$(CC) -o $@ $<

clean:
	$(Q_ECHO) $(qtag_CLEAN)$(relpwd)
	-$(Q)rm -f *.o
	-$(Q)rm -f $(LIB)

realclean: clean
	-$(Q)rm -f .depend
	-$(Q)rm -f $(LIB_SHARED)
	-$(Q)rm -f $(LIB_STATIC)


install: all
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
	$(Q)install -d $(DEST_LIBDIR)
	$(Q)install -m 0755 $(LIB_SHARED) $(DEST_LIBDIR)/$(LIBFILE_SHARED)
	$(Q)install -m 0755 $(LIB_PSHARED) $(DEST_LIBDIR)/$(LIBFILE_PSHARED)
	$(Q)install -m 0644 $(LIB_STATIC) $(DEST_LIBDIR)/$(LIBFILE_STATIC)
ifeq ($(DESTDIR),)
	$(Q)ldconfig
endif

uninstall:
	-$(Q)rm -f $(DEST_LIBDIR)/$(LIBFILE_SHARED)
	-$(Q)rm -f $(DEST_LIBDIR)/$(LIBFILE_PSHARED)
	-$(Q)rm -f $(DEST_LIBDIR)/$(LIBFILE_STATIC)
ifeq ($(DESTDIR),)
	$(Q)ldconfig
endif
