/*                               -*- Mode: C -*- 
 * mdep.h --- machine (actually kernel) dependencies.
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: linux-mdep.h,v 1.2 2006/05/18 17:06:07 steve Exp $
 * Purpose         : provide kernel independence as much as possible
 *                 : This could be also considered to be en embryo for
 *                 : dki stuff,i.e. linux-dki
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Francisco J. Ballesteros, Denis Froschauer
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
 *    You can reach su by email to any of
 *    nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 *    dave@gcom.com
 */

#ifndef _LIS_M_DEP_H
#define _LIS_M_DEP_H 1

#ident "@(#) CSLiS linux-mdep.h 7.11 2022-10-26 15:30:00 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifdef __KERNEL__

/*
 * We want to discard the kernel's definition of module_info since
 * it clashes with a standard STREAMS usage.
 */
#define module_info	kernel_module_info

#endif

/*
 * types.h will include <linux/config.h>.  If we have generated our own
 * autoconf.h file then we need to include it prior to anything else.
 * We set the include marker that prevents the <linux/config.h> from
 * including its own autoconf.h.
 */
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#include <linux/kconfig.h>
#define f_vfsmnt   f_path.mnt   /* define needed for later kernel support */
#endif
#ifdef LISAUTOCONF
#include <sys/autoconf.h>           /* /usr/src/LiS/include/sys */
#define _LINUX_CONFIG_H 1	    /* prevent <linux/config.h> */
#endif
#if !defined(NOKSRC)		/* have kernel source */
#ifndef _SYS_TYPES_H
#include <linux/types.h>
#define _SYS_TYPES_H	1	/* pretend included */
#endif
#endif

/* kernel includes go here */
#ifdef __KERNEL__
/*
 * The IRDA driver's use of queue_t interferes with ours.  Later versions
 * of the kernel source do not have this problem, but this is safe for all
 * versions.
 */
#define queue_t	irda_queue_t
#include <linux/version.h>
#ifdef MODVERSIONS
# ifdef LISMODVERS
# include <sys/modversions.h>           /* /usr/src/LiS/include/sys */
# else
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5)
#   include <linux/modversions.h>
#  endif
# endif
#endif
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#define	KERNEL_2_0
#else
# define	KERNEL_2_1	/* 2.1.x and 2.2.x kernel */
# if LINUX_VERSION_CODE > KERNEL_VERSION(2,3,0)
# define	KERNEL_2_3	/* 2.3.x and 2.4.x kernel */
#  if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,7)
#  define	KERNEL_2_4_7	/* 2.4.7+ redefines dentry structure */
#  endif
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
#  define	KERNEL_2_5	/* 2.5.x and 2.6.x kernel */
#  endif

# endif
#endif

#if !defined(KERNEL_2_5) && !defined(KERNEL_2_3)
#errof "LiS cannot be compiled for pre 2.4 kernels"
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0)
#define PIPE_SEM(inode)         (&(inode).i_mutex)
#else
#define PIPE_SEM(inode)         (&(inode).i_rwsem)
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
#define PIPE_WAIT(inode)        (&(inode).i_pipe->rd_wait)
#else
#define PIPE_WAIT(inode)        (&(inode).i_pipe->wait)
#endif
#define PIPE_READERS(inode)     ((inode).i_pipe->readers)
#define PIPE_WRITERS(inode)     ((inode).i_pipe->writers)
#define PIPE_RCOUNTER(inode)    ((inode).i_pipe->r_counter)
#define PIPE_WCOUNTER(inode)    ((inode).i_pipe->w_counter)
#endif

#if defined(NOKSRC)

#include <linux/types.h>        /* common system types */
#include <linux/spinlock.h>
#define	__need_sigset_t	1
#include <signal.h>
#define timespec time_h_timespec
#include <linux/time.h>
#undef timespec

#else

#if !defined(_LINUX_TYPES_H)
#include <linux/types.h>        /* common system types */
#endif

#endif

#include <linux/kdev_t.h>	/* 1.3.xx needs this */
#include <linux/sched.h>	/* sleep,wake,... */
#include <linux/wait.h>
#include <linux/kernel.h>	/* suser,...*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5,14,0))
#include <linux/interrupt.h>
#endif
#include <linux/major.h>
#include <linux/fs.h>		/* inodes,... */
#include <linux/fcntl.h>	/* inodes,... */
#include <linux/string.h>	/* memcpy,... */
#include <linux/timer.h>	/* timers */
#include <linux/mm.h>		/* memory manager, pages,... */
#include <linux/slab.h>		/* memory manager, pages,... */
#include <linux/stat.h>		/* S_ISCHR */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10)
#include <asm/segment.h>	/* memcpy_{to,from}_fs */
#endif
#include <linux/errno.h>	      /* for errno */
#include <linux/signal.h>	      /* for signal numbers */
#include <sys/poll.h>		/* ends up being linux/poll.h */
#include <linux/file.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,17,0)
#include <linux/cred.h>
#endif
#include <asm/uaccess.h>
#include <sys/LiS/genconf.h>	/* generated configs from installation */
#include <sys/LiS/config.h>
/* #include <sys/lislocks.h>	needs lis_atomic_t, below */

