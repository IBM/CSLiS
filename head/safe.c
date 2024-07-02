/*                               -*- Mode: C -*- 
 * safe.c --- stream safe
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: safe.c,v 1.1.1.1 2005/04/12 20:27:05 ragnar Exp $
 * Purpose         : stream safe processing stuff
 * ----------------______________________________________________
 *
 *  Copyright (C) 1995  Graham Wheeler
 *  Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
 *    gram@aztec.co.za
 *    dave@gcom.com
 */

#ident "@(#) CSLiS safe.c 7.111 2024-05-07 12:30:00 "


/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/stream.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,14,0)
#define _LINUX_IF_H
#define	IFNAMSIZ	16
#define __iovec_defined 1
#endif
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

#define	LOG(fil, line, msg)	printk("%s: called from file %s #%d\n",  \
					msg, fil, line)


/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */



void _RP lis_safe_noenable(queue_t *q, char *f, int l)
{
    lis_flags_t     psw;
    if (!LIS_QMAGIC(q,f,l)) return ;
    LIS_QISRLOCK(q, &psw) ;
    q->q_flag |= QNOENB;
    LIS_QISRUNLOCK(q, &psw) ;
}

void _RP lis_safe_enableok(queue_t *q, char *f, int l)
{
    lis_flags_t     psw;
    if (!LIS_QMAGIC(q,f,l)) return ;
    LIS_QISRLOCK(q, &psw) ;
    q->q_flag &= ~QNOENB;
    LIS_QISRUNLOCK(q, &psw) ;
}

int _RP lis_safe_canenable(queue_t *q, char *f, int l)
{
    if (LIS_QMAGIC(q,f,l))
	return !(q->q_flag & QNOENB);

    return 0;
}

queue_t * _RP lis_safe_OTHERQ(queue_t *q, char *f, int l)
{
    queue_t	*oq = NULL ;

    if (LIS_QMAGIC(q,f,l))
    {
	oq = q->q_other;
	if (LIS_QMAGIC(oq,f,l))
	    return (oq) ;
    }

    return NULL;
}

queue_t * _RP lis_safe_RD(queue_t *q, char *f, int l)
{
    queue_t	*oq = NULL ;

    if (LIS_QMAGIC(q,f,l))
    {
	if ((q->q_flag&QREADR))
	    oq = q ;
	else
	    oq = q->q_other;

	if (LIS_QMAGIC(oq,f,l))
	    return (oq) ;

    }
    return NULL;
}

queue_t * _RP lis_safe_WR(queue_t *q, char *f, int l)
{
    queue_t	*oq = NULL ;

    if (LIS_QMAGIC(q,f,l))
    {
	if ((q->q_flag&QREADR))
	    oq = q->q_other;
	else
	    oq = q ;

	if (LIS_QMAGIC(oq,f,l))
	    return (oq) ;
    }

    return NULL;
}

int  _RP lis_safe_SAMESTR(queue_t *q, char *f, int l)
{
    if (   LIS_QMAGIC(q,f,l)
	&& q->q_next != NULL
	&& LIS_QMAGIC((q->q_next),f,l)
       )
	return ((q->q_flag&QREADR) == (q->q_next->q_flag&QREADR));

    return 0;
}



int lis_safe_do_putmsg(queue_t *q, mblk_t *mp, ulong qflg, int retry,
		       char *f, int l)
{
    lis_flags_t     psw;
    int             ta_count;  /* Try Again Count to protect from looping */       

    qflg |= QOPENING ;
    ta_count = 0;

try_again:
    LIS_QISRLOCK(q, &psw) ;   /* On s390x platform, the Locks needed before test */
    if (   mp == NULL || q == NULL
	|| q->q_magic != Q_MAGIC
	|| q->q_qinfo == NULL
	|| q->q_qinfo->qi_putp == NULL
       )
    {
        LIS_QISRUNLOCK(q, &psw) ;
	LOG(f, l, "NULL q, mp, q_qinfo or qi_putp in putmsg");
	if (mp != NULL)
           freemsg(mp) ;
	return(1) ;		/* message consumed */
    }

    if (q->q_flag & (QPROCSOFF | QFROZEN))
    {
        ta_count++;  /* count how many times checking queue */
	if ((q->q_flag & QPROCSOFF) && SAMESTR(q))   /* there is a next queue */
	{					/* pass to next queue */
	    LIS_QISRUNLOCK(q, &psw) ;
            if (ta_count < 1000) { /* if shutting down, but queues are busy, prevent loop */
	       q = q->q_next ;
	       goto try_again ;
            }
	}

	lis_defer_msg(q, mp, retry, &psw) ;	/* put in deferred msg list */
	LIS_QISRUNLOCK(q, &psw) ;
	return(0) ;		/* msg deferred */
    }

    if ((q->q_flag & qflg) || q->q_defer_head != NULL)
    {
	lis_defer_msg(q, mp, retry, &psw) ;
	LIS_QISRUNLOCK(q, &psw) ;
	return(0) ;		/* msg deferred */
    }
    LIS_QISRUNLOCK(q, &psw) ;

    /*
     * Now we are going to call the put procedure
     */
    if (lis_lockqf(q, f,l) < 0)
    {
	freemsg(mp) ;		/* busted */
	return(1) ;
    }

    (*(q->q_qinfo->qi_putp))(q, mp);

    lis_unlockqf(q, f,l) ;
    return(1) ;			/* msg consumed */
}

void _RP lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l)
{
    lis_safe_do_putmsg(q, mp, (QDEFERRING | QOPENING), 0, f, l) ;
}

void  _RP lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l)
{
    queue_t    *qnxt = NULL ;

    if (   mp == NULL
	|| !LIS_QMAGIC(q,f,l)
	|| !LIS_QMAGIC((qnxt = q->q_next),f,l)
       )
    {
	LOG(f, l, "NULL q, mp or q_next in putnext");
	freemsg(mp) ;
	return ;
    }

    if (LIS_DEBUG_TRCE_MSG)
	lis_mark_mem(mp, lis_queue_name(qnxt), MEM_MSG) ;

    if (LIS_DEBUG_PUTNEXT)
    {
	printk("putnext: %s from \"%s\" to \"%s\" size %d\n",
			    lis_msg_type_name(mp),
			    lis_queue_name(q), lis_queue_name(qnxt),
			    lis_msgsize(mp)
	      ) ;

	if (LIS_DEBUG_ADDRS)
	    printk("        q=%lx, mp=%lx, mp->b_rptr=%lx, wptr=%lx\n",
		   (long) q, (long) mp, (long) mp->b_rptr, (long) mp->b_wptr);
    }

    lis_safe_putmsg(qnxt, mp, f, l) ;
}

void  _RP lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l)
{
    if (mp == NULL)
    {
	LOG(f, l, "NULL msg in qreply");
	return ;
    }

    if ((q = LIS_OTHERQ(q)) == NULL)
    {
	LOG(f, l, "NULL OTHERQ(q) in qreply");
	freemsg(mp) ;
	return ;
    }

    lis_safe_putnext(q, mp, f, l);
}



/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
