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

LIB_SHARED = $(LIBDIR)/$(LIBFILE_SHARED)
LIB_PSHARED = $(LIBDIR)/$(LIBFILE_PSHARED)
LIB_STATIC = $(LIBDIR)/$(LIBFILE_STATIC)

CC += -c $(CCUSEROPT)

all: $(Q_CC) $(LIB_SHARED) $(LIB_PSHARED) $(LIB_STATIC)
	$(nothing)

$(LIB_SHARED): $(LIBOBJS)
	$(Q_ECHO) $(qtag_LD)$(reltarget)
ifeq ($(ARCH),ppc64le)
	$(Q)$(LD) -shared -melf64lppc -o $@ $^ -lc
else
ifeq ($(ARCH),x86_64)
	$(Q)$(LD) -shared -m elf_x86_64 -o $@ $^ -lc
else
	$(Q)$(LD) -shared -o $@ $^ -lc
endif
endif

$(LIB_PSHARED): $(PRELDOBJS)
	$(Q_ECHO) $(qtag_LD)$(reltarget)
ifeq ($(ARCH),ppc64le)
	$(Q)$(LD) -shared -melf64lppc -o $@ $^ -lc
else
ifeq ($(ARCH),x86_64)
	$(Q)$(LD) -shared -m elf_x86_64 -o $@ $^ -lc
else
	$(Q)$(LD) -shared -o $@ $^ -lc
endif
endif

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
	-$(Q)rm -f *.o .compiler
	-$(Q)rm -f $(LIB)

realclean: clean
	-$(Q)rm -f .depend
	-$(Q)rm -f $(LIB_SHARED)
	-$(Q)rm -f $(LIB_PSHARED)
	-$(Q)rm -f $(LIB_STATIC)

install: .compiler all
	$(Q_ECHO) $(qtag_INSTALL)$(relpwd)
ifeq ($(ARCH),s390x)
	$(Q)install -d $(DEST_LIB64DIR)
	$(Q)install -m 0755 $(LIB_SHARED) $(DEST_LIB64DIR)/$(LIBFILE_SHARED)
	$(Q)install -m 0755 $(LIB_PSHARED) $(DEST_LIB64DIR)/$(LIBFILE_PSHARED)
	$(Q)install -m 0644 $(LIB_STATIC) $(DEST_LIB64DIR)/$(LIBFILE_STATIC)
else
ifeq ($(ARCH),ppc64le)
	$(Q)install -d $(DEST_LIB64DIR)
	$(Q)install -m 0755 $(LIB_SHARED) $(DEST_LIB64DIR)/$(LIBFILE_SHARED)
	$(Q)install -m 0755 $(LIB_PSHARED) $(DEST_LIB64DIR)/$(LIBFILE_PSHARED)
	$(Q)install -m 0644 $(LIB_STATIC) $(DEST_LIB64DIR)/$(LIBFILE_STATIC)
else
ifeq ($(ARCH),x86_64)
	$(Q)install -d $(DEST_LIB64DIR)
	$(Q)install -m 0755 $(LIB_SHARED) $(DEST_LIB64DIR)/$(LIBFILE_SHARED)
	$(Q)install -m 0755 $(LIB_PSHARED) $(DEST_LIB64DIR)/$(LIBFILE_PSHARED)
	$(Q)install -m 0644 $(LIB_STATIC) $(DEST_LIB64DIR)/$(LIBFILE_STATIC)
else
	$(Q)install -d $(DEST_LIBDIR)
	$(Q)install -m 0755 $(LIB_SHARED) $(DEST_LIBDIR)/$(LIBFILE_SHARED)
	$(Q)install -m 0755 $(LIB_PSHARED) $(DEST_LIBDIR)/$(LIBFILE_PSHARED)
	$(Q)install -m 0644 $(LIB_STATIC) $(DEST_LIBDIR)/$(LIBFILE_STATIC)
endif
endif
endif

ifeq ($(DESTDIR),)
	$(Q)ldconfig
endif
	$(Q)install -d $(DESTDIR)$(pkgdatadir)/$(relpwd)
	$(Q)cat .compiler | \
	    sed "s:^:CC=\(:" | sed "s:$$:\):" | \
	    sed "s:$(SRCDIR)/include:$$\{pkgincludedir\}:g" | \
	    sed "s:$(SRCDIR):$$\{pkgdatadir\}:g" | \
	    sed "s:$(KINCL):$$\{KINCL\}:g" | \
	    sed "s:$(KSRC):$$\{KSRC\}:g" | \
	    cat > $(DESTDIR)$(pkgdatadir)/$(relpwd)/.compiler

uninstall:
ifeq ($(ARCH),s390x)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_SHARED)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_PSHARED)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_STATIC)
else
ifeq ($(ARCH),ppc64le)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_SHARED)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_PSHARED)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_STATIC)
else
ifeq ($(ARCH),x86_64)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_SHARED)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_PSHARED)
	-$(Q)rm -f $(DEST_LIB64DIR)/$(LIBFILE_STATIC)
else
	-$(Q)rm -f $(DEST_LIBDIR)/$(LIBFILE_SHARED)
	-$(Q)rm -f $(DEST_LIBDIR)/$(LIBFILE_PSHARED)
	-$(Q)rm -f $(DEST_LIBDIR)/$(LIBFILE_STATIC)
endif
endif
endif
ifeq ($(DESTDIR),)
	$(Q)ldconfig
endif


# the following relates to the Q_CC variable, which may be set to .compiler if
# this target's output is desired
#
.compiler:
	$(Q_ECHO) $(qtag__WD_)$(relpwd)
	$(Q_ECHO) $(qtag__CC_)$(CC)
	@echo $(CC) > $@