/*
 * Kernel loadable module support
 */
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#define LIS_LOADABLE_SUPPORT 1
#endif
#if defined(KERNEL_2_5)
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
#define INCLUDE_VERMAGIC 1
#endif
#include <linux/vermagic.h>
#endif
#undef queue_t			/* allow visibility for LiS */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
/*
 * The symbol "dev_t" inside LiS is the LiS definition (32 bits) not
 * the kernel definition (16 bits in 2.4).  STREAMS drivers always mean
 * LiS-dev_t when they use this typedef.
 */
#if defined(dev_t)
#undef dev_t
#endif
#define dev_t	lisdev_t
typedef unsigned int		lisdev_t ;
#endif

/*
 * If some kernel include did a #define on module_info, undo it
 */
#if defined(module_info)
#undef module_info
#endif

/*
 * Capture the kernel's idea of cache line size.  LiS allocators
 * align memory on this boundary.
 */
#ifdef SMP_CACHE_BYTES
#define LIS_CACHE_BYTES	SMP_CACHE_BYTES
#else
#define LIS_CACHE_BYTES	16
#endif

#endif /* __KERNEL__ */

/*  -------------------------------------------------------------------  */

/*  -------------------------------------------------------------------  */


#ifdef __KERNEL__
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,1)
#ifndef signal_pending
#define	signal_pending(tsk)	(tsk->signal & ~tsk->blocked)
#endif
#ifndef sigismember
#define sigismember(sig_msk_addr,signo) ( *(sig_msk_addr) & (1 << (signo)) )
#endif
#ifndef sigaddset
#define sigaddset(sig_msk_addr,signo) ( *(sig_msk_addr) |= (1 << (signo)) )
#endif
#endif
#endif


/*  -------------------------------------------------------------------  */

#if defined(__KERNEL__) && defined(KERNEL_2_0)

# ifndef MODULE_AUTHOR
# define MODULE_AUTHOR(str)	 static const char module_author[] = str
# endif
# ifndef MODULE_DESCRIPTION
# define MODULE_DESCRIPTION(str) static const char module_description[] = str
# endif
#endif

/* some missing symbols
 */

#ifdef __KERNEL__
			/* seconds to system tmout time units */
#define SECS_TO(t)	lis_milli_to_ticks(1000*(t))

extern long lis_time_till(long target_time) _RP;
extern long lis_target_time(long milli_sec) _RP;
extern long lis_milli_to_ticks(long milli_sec)  _RP;
#endif				/* __KERNEL__ */

/* some missing generic types 
 */
#undef uid 
#undef gid
typedef int     o_uid_t;
typedef int     o_gid_t;
typedef unsigned   char uchar;
#ifdef __KERNEL__
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
typedef struct cred {
	uid_t	cr_uid;			/* effective user id */
	gid_t	cr_gid;			/* effective group id */
	uid_t	cr_ruid;		/* real user id */
	gid_t	cr_rgid;		/* real group id */
} cred_t;
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
typedef struct {
        uid_t   cr_uid;                 /* effective user id */
        gid_t   cr_gid;                 /* effective group id */
        uid_t   cr_ruid;                /* real user id */
        gid_t   cr_rgid;                /* real group id */
} cred_t;
#else
typedef struct {
        kuid_t   cr_uid;                 /* effective user id */
        kgid_t   cr_gid;                 /* effective group id */
        kuid_t   cr_ruid;                /* real user id */
        kgid_t   cr_rgid;                /* real group id */
} cred_t;
#endif
#endif
#endif

#ifdef __KERNEL__
#if defined(KERNEL_2_5)
#define lis_suser(fp)	capable(CAP_SYS_ADMIN)
#else
#define lis_suser(fp)	suser()
#endif
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */


#ifdef __KERNEL__
#define lis_free_page(cp) free_page((unsigned long)(cp))

#define PRINTK		printk

/*
 *  The ASSERT macro.
 */
extern void lis_assert_fail(const char *expr, const char *objname,
			    const char *file, unsigned int line) _RP;

#ifdef LIS_OBJNAME
#define ___ASSERT_XSTR(s) ___ASSERT_STR(s)
#define ___ASSERT_STR(s) #s
#define LISASSERT(expr)							   \
	((void)((expr) ? 0 : lis_assert_fail(#expr,			   \
					      ___ASSERT_XSTR(LIS_OBJNAME), \
					     __FILE__, __LINE__)))
#else
#define LISASSERT(expr)							   \
	((void)((expr) ? 0 : lis_assert_fail(#expr, "streams",		   \
					     __FILE__, __LINE__)))
#endif

/*
 * Task identity
 */
#define	lis_is_current_task(taskp)	( ((void *)current) == (void *)(taskp) )
#define	lis_current_task_ptr		( (void *)current )

/* disable/enable interrupts
 */
#define SPLSTR(x)	x = lis_splstr()
#define SPLX(x)		lis_splx(x)


