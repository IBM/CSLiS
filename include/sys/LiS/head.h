/*                               -*- Mode: C -*- 
 * head.h --- streams head handling
 * Author          : Graham Wheeler, Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: head.h,v 1.2 2006/05/18 17:06:07 steve Exp $
 * Purpose         : here you have utilites to handle str heads.
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros,
 *                        Denis Froschauer
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
 *    gram@aztec.co.za, nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 *    dave@gcom.com
 */

#ifndef _HEAD_H
#define _HEAD_H 1
#ifdef __KERNEL__

#ident "@(#) CSLiS head.h 7.11 2022-10-26 15:30:00 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifndef _STRPORT_H
#include <sys/strport.h>	/* porting environment tailoring */
#endif
#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* streams config symbols are here */
#endif
#ifndef _STR_MSG_H
#include <sys/strmsg.h>		/* streams msgs */
#endif
#ifndef _STR_OPTS_H
#include <sys/stropts.h>	/* streams operations */
#endif
#ifndef _MSG_H
#include <sys/LiS/msg.h>	/* streams msg symbols & types */
#endif
#ifndef _MSGUTL_H
#include <sys/LiS/msgutl.h>	/* streams msg utilities  */
#endif
#ifndef _QUEUE_H
#include <sys/LiS/queue.h>	/* streams queue symbols & types */
#endif
#ifndef _EVENTS_H
#include <sys/LiS/events.h>	/* streams events  */
#endif
#ifndef _MOD_H
#include <sys/LiS/mod.h>	/* streams module symbols & types */
#endif
#ifndef SYS_LISLOCKS_H
#include <sys/lislocks.h>	/* lis_semaphore_t, etc */
#endif
#include <sys/dki.h>		/* toid_t */
#if !defined(KERNEL_2_1)
# ifndef _POLL_H
# include <sys/LiS/poll.h>	/* streams module symbols & types */
# endif
#endif

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/* Close time max.
 */
#define LIS_MAX_CLTIME	1000*60*5	/* in m.seconds -> 5min */

/* Link time, default & max.
 */
#define LIS_MAX_LNTIME  1000*60*5
#define LIS_LNTIME	1000*15

/* str head default high/low water marks
 */
#define STRHIGH 5120
#define STRLOW  1024

/* str head flags 
 */
#define STWOPEN		0x00000001L /* someone's opening this */
#define	STFIFO		0x00000002L /* FIFO stream (or pipe end)? */
#define	STCONNLD	0x00000004L /* 'connld' pushed? */
#define	IOCWAIT		0x00000008L /* someone wants to do ioctl */
#define	STRCLOSE	0x00000010L /* stream is being closed */
#define	STROSEM_HELD	0x00000020L /* sd_opening semaphore is held */
#define STRHOLD		0x00000040L /* use stream hold on write feature */
#define STRATTACH       0x00000080L /* stream has been fattach'ed */
#define STREOPEN        0x00000100L /* stream can be reopened (uses sd_from) */
#define	STPLEX		0x00001000L /* multiplexed stream */
#define	STRISTTY	0x00002000L /* stream is a terminal */
#define	STRTOSTOP	0x00004000L /* block background writes */
#define	STRSIGPIPE	0x00008000L /* send SIGPIPE on write errors */
#define	STRPRI		0x00010000L /* An M_PCPROTO is at stream head */
#define	STRDERR		0x00020000L /* fatal read error from M_ERROR */
#define	STWRERR		0x00040000L /* fatal write error from M_ERROR */
#define	STRHUP		0x00080000L /* Device has vanished */
#define	SNDMREAD	0x00100000L /* used for read notification */
#define	OLDNDELAY	0x00200000L /* use old TTY semantics for */
				    /* NDELAY reads and writes */
#define	STRDELIM	0x00400000L /* generate delimited messages ???*/
#define	STRSELPND	0x00800000L /* select waiting on this stream */
#define STIOCTMR	0x01000000L /* ioctl timer in progress */
#define STRCLOSEWT	0x02000000L /* waiting on sd_close_wt sem */
#define STRFLUSHWT	0x04000000L /* waiting on sd_close_wt sem for flush */
#define STRFROZEN	0x08000000L /* successful freezestr() performed */

#ifndef VERIFY_READ  /* RHEL_RELEASE_CODE >= 2049 or Distro > 4.18.0, type not included */
#define VERIFY_READ 0           /* argument for lis_check_umem */
#endif
#ifndef VERIFY_WRITE    
#define VERIFY_WRITE 1          /* argument for lis_check_umem */
#endif

/*  -------------------------------------------------------------------  */

/* Should be in head->magic, it's used to cach 254/255 ;) 
 * dangling references to a stream heads in critical places
 */
