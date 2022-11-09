/*                               -*- Mode: C -*- 
 * <stropts> --- STREAMS ops.
 * Author          : gram & nemo
 * Created On      : Mon Oct 17 11:37:12 1994
 * RCS Id          ; $Id: stropts.h,v 1.2 2006/05/18 17:06:07 steve Exp $
 * Last Modified By: David Grothe
 *                 :
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

#ifndef _SYS_STROPTS_H
#define _SYS_STROPTS_H
#define	_LIS_SYS_STROPTS_H	/* so you can tell which stropts.h you got */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * There is now a GNU stropts.h in /usr/include and /usr/include/bits.
 * We are attempting to maintain compatibility of constant values among
 * that file, LiS, Solaris and UnixWare.  Nonetheless, LiS must be compiled
 * with its own stropts.h file due to special LiS-only STREAMS ioctls
 * that are not defined in the other files.
 */

/*
 * If you want backward compatibility to LiS-2.6 and before, 
 * define the following before including this file.  With this
 * not defined we are compatible with Solaris and UnixWare in
 * our use of constant values.
 *
 * We define a compatibility variable that can be tested.
 */
#ifdef USE_OLD_CONSTS
#define	LIS_STROPTS_H_SOLARIS_UW_COMPATIBLE	0
#else
#define	LIS_STROPTS_H_SOLARIS_UW_COMPATIBLE	1
#endif

#ident "@(#) CSLiS stropts.h 7.11 10/20/22 15:30:00 "
/*  *******************************************************************  */
/*                               Dependencies                            */

#ifndef _SYS_TYPES_H
#if defined(LINUX) && defined(__KERNEL__)
#include <linux/types.h>
#else
#include <sys/types.h>
#endif
#endif
#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>
#endif


/*  *******************************************************************  */
/*                                 Symbols                               */

/*
 *  timeout for eternity
 */

#define INFTIM          -1
#define	LIS_DFLT_TIM	15		/* Streams Programmer's Guide
					 * hints that this is the
					 * value to use.
					 */

/*  *******************************************************************  */

/*
 * Write opts.
 */

#define SNDZERO         0x001           /* send a zero length message */
#define SNDPIPE         0x002           /* send SIGPIPE on write and */
                                        /* putmsg if sd_werror is set */

/*
 *  Read opts that can be defined with SO_READOPT (so_readopt values)
 */

#define RNORM           0x0000  /* byte stream */
#define RMSGD           0x0001  /* message discard */
#define RMSGN           0x0002  /* message non-discard */

#define RMODEMASK       0x0003  /* RMODE bits */

/* As Solaris' guys say:
 * "These next three read options are added for the sake of
 * user-level transparency.  RPROTDAT will cause the stream head
 * to treat the contents of M_PROTO and M_PCPROTO message blocks
 * as data.  RPROTDIS will prevent the stream head from failing
 * a read with EBADMSG if an M_PROTO or M_PCPROTO message is on
 * the front of the stream head read queue.  Rather, the protocol
 * blocks will be silently discarded and the data associated with
 * the message (in linked M_DATA blocks), if any, will be delivered
 * to the user.  RPROTNORM sets the default behavior, where read
 * will fail with EBADMSG if an M_PROTO or M_PCPROTO are at the
 * stream head."
 *
 * For compatibility, we use the same bits as SVR4.
 */

#define RPROTDAT        0x0004  /* turn M*PROTO into M_DATA */
#define RPROTDIS        0x0008  /* discard M*PROTO */
#define RPROTNORM       0x0010  /* normal proto */

#define RPROTMASK       0x001C  /* RPROT bits */

/*  *******************************************************************  */

/*
 *  flags for M_FLUSH
 */

#define FLUSHR          0x01    /* flush read side */
#define FLUSHW          0x02    /* flush write side */
#define FLUSHRW         0x03    /* flush both read & write sides */
#ifdef USE_OLD_CONSTS
#define FLUSHBAND       0x10    /* flush msg for band priority */
#else
#define FLUSHBAND       0x04    /* flush msg for band priority */
#endif

/*  *******************************************************************  */

/*
 *  events for SIGPOLL to be sent
 */

#define S_INPUT         0x0001          /* any msg but hipri on read Q */
#define S_HIPRI         0x0002          /* high priority msg on read Q */
#define S_OUTPUT        0x0004          /* write Q no longer full */
#define S_MSG           0x0008          /* signal msg at front of read Q */
#define S_ERROR         0x0010          /* error msg arrived at stream head */
#define S_HANGUP        0x0020          /* hangup msg arrived at stream head */
#define S_RDNORM        0x0040          /* normal msg on read Q */
#define S_WRNORM        S_OUTPUT
#define S_RDBAND        0x0080          /* out of band msg on read Q */
#define S_WRBAND        0x0100          /* can write out of band */
#define S_BANDURG       0x0200          /* modifier to S_RDBAND, to generate */
                                        /* SIGURG instead of SIGPOLL */