/*
 * Atomic functions
 *
 * Usage is: lis_atomic_t	av ;
 *           lis_atomic_set(&av, 1) ;
 *
 * Use the lis_ versions of these for portability across kernel versions.
 * You can use the direct kernel versions for speed at the risk of needing a
 * recompile of your driver code with each new kernel version.
 *
 * The "long" type is intended to be an opaque type to the user.  The routines
 * cast the pointer to lis_atomic_t to a pointer to atomic_t (kernel struct)
 * for operational purposes.  By keeping the kernel's atomic_t type invisible
 * from STREAMS drivers we help insulate them from kernel changes.
 */
typedef	volatile long		lis_atomic_t ;

void	lis_atomic_set(lis_atomic_t *atomic_addr, int valu) _RP;
int	lis_atomic_read(lis_atomic_t *atomic_addr) _RP;
void	lis_atomic_add(lis_atomic_t *atomic_addr, int amt) _RP;
void	lis_atomic_sub(lis_atomic_t *atomic_addr, int amt) _RP;
void	lis_atomic_inc(lis_atomic_t *atomic_addr) _RP;
void	lis_atomic_dec(lis_atomic_t *atomic_addr) _RP;
int	lis_atomic_dec_and_test(lis_atomic_t *atomic_addr) _RP;

/*
 * Internally we can use these direct access macros for speed since LiS
 * is always compiled from source against the running kernel.
 */
#define	K_ATOMIC_INC(lis_atomic_addr) atomic_inc((atomic_t *)(lis_atomic_addr))
#define	K_ATOMIC_DEC(lis_atomic_addr) atomic_dec((atomic_t *)(lis_atomic_addr))
#define	K_ATOMIC_READ(lis_atomic_addr) atomic_read((atomic_t *)(lis_atomic_addr))
#define	K_ATOMIC_SET(lis_atomic_addr,v)		\
    				atomic_set((atomic_t *)(lis_atomic_addr),(v))
#define	K_ATOMIC_ADD(lis_atomic_addr,v)		\
    				atomic_add((v),(atomic_t *)(lis_atomic_addr))
#define	K_ATOMIC_SUB(lis_atomic_addr,v)		\
				atomic_sub((v),(atomic_t *)(lis_atomic_addr))

extern int	lis_in_interrupt(void) _RP ;

/*
 * Now include lislocks.h
 */
#include <sys/lislocks.h>

/*
 *  lis_gettimeofday -  used by lis_hitime and similar functions
 */
void lis_gettimeofday(struct timeval *tv)_RP;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,15)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0)
/* lock inodes...
 *
 * Must use kernel mutex routine directly since the inode mutex is a
 * kernel mutex and not an LiS semaphore.
 */
int lis_kernel_down(struct mutex *mut)_RP;
void lis_kernel_up(struct mutex *mut)_RP;
#else
/* lock inodes...
 *
 * Must use kernel routine  to use semaphore on the inode 
 */
int lis_kernel_down(struct rw_semaphore *i_rwsem)_RP;
void lis_kernel_up(struct rw_semaphore *i_rwsem)_RP;
#endif
#else
/* lock inodes...
 *
 * Must use kernel semaphore routine directly since the inode semaphore is a
 * kernel semaphore and not an LiS semaphore.
 */
int lis_kernel_down(struct semaphore *sem)_RP;
void lis_kernel_up(struct semaphore *sem)_RP;
#endif

#if 0			/* don't need to hold inode semaphore for I/O oprns */
#define	LOCK_INO(i)	lis_kernel_down(&((i)->i_sem))
#define	ULOCK_INO(i)	lis_kernel_up(&((i)->i_sem))
#else
#define	LOCK_INO(i)	do {;} while (0)
#define	ULOCK_INO(i)	do {;} while (0)
#endif

/*
 *  inode/stdata access
 */
struct inode  *lis_file_inode(struct file *f);
char          *lis_file_name(struct file *f);
struct stdata *lis_file_str(struct file *f);
void           lis_set_file_str(struct file *f, struct stdata *s);
struct stdata *lis_inode_str(struct inode *i);
void           lis_set_inode_str(struct inode *i, struct stdata *s);
struct inode  *lis_set_up_inode(struct file *f, struct inode *inode) ;
#define FILE_INODE(f)   lis_file_inode(f)
#define FILE_NAME(f)    lis_file_name(f)
#define FILE_STR(f)     lis_file_str(f)
#define SET_FILE_STR(f,s) lis_set_file_str(f,s)
#define INODE_STR(i)    lis_inode_str(i)
#define SET_INODE_STR(i,s)  lis_set_inode_str(i,s)
#define	I_COUNT(i)	( (i) ? atomic_read(&((i)->i_count)) : -1 )

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,24)
   #define F_COUNT(f)	( (f) ? atomic_read(&((f)->f_count)) : -1 )
#else
   #define F_COUNT(f)   ( (f) ? atomic_long_read(&((f)->f_count)) : -1 )
#endif

struct dentry *lis_d_alloc_root(struct inode *i, int m);
void           lis_dput(struct dentry *d);
struct dentry *lis_dget(struct dentry *d);

/*
 * mode (m) parameter values for lis_d_alloc_root
 */