#define STDATA_MAGIC (0x11110000L | sizeof(stdata_t))



/*  -------------------------------------------------------------------  */
/*				    Types                                */

/*
 * This structure is embedded in the stdata struct.  It is used for
 * maintaining the topology of multiplexors.  When the user does
 * an ioctl(fd, I_LINK, muxfd), the stream represented by muxfd
 * is linked "below" the stream driver represented by fd.  The
 * stdata struct of fd uses mx_hd as a list head to thread together
 * all such streams that were linked below it.  The mx_next field
 * carries the links.
 *
 * The muxfd stream is marked as belonging to a multiplexor and
 * cannot be operated on with file I/O routines, except close.
 *
 * The muxfd stream has its refcnt incremented so that if it is
 * closed it will not be deallocated.
 *
 * When it is time to unlink the stream it is removed from its list.
 * The refcnt is then decremented and if it reaches zero the stream
 * is deallocated.
 *
 * If you close the stream that heads a list (a so-called "control stream")
 * then its list is traversed and each stream on the list is unlinked.
 *
 * The I_LINK ioctl returns a reference number, l_index, that is used
 * by the user to unlink the stream with ioctl(fd, I_UNLINK, l_index).
 * The list of streams headed by fd is scanned and the one with the
 * mx_index equal to l_index is unlinked.
 */
typedef 
struct stmux
{
    int            mx_cmd;	/* I_LINK or I_PLINK */
    int		   mx_index;	/* l_index for this mux */
    struct stdata *mx_hd;	/* list head for control stream */
    struct stdata *mx_next;	/* list threaded through here */

} stmux_t;


/*
 *  The stdata struct. This is the main stream structure and is pointed
 *  to by file pointers for the stream. It corresponds to the STREAM head.
 */

#if defined(KERNEL_2_1)

typedef struct
{
    uid_t	uid ;
    uid_t	euid ;
    uid_t	suid ;
    uid_t	fsuid ;
    gid_t	gid ;
    gid_t	egid ;
    gid_t	sgid ;
    gid_t	fsgid ;
#if 0
    int		ngroups ;
    gid_t	groups[NGROUPS] ;
    kernel_cap_t cap_effective ;
    kernel_cap_t cap_inheritable ;
    kernel_cap_t cap_permitted ;
#endif

} lis_kcreds_t ;

#else

typedef struct
{
    int		dummy ;

} lis_kcreds_t ;

#endif

typedef
struct stdata
{
    	/*
	 * Frequently used fields are gathered at the beginning
	 * of the structure.
	 */
        long              magic; 	/* should be always STDATA_MAGIC */
	lis_atomic_t      sd_refcnt;	/* reference count */
	lis_spin_lock_t	  sd_lock ;	/* get exclusive use of strm head */
        struct queue     *sd_wq;        /* write queue */
	struct queue	 *sd_rq ;	/* RD(sd_wq) */
        volatile long	  sd_flag;      /* state/flags */
	mblk_t		 *sd_rput_hd;	/* head of msgs from strrput */
	mblk_t		 *sd_rput_tl;	/* tail of that list */
	mblk_t           *sd_wmsg;      /* buff for write msg */
        int		  sd_sigflags;  /* logical OR of all siglist events */
        int		  sd_events;    /* logical OR of all eventlist events*/
        int		  sd_rdopt;	/* read options */
        int		  sd_wropt;	/* write options */
        int		  sd_rerror;    /* read error to set u.u_error */
        int		  sd_werror;    /* write error to set u.u_error */
        long		  sd_maxpsz;    /* max pkt size --should be a cache*/
        long		  sd_minpsz;    /* min pkt size   of below module's */
	unsigned short    sd_wroff;	/* write offset for downstream data */
	lis_atomic_t	  sd_rdcnt ;	/* # users sleeping on read */
	lis_atomic_t	  sd_rdsemcnt ;	/* # users sleeping on read_sem */
	lis_atomic_t	  sd_wrcnt ;	/* # users sleeping on write */
        lis_semaphore_t   sd_wwrite;	/* wait to room for write */
        lis_semaphore_t   sd_write_sem;	/* single thread semaphore */
        lis_semaphore_t   sd_wread;	/* wait for msg to arrive */
        lis_semaphore_t   sd_read_sem;	/* single thread semaphore */

	/* End of frequently used fields */

