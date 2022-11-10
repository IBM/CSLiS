/*
 * Copyright 2022 - IBM Inc. All rights reserved
 * SPDX-License-Identifier: LGPL-2.1
 */
#ifndef LIS_MODULE_H
#define LIS_MODULE_H
/************************************************************************
*                        LiS Module Header File                         *
*************************************************************************
*									*
* A STREAMS driver that is capable of being a separately loadable	*
* module can include this file at the very beginning.  This header file	*
* will sense the MODULE and MODVERSIONS kernel parameters and include	*
* the proper kernel header files in the proper order.			*
* 									*
* There is a difference between 2.4 and 2.6 in this respect.  In 2.4	*
* the kernel defines "module_info", a standard STREAMS symbol.  If we	*
* include <sys/stream.h> BEFORE <linux/module.h> then the kernel's	*
* definition clashes with ours.						*
* 									*
* In 2.6 the kernel declares "dev_t" in such a way that it interferes	*
* with the LiS definition.  We need to define it away before including	*
* any kernel includes.							*
* 									*
* The bottom line is that the order of includes makes a difference	*
* depending upon whether it is a 2.4 or 2.6 kernel.  So this file exists*
* in order to make it easy for the driver write to get around this.	*
*									*
************************************************************************/

#ident "@(#) CSLiS module.h 7.11 2022-10-26 15:30:00 "

#if defined(LINUX) && defined(__KERNEL__)

#include <sys/LiS/linux-mdep.h>

#if defined(KERNEL_2_4)
#undef module_info
#endif

#ifdef MODULE
#  if defined(LINUX) && defined(__KERNEL__)
#    ifdef MODVERSIONS
#     ifdef LISMODVERS
#      include <sys/modversions.h>      /* /usr/src/LiS/include/sys */
#     else
#      if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5)
#       include <linux/modversions.h>
#      endif
#     endif
#    endif
#    include <linux/module.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,3,0)
MODULE_IMPORT_NS(STREAMS);
#endif
#  else
#    error This can only be a module in the Linux kernel environment
#  endif
#endif

#elif defined(USER)		/* defined(LINUX) && defined(__KERNEL__) */

#include <sys/LiS/user-mdep.h>

#endif				/* defined(LINUX) && defined(__KERNEL__) */

#include <sys/LiS/modcnt.h>	/* MODGET(), MODPUT() */

#endif		/* LIS_MODULE_H */