#define LIS_D_ALLOC_ROOT_NORMAL   0
#define LIS_D_ALLOC_ROOT_MOUNT    1

/*
 *  clone support
 */
extern struct inode *lis_grab_inode(struct inode *ino);
extern void          lis_put_inode(struct inode *ino);
extern int           lis_is_stream_inode(struct inode *ino);
extern struct inode *lis_new_inode(struct file *,dev_t);
extern struct inode *lis_old_inode(struct file *,struct inode *);
extern int           lis_new_file_name(struct file *, const char *);
extern void          lis_new_stream_name(struct stdata *, struct file *);
#if defined(KERNEL_2_1)
extern void          lis_cleanup_file_opening(struct file *,
					      struct stdata *, int,
					      struct dentry *,  int,
					      struct vfsmount *, int);
#else
extern void          lis_cleanup_file_opening(struct file *,
					      struct stdata *, int);
#endif
extern void          lis_cleanup_file_closing(struct file *, struct stdata *);

extern int lis_major;

#if defined(KERNEL_2_5)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
extern int lis_strflush(struct file *);
#else
extern int lis_strflush(struct file *, fl_owner_t);
#endif
#endif

/*
 * Device node support
 */
int     lis_mknod(char *name, int mode, dev_t dev) _RP;
int     lis_unlink(char *name) _RP;


/*
 *  FIFO/pipe support
 */
extern int lis_get_fifo(struct file **);
extern int lis_get_pipe(struct file **, struct file **);
extern int lis_pipe( unsigned int * );

extern int  lis_fifo_open_sync(struct inode *, struct file *);
extern void lis_fifo_close_sync(struct inode *, struct file *);
extern int  lis_fifo_write_sync(struct inode *, int);

/*
 *  syscall interface
 */
extern asmlinkage int lis_sys_pipe(unsigned int *);
/*
 *  ioctl interface
 */
extern int lis_ioc_pipe(unsigned int *);

/*
 *  fattach()/fdetach() support
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
extern int lis_fattach(struct file *, const char *);
extern int lis_fdetach(const char *);
#else
extern int lis_fattach(struct file *, struct filename * );
extern int lis_fdetach(struct filename *);
#endif

/*
 *  syscall interfaces
 */
extern asmlinkage int lis_sys_fattach(int, const char *);
extern asmlinkage int lis_sys_fdetach(const char *);
/*
 *  ioctl interfaces
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
extern int lis_ioc_fattach(struct file *, char *);
extern int lis_ioc_fdetach(char *);
#else
extern int lis_ioc_fattach(struct file *, struct filename *);
extern int lis_ioc_fdetach(struct filename *);
#endif

/*
 * Kernel loadable module support
 */
#ifdef CONFIG_KERNELD
#define LIS_LOADABLE_SUPPORT 1
#endif

/* lis_loadable_load - load a loadable module.
 *
 * Routine is always present but returns error if the kernel is not
 * built for dynamic loading of modules from within the kernel.
 */
int lis_loadable_load(const char *name);

/*
 * Process kill
 */
extern int	lis_kill_proc(int pid, int sig, int priv) ;
extern int	lis_kill_pg (int pgrp, int sig, int priv) ;

/************************************************************************
*                            major/minor                                *
*************************************************************************
*									*
* Macros to extract the major and minor device numbers from a dev_t	*
* variable.								*
*									*
* This mechanism is being reworked for LiS-2.17.  The dev_t is defined	*
* by the kernel as an unsigned int, 32 bits.  The kernel has its own	*
* idea of how the major and minor device numbers are packed into this	*
* word.  In 2.4 it is 8maj/8min.  In 2.6 it is 12maj/20min.		*
*									*
* LiS is a file system and therefore gets to defines its own major/minor*
* device number split.  We use the 12/20 convention internally.  That	*
* means that any dev_t-s internal to LiS conform to that format.	*
* 									*
* LiS provides the standard Unix functions makedevice(maj,min),		*
* getmajor(dev_t) and getminor(dev_t).					*
*									*
* The MAJOR and MINOR macros are left as defined by the kernel.  STREAMS*
* drivers should not use these macros.  They should use getmajor() and	*
* getminor() instead.							*
*									*
* The lower case macros major() and minor() are to be used only by	*
* user space programs.							*
*									*
* STREAMS nodes in the /dev tree consume one major device number per	*
* STREAMS driver just as before.  The external major/minor device must	*
* conform to the external conventions -- 8/8 as far as I know.  STREAMS	*
* nodes do NOT work with devfs (AFAIK).					*
*									*
* Internally an LiS treatment of dev_t will have the external major	*
* number in its major field and the internal 20 bit minor number in its	*
* minor field.  The LiS open routine does the translation, so by the 	*
* time any STREAMS driver sees a dev_t, it is in LiS format.		*
*									*
* We don't provide any defines for the format of the LiS dev_t here.	*
* Use the getmajor/getminor routines.					*
*									*
************************************************************************/

/*
 * Major and minor macros come from linux ./include/linux/kdev_t.h
 */