	struct stdata	 *sd_next ;	/* all stdatas are linked together */
	struct stdata	 *sd_prev ;
        struct streamtab *sd_strtab;    /* pointer to streamtab for stream */
        struct stdata    *sd_peer;      /* other FIFO in a pipe */
	struct inode     *sd_inode;     /* corresponding inode */
#if defined(LINUX)
        struct dentry    *sd_from;      /* dentry->inode FIFO opened from */
#else
        struct inode     *sd_from;      /* inode a FIFO was opened from */
#endif
	struct file      *sd_file;      /* file being opened */
	mblk_t           *sd_iocblk; 	/* returned ioctl msg */
	int		  sd_open_flags ;/* flags that opened stream */
	toid_t		  sd_scantimer;	/* scanq timer handle */
	int		  sd_iocseq;	/* ioc seq # */
        int		  sd_session;   /* controlling session id */
        int		  sd_pgrp;      /* controlling process group id */
        int		  sd_closetime; /* time to wait to drain q in close */
        toid_t		  sd_close_timer;/* timer handle for close timer */
	int		  sd_maxpushcnt; /* currently there's no limit in 
					  * # of pushed mods, but let's set up
					  * an artificial one to aid debugging
					  */
	struct strevent   *sd_siglist;  /* processes to be sent SIGPOLL */
#if defined(PORTABLE_POLL)
	struct pollhead    sd_polllist;	/* polling processes*/
#elif defined(LINUX_POLL)
	wait_queue_head_t  sd_task_list;/* tasks waiting on poll */
#else
#error "Either PORTABLE_POLL or LINUX_POLL must be defined"
#endif
	lis_semaphore_t	   sd_opening;	/* stream is opening */
	lis_semaphore_t    sd_close_wt;	/* Waiting for close to complete*/
        lis_semaphore_t    sd_wioc;     /* wait for ioctl */
	lis_semaphore_t    sd_wiocing;  /* wait for iocl response */
	lis_semaphore_t    sd_closing;	/* waiting to close */
	lis_atomic_t       sd_opencnt;	/* # of files open to this stream */
	unsigned short     sd_linkcnt;	/* # of I_PLINKs done via stream */
        unsigned short     sd_pushcnt;  /* number of pushes done on stream */
        unsigned short     sd_rfdcnt;   /* number of M_PASSFP's queued */   
        struct stmux       sd_mux;      /* info for muxing streams */
        int		   sd_l_index;	/* muxid cntr for stream head */
        dev_t		   sd_dev ;	/* major/minor from inode */
#ifdef PORTABLE_POLL
	lis_select_t	   sd_select ;	/* abstract select structure */
					/* see *-mdep.h */
#endif
	unsigned long	   sd_save_sigs[8] ;	/* opaque mem area */
	char		   sd_name[32] ;	/* name of stream */
	lis_kcreds_t	   sd_kcreds ;	/* creds of stream opener */
	cred_t		   sd_creds ;	/* portable form of creds */
        void              *sd_mount ;   /* mount point (only for sd_from) */
        lis_atomic_t       sd_fattachcnt; /* number of fattaches */
} stdata_t;

#define	LIS_SD_REFCNT(hd)  K_ATOMIC_READ(&(hd)->sd_refcnt)
#define	LIS_SD_OPENCNT(hd) K_ATOMIC_READ(&(hd)->sd_opencnt)

#define SET_SD_FLAG(hd,msk)						\
		do {							\
		    lis_flags_t psw;					\
		    lis_spin_lock_irqsave(&(hd)->sd_lock,&psw) ;	\
		    (hd)->sd_flag |= (msk) ;				\
		    lis_spin_unlock_irqrestore(&(hd)->sd_lock,&psw) ;	\
		   }							\
		while (0)

#define CLR_SD_FLAG(hd,msk)						\
		do {							\
		    lis_flags_t psw;					\
		    lis_spin_lock_irqsave(&(hd)->sd_lock,&psw) ;	\
		    (hd)->sd_flag &= ~(msk) ;				\
		    lis_spin_unlock_irqrestore(&(hd)->sd_lock,&psw) ;	\
		   }							\
		while (0)


extern stdata_t		*lis_stdata_head ;	/* to list of stdatas */
extern lis_atomic_t      lis_stdata_cnt;

extern lis_semaphore_t   lis_stdata_sem;        /* for stdata list locking */

extern lis_atomic_t      lis_open_cnt;
extern lis_atomic_t      lis_close_cnt;

extern lis_atomic_t      lis_inode_cnt;
extern lis_atomic_t      lis_mnt_cnt;

extern void    		 lis_task_to_creds(lis_kcreds_t *cp) ;
extern void    		 lis_creds_to_task(lis_kcreds_t *cp) ;

#if defined(CONFIG_DEV)
extern stdata_t *lis_head_get_fcn(stdata_t *hd, const char *file, int line);
extern stdata_t *lis_head_put_fcn(stdata_t *hd, const char *file, int line);