#define S_ALL		0x03FF		/* every event */

/*  *******************************************************************  */

/*
 * Stream buffer structure for putpmsg and getpmsg system calls
 */

typedef
struct strbuf {
    int     maxlen;                 /* no. of bytes in buffer */
    int     len;                    /* no. of bytes returned */
    char    *buf;                   /* pointer to data */
} strbuf_t;

/*  *******************************************************************  */

/*
 * User level routines for getmsg/putmsg, etc
 */
int	putpmsg(int fd, struct strbuf *ctlptr,
			struct strbuf *dataptr,
			int band,
			int flags) ;
int	getpmsg(int fd, struct strbuf *ctlptr,
			struct strbuf *dataptr,
			int *bandp,
			int *flagsp) ;
int	getmsg(int fd, void *ctlptr, void *dataptr, int *flagsp) ;
int	putmsg(int fd, void *ctlptr, void *dataptr, int flags) ;
int	fattach(int fd, const char *path) ;
int	fdetach(const char *path) ;
int	isastream(int fd) ;

/*
 * Flags for getmsg() and putmsg() syscall arguments.
 * A value of zero will cause getmsg() to return
 * the first message on the stream head read queue and putpmsg() to send
 * a normal priority message.
 */
#define RS_HIPRI        0x01    /* high priority message */
#define RS_NORM         0x00    /* normal message */

/*  *******************************************************************  */

/* Flags for getpmsg() and putpmsg() syscall arguments.
 * These are settable by the user and will be set on return
 * to indicate the priority of message received.
 */
#define MSG_HIPRI       0x01            /* send/recv high priority message */
#define MSG_ANY         0x02            /* recv any messages */
#define MSG_BAND        0x04            /* recv messages from specified band */

/*  *******************************************************************  */

/* Flags returned as value of getmsg() and getpmsg() syscall.
 */
#define MORECTL         1               /* more ctl info is left in message */
#define MOREDATA        2               /* more data is left in message */


/*  *******************************************************************  */

/*
 * Define to indicate that all multiplexors beneath a stream should
 * be unlinked.
 */

#define MUXID_ALL       (-1)

/*  *******************************************************************  */

/*
 *  Flag definitions for the I_ATMARK ioctl.
 */

#define ANYMARK         0x01
#define LASTMARK        0x02

/*  *******************************************************************  */

/*
 *   STREAMS Ioctls
 */

#if defined(STR)			/* possibly from <asm/hw_irq.h> */
#undef STR
#endif

#define __SID           (0xdc<<8)	/* for GNU libc compatibility */
#define STR             (0xdc<<8)	/* for UnixWare/Solaris compatibility */
#define I_NREAD         (__SID | 1)
#define I_PUSH          (__SID | 2)
#define I_POP           (__SID | 3)
#define I_LOOK          (__SID | 4)
#define I_FLUSH         (__SID | 5)
#define I_SRDOPT        (__SID | 6)
#define I_GRDOPT        (__SID | 7)
#define I_STR           (__SID | 8)
#define I_SETSIG        (__SID | 9)
#define I_GETSIG        (__SID |10)
#define I_FIND          (__SID |11)
#define I_LINK          (__SID |12)
#define I_UNLINK        (__SID |13)
#ifdef USE_OLD_CONSTS
#define I_PEEK          (__SID |14)
#define I_FDINSERT      (__SID |15)
#define I_SENDFD        (__SID |16)
#define I_RECVFD        (__SID |17)
#define I_SWROPT        (__SID |18)
#define I_GWROPT        (__SID |19)
#define I_LIST          (__SID |20)
#define I_PLINK         (__SID |21)
#define I_PUNLINK       (__SID |22)
#define I_FLUSHBAND     (__SID |27)
#define I_CKBAND        (__SID |28)
#define I_GETBAND       (__SID |29)
#define I_ATMARK        (__SID |30)
#define I_SETCLTIME     (__SID |31)
#define I_GETCLTIME     (__SID |32)
#define I_CANPUT        (__SID |33)
#else
#define I_PEEK          (__SID |15)
#define I_FDINSERT      (__SID |16)
#define I_SENDFD        (__SID |17)
#if defined(_KERNEL)			/* this gambit for Solaris/UW compat */
#define I_RECVFD        (__SID |18)
#define I_E_RECVFD      (__SID |14)
#elif !defined(_STYPES)
#define I_RECVFD        (__SID |14)
#else
#define I_RECVFD        (__SID |18)
#endif
#define I_SWROPT        (__SID |19)
#define I_GWROPT        (__SID |20)
#define I_LIST          (__SID |21)
#define I_PLINK         (__SID |22)
#define I_PUNLINK       (__SID |23)
#define I_FLUSHBAND     (__SID |28)
#define I_CKBAND        (__SID |29)
#define I_GETBAND       (__SID |30)
#define I_ATMARK        (__SID |31)
#define I_SETCLTIME     (__SID |32)
#define I_GETCLTIME     (__SID |33)
#define I_CANPUT        (__SID |34)
#endif
/*
 * The following ioctls are specific to this STREAMS implementation.
 */