#ifdef minor_t
#undef minor_t
#endif
#ifdef major_t
#undef major_t
#endif

typedef unsigned	lis_major_t ;
typedef unsigned	lis_minor_t ;
#define	major_t		lis_major_t
#define	minor_t		lis_minor_t

extern major_t		lis_getmajor(dev_t dev) _RP;
extern minor_t		lis_getminor(dev_t dev) _RP;
extern dev_t		lis_makedevice(major_t majr, minor_t minr) _RP;
extern dev_t		lis_kern_to_lis_dev(dev_t dev) ;

/*
 * If ddi.h has not been included, provide definitions for makedevice,
 * getmajor and getminor here.
 */
#ifndef _DDI_H

#define makedevice              lis_makedevice
#define getmajor                lis_getmajor
#define getminor                lis_getminor

#endif

/*
 * Make a dev_t as it would be in user mode.  Used to call lis_mknod()
 */
#define UMKDEV(majr,minr)	(((majr) << 8) | (minr))

#define DEV_TO_INT(dev) ((int)(dev))
#define DEV_SAME(d1,d2)	(DEV_TO_INT(d1) == DEV_TO_INT(d2))

extern dev_t			lis_i_rdev(struct inode *) ;
#define	GET_I_RDEV(inode)	lis_i_rdev(inode)

/*
 * 2.4 used kdev_t for inode->i_rdev.  2.6 uses dev_t for that field.
 * These conversion routines are used to handle the i_rdev field.  Since
 * the i_rdev field is in LiS format all these do is apply casts to
 * silence the compiler.
 *
 * MKDEV is defined by the kernel and makes a kernel device "structure"
 * out of a major and minor device number.
 */
#if defined(KERNEL_2_5)
#define	DEV_TO_RDEV(dev)	(dev)
#define	RDEV_TO_DEV(rdev)	(rdev)
#define	RDEV_TO_INT(rdev)	((int)(rdev))
#else
#define	DEV_TO_RDEV(dev)	((kdev_t)(dev))
#define	RDEV_TO_DEV(rdev)	((dev_t)(rdev))
#define	RDEV_TO_INT(rdev)	((int)(rdev))
#endif


#define	LIS_FIFO  FIFO__CMAJOR_0
#define LIS_CLONE CLONE__CMAJOR_0

/* Use Linux system macros for MAJOR and MINOR */
#if defined(KERNEL_2_5)

#define	STR_MAJOR		lis_getmajor	/* for dev_t */
#define	STR_MINOR		lis_getminor	/* for dev_t */
#define	STR_KMAJOR		MAJOR		/* for kdev struct */
#define	STR_KMINOR		MINOR		/* for kdev struct */

#else			/* not KERNEL_2_5 */

#define	STR_MAJOR		lis_getmajor	/* for dev_t */
#define	STR_MINOR		lis_getminor	/* for dev_t */
#define	STR_KMAJOR		MAJOR		/* for kdev struct */
#define	STR_KMINOR		MINOR		/* for kdev struct */

#endif			/* KERNEL_2_5 */

/*			End of Major/Minor Device Definitions		*/

/*
 * Kernel threads
 *
 * This function can be used to start a kernel thread.  Any driver can use
 * this function.
 *
 * 'fcn' is the function that serves as the entry point for the thread.
 *
 * 'arg' is the argument passed to that function.
 *
 * 'name' is the name to give to the function.  Keep it under 16 bytes.
 *
 * lis_thread_start returns the pid of the new process, or < 0 if an error
 * occurred.
 *
 * Before the 'fcn' is entered, the newly created thread will have shed all
 * user space files and mapped memory.  All signals are still enabled.  Note
 * that when the kernel goes down for reboot all processes are first sent a
 * SIGTERM.  Once those have been processed, all processes are then sent a
 * SIGKILL.  It is the implementor's choice which of these it pays attention to
 * in order to exit prior to a reboot.  The LiS queue runner ignores SIGTERM so
 * that it can be in place to help close streams files.  It then exits on
 * SIGKILL.  Other processes may behave differently.
 *
 * The 'fcn' is entered with the "big kernel lock" NOT held, just as it would
 * be for calling the "kernel_thread" function directly.  On 2.2 kernels, the
 * 'fcn' should get this lock so that it can utilize kernel services safely.
 *
 * The user's function returns a value when it exits and that value is returned
 * to the kernel.  It is not clear that anything actually pays any attention to
 * this returned value.  It particular, it is not visible to the thread that
 * started the new thread.
 */
pid_t   lis_thread_start(int (*fcn)(void *), void *arg, const char *name) _RP;
int	lis_thread_stop(pid_t pid) _RP;

#else				/* __KERNEL__ */

/*
 * sad.h needs these definitions in user space.
 */
typedef unsigned	lis_major_t ;
typedef unsigned	lis_minor_t ;
#define	major_t		lis_major_t
#define	minor_t		lis_minor_t

/*
 * For user programs, provide a substitute for the lis_atomic_t that
 * is the same size, and hopefully numerical layout, as the kernel's
 * type.  This allows uer programs to view data structures that have
 * lis_atomic_t data in them.
 */