#define lis_head_get(hd)	lis_head_get_fcn(hd, __LIS_FILE__, __LINE__)
#define lis_head_put(hd)	lis_head_put_fcn(hd, __LIS_FILE__, __LINE__)
#else
extern stdata_t *lis_head_get_fcn(stdata_t *hd);
extern stdata_t *lis_head_put_fcn(stdata_t *hd);

#define lis_head_get(hd)	lis_head_get_fcn(hd)
#define lis_head_put(hd)	lis_head_put_fcn(hd)
#endif




/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/* Scheduling  & scan list
 */
extern volatile queue_t	*lis_qhead; /* first scheduled queue	*/
extern volatile queue_t	*lis_qtail; /* last scheduled queue		*/
extern volatile queue_t	*lis_scanqhead;	/* head of STREAMS scan q */
extern volatile queue_t	*lis_scanqtail;	/* tail of STREAMS scan q */
extern char lis_qrunflag;	/*set if there is at least one enabled queue*/
extern char lis_queueflag;	/*the function queuerun is running    	*/

/* cfg. opts.
 */
extern int lis_nstrpush;	/* maximum # of pushed modules */
extern int lis_strhold;		/* if not zero str hold feature's activated*/
extern unsigned long lis_strthresh;	/* configurable STREAMS memory limit */
extern int lis_iocseq; 		/* ioctl id */


/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */


/* The streams file ops
 * these are the entry points into the LiS subsystem.
 * in response to a system call the Linux kernel will dispatch to
 * one of these entries.
 * -- see "The design of the unix operating system" (Bach)
 */
extern int lis_stropen( struct inode *, struct file *);
extern int lis_strioctl(struct inode *, struct file *,
			unsigned int, unsigned long);
extern int lis_strputpmsg(struct inode *, struct file *,
			void *, void *, int, int);
extern int lis_strgetpmsg(struct inode *, struct file *,
			void *, void *, int *,int *,int);
extern int lis_strpoll(struct inode *i, struct file *f, void *ptr) ;
extern int lis_strclose(struct inode *i, struct file *f);
extern ssize_t lis_strwrite(struct file *, const char *,  size_t, loff_t *);
extern ssize_t lis_strread(struct file *, char *,  size_t, loff_t *);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13)
extern int lis_ioctl32_str(struct file *, unsigned int, unsigned long);
#else
extern int lis_ioctl32_str(unsigned int, unsigned int, unsigned long);
#endif

/* Initialize some glob vars...
 */
extern void
lis_init_head(void);

extern void
lis_terminate_head(void);

/*
 * Run the STREAMS queues
 */
extern void lis_run_queues(int cpu);

/*
 *  for stream head qinit structures
 */
extern int _RP
lis_strrput(queue_t *, mblk_t *);
extern int _RP
lis_strwsrv(queue_t *);
extern int _RP
lis_strrsrv(queue_t *);

/*
 *  I_SENFD/I_RECVFD ioctl support
 */
extern int  lis_sendfd(stdata_t *, unsigned int, struct file *);
extern int  lis_recvfd(stdata_t *, strrecvfd_t *, struct file *);
extern void lis_free_passfp(mblk_t *);

/*
 *  these are needed by or associated with fdetach()
 */
extern stdata_t *lookup_stdata(dev_t *, struct inode *, stdata_t *);
extern int lis_doclose(struct inode *, struct file *f, stdata_t *, cred_t *);
extern void lis_fdetach_stream(stdata_t *);

/*
 *  streamtabs needed internally
 */
extern struct streamtab fifo_info;
extern struct streamtab clone_info;

/*
 * Routines to save and restore signal masks.  Actual routines are
 * located in the mdep.c files for the target platform.
 */
extern void lis_clear_and_save_sigs(stdata_t *hd);
extern void lis_restore_sigs(stdata_t *hd);

/*
 * Return the major device number of the LiS clone device.  Useful for
 * dynamically loading drivers to synthesis clone major/minors.
 */
extern int lis_clone_major(void) _RP;

/*
 * Set an errno in the stream head and wake up relevant processes
 */
void	lis_stream_error(stdata_t *shead, int rderr, int wrerr) ;

/*
 *  timing functions - these are available outside the kernel also
 */
extern unsigned long lis_hitime(void)_RP;  /* usec res; 64s cycle */
extern unsigned long lis_usecs(void)_RP;   /* usec res; ulong cycle */
extern unsigned long lis_msecs(void)_RP;   /* msec res; ulong cycle */
extern unsigned long lis_dsecs(void)_RP;   /* 1/10sec res; ulong cycle */
extern unsigned long lis_secs(void)_RP;    /* sec res; ulong cycle */

/*  -------------------------------------------------------------------  */
#endif /* __KERNEL__ */
#endif /*!_HEAD_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