#define I_LIS_GETMSG 		(__SID |237)	/* getmsg() */
#define I_LIS_PUTMSG 		(__SID |238)	/* putmsg() */
#define I_LIS_SEMTIME 		(__SID |239)	/* sem wakeup histogram */
#define I_LIS_LOCKS 		(__SID |240)	/* lock contention */
#define I_LIS_SDBGMSK2 		(__SID |241)	/* 2nd debug mask */
#define	I_LIS_QRUN_STATS	(__SID |242)	/* see qrun_stats_t */
#define I_LIS_PIPE              (__SID |243)    /* pipe() */
#define I_LIS_FATTACH           (__SID |244)    /* fattach() */
#define I_LIS_FDETACH           (__SID |245)    /* fdetach() */

#define	I_LIS_PRNTQUEUES 	(__SID |246)	/* print all the queues */
#define	I_LIS_PRNTSPL		(__SID |247)	/* print spl tracking table */
#define I_LIS_GET_MAXMSGMEM	(__SID |248)
#define I_LIS_SET_MAXMSGMEM	(__SID |249)
#define I_LIS_GET_MAXMEM	(__SID |250)
#define I_LIS_SET_MAXMEM	(__SID |251)
#define I_LIS_GETSTATS 		(__SID |252)   /* see include/sys/LiS/stats.h */
#define I_LIS_PRNTSTRM 		(__SID |253)
#define	I_LIS_PRNTMEM		(__SID |254)
#define I_LIS_SDBGMSK  		(__SID |255)

/*  -------------------------------------------------------------------  */
/*                                  Types                                */

/*
 * I_STR ioctl user data
 */

typedef
struct strioctl {
    int     ic_cmd;                 /* command */
    int     ic_timout;              /* timeout value */
    int     ic_len;                 /* length of data */
    char    *ic_dp;                 /* pointer to data */
} strioctl_t;

/*  *******************************************************************  */

/*
 *  I_PEEK ioctl
 */

typedef
struct strpeek {
    struct strbuf   ctlbuf;
    struct strbuf   databuf;
    long            flags;
} strpeek_t;

/*  *******************************************************************  */

/*
 * Stream I_FDINSERT ioctl format
 */

typedef
struct strfdinsert {
    struct strbuf   ctlbuf;
    struct strbuf   databuf;
    long            flags;
    int             fildes;
    int             offset;
} strfdinsert_t;

/*
 * Receive file descriptor structure
 */

typedef
struct strrecvfd {
#ifdef __KERNEL__
    union {
	struct file *fp;
	int          fd;
    } f;
#else
    int   fd;
#endif
    uid_t uid;
    gid_t gid;
#ifdef __KERNEL__
    struct {
	struct file *fp;
	struct stdata *hd;
    } r;
#else
    char  fill[8];
#endif
} strrecvfd_t;

/*  *******************************************************************  */

/*
 *  I_LIST ioctl.
 */

typedef
struct str_mlist {
    char l_name[FMNAMESZ+1];
} str_mlist_t;

typedef
struct str_list {
    int               sl_nmods;
    struct str_mlist *sl_modlist;
} str_list_t;

/*  *******************************************************************  */

/*
 * For I_FLUSHBAND ioctl.  Describes the priority
 * band for which the operation applies.
 */

typedef
struct bandinfo {
    unsigned char   bi_pri;
    int             bi_flag;
} bandinfo_t;

/*
 * For I_LIS_QRUN_STATS ioctl
 */
typedef struct
{
    int			num_cpus ;
    int			num_qrunners ;
    int			queues_running ;
    int			runq_req_cnt ;
    unsigned long	runq_cnts[LIS_NR_CPUS] ;
    unsigned long	queuerun_cnts[LIS_NR_CPUS] ;
    int			active_flags[LIS_NR_CPUS] ;
    int			runq_pids[LIS_NR_CPUS] ;
    unsigned long	runq_wakeups[LIS_NR_CPUS] ;
    unsigned long	setqsched_cnts[LIS_NR_CPUS] ;
    unsigned long	setqsched_isr_cnts[LIS_NR_CPUS] ;

} lis_qrun_stats_t ;

typedef struct getpmsg_args
{
    int		 	 fd ;
    struct strbuf	*ctl ;
    struct strbuf	*dat ;
    int			*bandp ;
    int			*flagsp ;

} getpmsg_args_t ;

typedef struct putpmsg_args
{
    int		 	 fd ;
    struct strbuf	*ctl ;
    struct strbuf	*dat ;
    int			 band ;
    int			 flags ;

} putpmsg_args_t ;


/*  *******************************************************************  */
/*                         Shared global variables                       */



/*  *******************************************************************  */

#ifdef __cplusplus
}
#endif

#endif /*!_SYS_STROPTS_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