typedef	volatile long		lis_atomic_t ;

#define	lis_atomic_set(atomic_addr,valu) (*(atomic_addr) = (valu))
#define	lis_atomic_read(atomic_addr) 	 (*(atomic_addr))
#define	lis_atomic_add(atomic_addr,amt)  (*(atomic_addr) += (amt))
#define	lis_atomic_sub(atomic_addr,amt)  (*(atomic_addr) -= (amt))
#define	lis_atomic_inc(atomic_addr) 	 ((*(atomic_addr))++)
#define	lis_atomic_dec(atomic_addr) 	 ((*(atomic_addr))--)
#define	lis_atomic_dec_and_test(atomic_addr) ((*(atomic_addr))--)



#endif				/* __KERNEL__ */

#ifdef __KERNEL__

#ifndef VOID
#define VOID	void
#endif

#ifdef __KERNEL__
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
#define UID(fp)	  current->uid
#define GID(fp)	  current->gid
#define EUID(fp)  current->euid
#define EGID(fp)  current->egid
#else
#define UID(fp)   current_uid()
#define GID(fp)   current_gid()
#define EUID(fp)  current_euid()
#define EGID(fp)  current_egid()
#endif
#endif

#if defined(KERNEL_2_5)
#define PGRP(fp)  process_group(current)
#else
#define PGRP(fp)  current->pgrp
#endif
#define PID(fp)	  current->pid

#define OPENFILES()     current->files->count
#define SESSION()       current->session

#define DBLK_ALLOC(n,f,l,g)	lis_malloc(n,GFP_ATOMIC | (g),1,f,l)
#define ALLOC(n)		lis_malloc(n,GFP_ATOMIC,0,__FILE__,__LINE__)
#define ZALLOC(n)		lis_zmalloc(n,GFP_ATOMIC,__FILE__,__LINE__)
#define ALLOCF(n,f)		lis_malloc(n,GFP_ATOMIC,0, f __FILE__,__LINE__)
#define ALLOCF_CACHE(n,f)	lis_malloc(n,GFP_ATOMIC,1, f __FILE__,__LINE__)
#define MALLOC(n)		lis_malloc(n,GFP_ATOMIC,0,__FILE__,__LINE__)
#define LISALLOC(n,f,l)		lis_malloc(n,GFP_ATOMIC,0,f,l)
#define FREE(p)			lis_free(p,__FILE__,__LINE__)
#define MEMCPY(dest, src, len)	memcpy(dest, src, len)
#define PANIC(msg)		panic(msg)

#if (defined(LINUX) && defined(USE_KMEM_CACHE))

extern lis_atomic_t             lis_qsync_cnt;
extern lis_atomic_t             lis_locks_cnt;
extern lis_atomic_t             lis_head_cnt;
extern lis_atomic_t             lis_qband_cnt;
extern lis_atomic_t             lis_queue_cnt;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
void lis_cache_destroy(kmem_cache_t *p, lis_atomic_t *c, char *label);
#else
void lis_cache_destroy(struct kmem_cache *p, lis_atomic_t *c, char *label);
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
static inline void *lis_cache_alloc(kmem_cache_t *cp, lis_atomic_t *cntr)
#else
static inline void *lis_cache_alloc(struct kmem_cache *cp, lis_atomic_t *cntr)
#endif
{
    void *p = kmem_cache_alloc(cp, GFP_ATOMIC) ;

    if (p != NULL)
	K_ATOMIC_INC(cntr) ;

    return(p) ;
}

#define LIS_CA(cachep, cntr)	lis_cache_alloc(cachep, &cntr)
#define LIS_CF(cachep, p, cntr)	(kmem_cache_free(cachep,(p)), \
				         K_ATOMIC_DEC(&cntr))

#define	LIS_QSYNC_FREE(p)	LIS_CF(lis_qsync_cachep, (p), lis_qsync_cnt)
#define	LIS_LOCK_FREE(p)	LIS_CF(lis_locks_cachep, (p), lis_locks_cnt)
#define	LIS_HEAD_FREE(p)	LIS_CF(lis_head_cachep, (p), lis_head_cnt)
#define	LIS_QBAND_FREE(p)	LIS_CF(lis_qband_cachep, (p), lis_qband_cnt)
#define	LIS_QUEUE_FREE(p)	LIS_CF(lis_queue_cachep, (p), lis_queue_cnt)
#define	LIS_QUEUE_ALLOC(nb,s)	LIS_CA(lis_queue_cachep, lis_queue_cnt)
#define LIS_QBAND_ALLOC(nb,s)	LIS_CA(lis_qband_cachep,lis_qband_cnt)
#define LIS_HEAD_ALLOC(nb,s)	LIS_CA(lis_head_cachep,lis_head_cnt)
#define LIS_LOCK_ALLOC(nb,s)	LIS_CA(lis_locks_cachep,lis_locks_cnt)
#define LIS_QSYNC_ALLOC(nb,s)	LIS_CA(lis_qsync_cachep,lis_qsync_cnt)

#else

