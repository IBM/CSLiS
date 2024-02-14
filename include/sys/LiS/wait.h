/*                               -*- Mode: C -*- 
 * wait.h --- waiting for events...
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: wait.h,v 1.1.1.1 2005/04/12 20:27:06 ragnar Exp $
 * Purpose         : provide user waitentials
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Francisco J. Ballesteros
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
 *    You can reach me by email to
 *    nemo@ordago.uc3m.es
 */


#ifndef _WAIT_H
#define _WAIT_H 1

#ident "@(#) CSLiS wait.h 7.11 2022-10-26 15:30:00 "

#ifdef __KERNEL__
/*  -------------------------------------------------------------------  */
/*                               Dependencies                            */

#ifndef _HEAD_H
#include <sys/LiS/head.h>	/* stream head */
#endif


/*  -------------------------------------------------------------------  */
/*                         Shared global variables                       */

#define ONESEC (1000000/HZ)	/* ? */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */


/* The functions below are to achieve mutual exclusion on the stream
 * head. They use the sd_wopen wait queue to sleep and the sd_lock char to
 * flag the lock.
 * The lock/unlock are the complete ones.
 * if you just want to wait you may use wait_on/wake_up.
 *
 * You can lock(L)/wait(W)/sleep(S) on the following events:
 * L/S wopen:  the 1st opener completes its job
 * W/S wwrite: there's room in the write queue to issue a downstream msg.
 * W/S wread: msg arrives at the stream head.
 * L/S wioc:  process completes it's ioctl()
 *   S wiocing: answer to ioc arrived
 * STATUS: complete(?), untested
 */
extern int lis_sleep_on_wopen(struct stdata * sd) ;
extern int lis_sleep_on_wioc(struct stdata * sd, char *f,int l) ;
extern int lis_sleep_on_wwrite(struct stdata * sd);
extern int lis_sleep_on_wread(struct stdata * sd);
extern int lis_sleep_on_wiocing(struct stdata * sd);
extern int lis_sleep_on_read_sem(struct stdata * sd);


/*  -------------------------------------------------------------------  */

#define lis_wait_on_wwrite(sd)	lis_sleep_on_wwrite(sd)

#define lis_wait_on_wread(sd)	lis_sleep_on_wread(sd)

#define lis_lock_wioc(sd)	lis_sleep_on_wioc(sd,__FILE__,__LINE__)

/*  -------------------------------------------------------------------  */

extern void	lis_wake_up_wwrite(struct stdata * sd) ;     /* wait.c */
extern void	lis_wake_up_all_wwrite(struct stdata * sd) ; /* wait.c */
extern void	lis_wake_up_wread(struct stdata * sd) ;      /* wait.c */
extern void	lis_wake_up_all_wread(struct stdata * sd) ;  /* wait.c */
extern void	lis_wake_up_read_sem(struct stdata * sd);    /* wait.c */
extern void	lis_wake_up_all_read_sem(struct stdata * sd);    /* wait.c */

#define lis_wake_up_wiocing(sd)						\
				{					\
				    lis_up(&sd->sd_wiocing);		\
				}


/*  -------------------------------------------------------------------  */
#define lis_unlock_wioc(sd)						\
				{					\
				    lis_up(&sd->sd_wioc);		\
				}


/*  -------------------------------------------------------------------  */

/* Sched & cancel timeouts
 */
/*****************************************************/
/*  In LINUX LEVEL 4.15.0, the timer_list removed    */
/*  the parameter "data", unsigned long.             */
/*  To pass a 4 byte argument, need a new structure  */
/*  type to pass the arg that is tied to a timer_list*/
/*****************************************************/
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,14,0)
typedef struct lis_timer_list { 
    struct timer_list tl;
    unsigned long arg;
} lis_timer_list;

typedef void    tmout_fcn_t(struct timer_list *) ;

extern void
lis_tmout(struct lis_timer_list *lis_tl, tmout_fcn_t *fn, long arg, long ticks);
#else
typedef void    tmout_fcn_t(ulong) ;
extern void
lis_tmout(struct timer_list *tl, tmout_fcn_t *fn, long arg, long ticks) ;
#endif

extern void
lis_untmout( struct timer_list *tl) ;

#endif /* __KERNEL__ */

#endif /*!_WAIT_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
