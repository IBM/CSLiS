/*                               -*- Mode: C -*- 
 * <strport.h> --- Linux STREAMS portability declarations. 
 * Author          : gram & nemo
 * Created On      : Fri Mar 24 2:40:21 1995
 * RCS Id          ; $Id: strport.h,v 1.1.1.1 2005/04/12 20:27:06 ragnar Exp $
 * Last Modified By: David Grothe
 * Restrictions    : SHAREd items can be read/writen by usr
 *                 : EXPORTed items can only be read by usr
 *                 : PRIVATEd items cannot be read nor writen
 * Purpose         : All system dependent stuff goes here. The idea
 *                 : is that different versions of this file can be
 *                 : used to port STREAMS to other operating systems
 *                 : as well as providing a user-space testbed environment.
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 * Copyright 2022 - IBM Inc. All rights reserved
 * SPDX-License-Identifier: LGPL-2.1
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

#ifndef _STRPORT_H
#define _STRPORT_H

#ident "@(#) CSLiS strport.h 7.11 10/20/22 15:30:00 "
/*  *******************************************************************  */
/*                               Dependencies                            */

#if	defined( __MSDOS__)
#include <sys/LiS/dos-mdep.h>
#elif defined(LINUX)
#include <sys/LiS/linux-mdep.h>
#elif defined(USER)
#include <sys/LiS/user-mdep.h>
#elif defined(PORTABLE)
#include <sys/LiS/port-mdep.h>
#endif /* !__MSDOS__ */

/*
 * INLINE and STATIC
 */
#ifndef INLINE
#define	INLINE	
#endif
#ifndef STATIC
#define	STATIC	static
#endif

/*
 * linux/types.h does not have intptr_t or uintptr_t
 * For user level pgms, stdint.h supplies these
 */
#if defined(__KERNEL__) && !defined(_INTTYPES_H)
#define _INTTYPES_H	1		/* kernel types.h is just as good */
 					/* with the addition of these */
# if defined(_ASM_IA64_UNISTD_H)
#  ifndef intptr_t
typedef long		_intptr_t;
#  define intptr_t	_intptr_t
#  endif
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
typedef unsigned long   uintptr_t;
#else
#ifndef __KERNEL__
typedef unsigned long	uintptr_t;
#endif
#endif
# else					/* _ASM_IA64_UNISTD_H */
#  ifndef intptr_t
typedef int		_intptr_t;
#  define intptr_t	_intptr_t
#  endif
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
typedef unsigned int    uintptr_t;
#else
#ifndef __KERNEL__
typedef unsigned int	uintptr_t;
#endif
#endif
# endif					/* _ASM_IA64_UNISTD_H */

/*
 * Define some selected formatting phrases that we would have gotten
 * had we included the real inttypes.h.
 */
# if defined(_ASM_IA64_UNISTD_H)
#  define PRIx32	"x"
#  define PRIx64	"qx"
#  define PRId32	"d"
#  define PRId64	"qd"
#  define PRIu32	"u"
#  define PRIu64	"qu"
# else					/* _ASM_IA64_UNISTD_H */
#  define PRIx32	"x"
#  define PRIx64	"lx"
#  define PRId32	"d"
#  define PRId64	"ld"
#  define PRIu32	"u"
#  define PRIu64	"lu"
# endif					/* _ASM_IA64_UNISTD_H */

#endif

/*
 * Establish some version dependent sub-defines for all to see.
 */
/*
 * If neither KERNEL_2_0 nor KERNEL_2_1 is defined then we are
 * being compiled as portable code.  We want to use portable constructs
 * for such things as poll.
 */
#if !defined(KERNEL_2_0) && !defined(KERNEL_2_1)
#define PORTABLE_POLL   1
#else
# if defined(KERNEL_2_0)
# define PORTABLE_POLL   1
# elif defined(KERNEL_2_1)
# define LINUX_POLL      2
# else
# error "ifdef logic error involving KERNEL_2_0 and KERNEL_2_1"
# endif
#endif

#ifndef OPENFAIL
#define OPENFAIL	(-1)
#endif
#ifndef INFPSZ
#define INFPSZ		(-1)
#endif

#ifdef __KERNEL__
extern char	*lis_errmsg(int lvl) ;
extern void	*lis_malloc(int nbytes, int class, int use_cache,
					char *file_name,int line_nr)_RP;
extern void	 lis_free(void *ptr, char *file_name,int line_nr)_RP;
#endif				/* __KERNEL__ */

#endif /* _STRPORT_H */


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