#define	LIS_QSYNC_FREE		FREE
#define	LIS_LOCK_FREE		FREE
#define	LIS_HEAD_FREE		FREE
#define	LIS_QBAND_FREE		FREE
#define	LIS_QUEUE_FREE		FREE
#define	LIS_QUEUE_ALLOC(nb,s)	ALLOCF_CACHE(nb,s)
#define LIS_QBAND_ALLOC(nb,s)	ALLOCF(nb,s)
#define LIS_HEAD_ALLOC(nb,s)	ALLOCF(nb,s)
#define LIS_LOCK_ALLOC(nb,s)	ALLOCF(nb,s)
#define LIS_QSYNC_ALLOC(nb,s)	ALLOCF(nb,s)

#endif
/*
 * These are used only internally
 */
#define	KALLOC(n,cls,cache)	lis__kmalloc(n,cls,cache)	/* lismem.c */
#define	KFREE(p)		lis__kfree(p)			/* lismem.c */

extern struct stdata	*lis_fd2str(int fd) ;	/* file descr -> stream */

extern void *lis__kmalloc(int nbytes, int class, int use_cache) ;
extern void *lis__kfree(void *ptr) ;

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

/* This should be entry points from the kernel into LiS
 * kernel should be fixed to call them when appropriate.
 */

/* some kernel memory has been free'd 
 * tell STREAMS
 */
#ifdef __KERNEL__
extern void
lis_memfree( void );

/* Get avail kernel memory size
 */
#define lis_kmemavail()	((unsigned long)-1) /* lots of mem avail :) */

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/* This will copyin usr string pointed by ustr and return the result  in
 * *kstr. It will stop at  '\0' or max bytes copyed in.
 * caller should call free_page(*kstr) on success.
 * Will return 0 or errno
 */
#ifdef __KERNEL__
extern int 
lis_copyin_str(struct file *fp, const char *ustr, char **kstr, int max);
int     lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len);
int     lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len);
int	lis_check_umem(struct file *fp, int rd_wr_fcn,
	                   const void *usr_addr, int lgth) ;


#endif				/* __KERNEL__ */


/*  -------------------------------------------------------------------  */

/*
 * The routine 'lis_runqueues' just requests that the queues be run
 * at a later time.  A daemon process runs the queus with the help
 * of a special driver.  This driver has the routine lis_setqsched
 * in it.  See drivers/str/runq.c.
 */
#ifdef __KERNEL__

extern void	lis_setqsched(int can_call) ;
extern lis_atomic_t	lis_in_syscall ;
extern lis_atomic_t	lis_runq_req_cnt ;
#define	lis_runqueues()		do {					      \
    				     if (K_ATOMIC_READ(&lis_runq_req_cnt))  \
					lis_setqsched(1);		      \
				   } while(0)


#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*
 * Macros for locking and unlocking a queue structure.
 */
#define lis_lockqf(qp,f,l)   lis_lockq_fcn((qp),f,l)
#define lis_lockq(qp)	     lis_lockqf(qp,__FILE__,__LINE__)
#define lis_unlockqf(qp,f,l) lis_unlockq_fcn((qp),f,l)
#define lis_unlockq(qp)	     lis_unlockqf(qp,__FILE__,__LINE__)


/*  -------------------------------------------------------------------  */

/*
 * The routine 'lis_select' handles select calls from the Linux kernel.
 * The structure 'lis_select_t' is embedded in the stdata structure
 * and contains the wait queue head.
 */
#ifdef __KERNEL__

#ifdef KERNEL_2_0

typedef struct lis_select_struct
{
    struct wait_queue	*sel_wait ;

} lis_select_t ;

extern int	lis_select(struct inode *inode, struct file *file,
			   int sel_type, select_table *wait) ;

extern void	lis_select_wakeup(struct stdata *hd) ;

#elif defined(KERNEL_2_1)

extern unsigned	lis_poll_2_1(struct file *fp, poll_table * wait);

#else
#error "Either KERNEL_2_0 or KERNEL_2_1 needs to be defined"
#endif

/*
 * bzero and bcopy
 */
#define	bzero(addr,nbytes)	memset(addr, 0, nbytes)
#define	bcopy(src,dst,n)	memcpy(dst,src,n)

/*  -------------------------------------------------------------------  */
/*
 *  d_count is atomic in 2.4 kernels and int in earlier ones
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,101)
#define	D_COUNT(d)	((d) ? atomic_read((atomic_t *)&((d)->d_count)) : -1)
#else
#define D_COUNT(d)  ((d) ? d_count(d) : -1)
#endif
#define FILE_D_COUNT(f) D_COUNT((f)->f_dentry)
/*  -------------------------------------------------------------------  */

#include <linux/mount.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,0,7)

#define	MNT_COUNT(m)	atomic_read(&((m)->mnt_count))

#else  /* 3.0 kernel, so test for which to read */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)

#ifdef CONFIG_SMP
#define	MNT_COUNT(m)	atomic_read(&((m)->mnt_longterm))
#else
#define	MNT_COUNT(m)	atomic_read(&((m)->mnt_count))
#endif  
#else    /* For 3.10 and above, count is not used any more */
#define MNT_COUNT(m)   D_COUNT(m->mnt_root)  
#endif  /* test for 3.10 kernel */

#endif  /* change needed for mnt_count change in 3.0 kernel */

#define FILE_MNT(f)	((f) ? (f)->f_vfsmnt : (struct vfsmount *)NULL)
#define FILE_MNTGET(f)  MNTGET(FILE_MNT((f)))
#define FILE_MNTPUT(f)  MNTPUT(FILE_MNT((f)))


extern struct vfsmount *lis_mnt;
extern lis_atomic_t     lis_mnt_cnt;


static inline
void lis_mnt_cnt_sync(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
    if (lis_mnt)
	K_ATOMIC_SET(&lis_mnt_cnt,MNT_COUNT(lis_mnt));
#endif
}

static inline
struct vfsmount *lis_mntget(struct vfsmount *m)
{
    struct vfsmount *mm = (m ? mntget(m) : NULL) ;


    lis_mnt_cnt_sync();


    return(mm) ;
}

static inline
void lis_mntput(struct vfsmount *m)
{
    if (m == NULL)
	return;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
    if (MNT_COUNT(m) > 0)
          mntput(m) ;
#else
    if (may_umount(m))
        mntput(m);
#endif
    
    lis_mnt_cnt_sync() ;

}

#if defined(CONFIG_DEV)
extern void
lis_mnt_cnt_sync_fcn(const char *file, int line, const char *fn);

extern struct vfsmount *
lis_mntget_fcn(struct vfsmount *m, 
	       const char *file, int line, const char *fn);

extern void
lis_mntput_fcn(struct vfsmount *m, 
	       const char *file, int line, const char *fn);

#define MNTSYNC()      lis_mnt_cnt_sync_fcn(__LIS_FILE__,__LINE__,__FUNCTION__)
#define	MNTGET(m)      lis_mntget_fcn((m),__LIS_FILE__,__LINE__,__FUNCTION__)
#define	MNTPUT(m)      lis_mntput_fcn((m),__LIS_FILE__,__LINE__,__FUNCTION__)
#else
#define MNTSYNC()      lis_mnt_cnt_sync()
#define MNTGET(m)      lis_mntget((m))
#define MNTPUT(m)      lis_mntput((m))
#endif  /* CONFIG_DEV  */

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*
 * These are externs for functions defined in the liskmod.c module.
 *
 * If the kernel is of an advanced enough version then these are
 * unnecessary since they will be inlines in a .h file or at least
 * there will be a standard extern for them in a kernel .h.
 *
 * When these externs are enabled the the liskmod module must be
 * loaded prior to streams.o in order for these symbols to be
 * resolved.
 */

#ifdef __KERNEL__

# if (   LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)	\
      && LINUX_VERSION_CODE <  KERNEL_VERSION(2,2,18))	\
  ||							\
     (   LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)	\
      && LINUX_VERSION_CODE <  KERNEL_VERSION(2,4,1))

extern void	put_unused_fd(unsigned int fd) ;

# endif

/*
 * The following version testing is only approximately correct.
 * I know that 2.2.5 does not have "igrab" and that 2.2.14
 * does.
 */
# if (   LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)	\
      && LINUX_VERSION_CODE <  KERNEL_VERSION(2,2,14))	\

extern struct inode *igrab(struct inode *inode) ;

# endif

/*
 * For convenience, define FATTACH_VIA_MOUNT if appropriate
 */
#if defined(KERNEL_2_4_7)
#define FATTACH_VIA_MOUNT 1
#endif


#endif				/* __KERNEL__ */

#ifdef __KERNEL__

#if defined(USE_KMEM_CACHE)

#if defined(CONFIG_DEV)
#define allochdr(a,b,c) lis_kmem_cache_allochdr(a)
#else
#define allochdr(a) lis_kmem_cache_allochdr(a)
#endif

#define freehdr(a) lis_msgb_cache_freehdr((a))
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,22)
extern kmem_cache_t *lis_msgb_cachep;
extern kmem_cache_t *lis_queue_cachep;
extern kmem_cache_t *lis_qsync_cachep;
extern kmem_cache_t *lis_qband_cachep;
extern kmem_cache_t *lis_head_cachep;
extern kmem_cache_t *lis_locks_cachep;
#else
extern struct kmem_cache *lis_msgb_cachep;
extern struct kmem_cache *lis_queue_cachep;
extern struct kmem_cache *lis_qsync_cachep;
extern struct kmem_cache *lis_qband_cachep;
extern struct kmem_cache *lis_head_cachep;
extern struct kmem_cache *lis_locks_cachep;
#endif
extern struct mdbblock *lis_kmem_cache_allochdr(unsigned int);
extern void lis_msgb_cache_freehdr(void *);
extern void lis_terminate_msg(void);	/* in linux-mdep.c */
extern void lis_init_queues(void);
extern void lis_terminate_queues(void);
extern void lis_init_locks(void);
extern void lis_terminate_locks(void);
#endif				/* USE_KMEM_CACHE */


#endif				/* __KERNEL__ */

#endif /*!__LIS_M_DEP_H*/


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
