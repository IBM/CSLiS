/************************************************************************
*                      SVR4 Driver-Kernel Interface                     *
*************************************************************************
*									*
* These routines implement an SVR4 compatible DKI.			*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
* Copyright (C) 2006  The Software Group Limited                        *
*                                                                       *
* Copyright 2022 - IBM Inc. All rights reserved                         *
* SPDX-License-Identifier: LGPL-2.1                                     *
*									*
************************************************************************/
/*
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
 */

#ident "@(#) CSLiS dki.c 7.111 2024-05-07 15:30:00 "

#include <sys/stream.h>
#ifdef LIS_OBJNAME  /* This must be defined before including module.h on Linux 6.8 */
#define _LINUX_IF_H
#define IFNAMSIZ        16
#endif 
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,14,0)
#include <sys/osif.h>
#endif

lis_spin_lock_t	  lis_tlist_lock ;

/************************************************************************
*                       SVR4 Compatible timeout                         *
*************************************************************************
*									*
* This implementation of SVR4 compatible timeout functions uses the	*
* Linux timeout mechanism as the internals.				*
*									*
* The technique is to allocate memory that contains a Linux timer	*
* structure and an SVR4-like handle.  We build up a list of these	*
* structures in a linked-list headed by 'lis_tlist_head'.  We never	*
* use handle zero, so that value becomes a marker for an available	*
* entry.  We recycle the list elements as they expire or get untimeout-	*
* ed.									*
*									*
************************************************************************/

typedef struct tlist
{
    struct tlist	*next ;		/* thread of these */
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,14,0)
    struct lis_timer_list lis_tl ;          /* new timer list due to level 4.15 change */
#else
    struct timer_list	 tl ;		/* Linux timer structure */
#endif
    int			 handle ;	/* SVR4-style handle */
    timo_fcn_t		*fcn ;		/* function to call */
    caddr_t		 arg ;		/* function argument */

} tlist_t ;

#define THASH		757		/* prime number */

/*
 * head[0] is for free timer structures.  The others head linked
 * lists of timer structures whose handles hash to the same value.
 */
volatile tlist_t *lis_tlist_heads[THASH] ;
/*
 * The tlist handles roll over at 2^31 and thus are virtually never
 * repeated.  But we check whenever assigning a timer handle to ensure
 * that it is not already in use.  The hash table makes this easy.
 */
volatile int	  lis_tlist_handle ;	/* next handle to use */

/*
 * Timer list manipulations
 *
 * Caller has timer list locked
 */
static INLINE void enter_timer_in_list(tlist_t *tp)
{
    tlist_t	**headp = (tlist_t **) &lis_tlist_heads[tp->handle % THASH] ;

    tp->next = *headp ;
    *headp = tp ;
}

/*
 * Remove timer from list.
 * Returns:
 *   0 - was not found on list
 *   1 - successfully removed
 */
static INLINE int remove_timer_from_list(tlist_t *tp)
{
    tlist_t	*t ;
    tlist_t	*nxt ;
    tlist_t	**headp = (tlist_t **) &lis_tlist_heads[tp->handle % THASH] ;

    if ((t = *headp) == NULL)	/* nothing in that hash list */
    {
	tp->next = NULL ;
	return 0;
    }

    if (t == tp)		/* our entry is first in list */
    {
	*headp = tp->next ;
	tp->next = NULL ;
	return 1;
    }

    /* Find this entry starting at the 2nd element and on down the list */
    for (nxt = NULL; t->next != NULL; t = nxt)
    {
	nxt = t->next ;
	if (nxt == tp)
	{
	    t->next = tp->next ;
	    tp->next = NULL ;
	    return 1;
	}
    }

    tp->next = NULL ;		/* ensure null next pointer */
    return 0;
}

static INLINE tlist_t *alloc_timer(char *file_name, int line_nr)
{
    tlist_t	*t ;
    tlist_t	**headp = (tlist_t **) &lis_tlist_heads[0] ;

    if ((t = *headp) == NULL)	/* nothing in the hash list */
	t = (tlist_t *) lis_alloc_timer(file_name, line_nr) ;
    else
    {
	*headp = t->next ;	/* link around 1st element */
	lis_mark_mem(t, file_name, line_nr) ;
    }

    return(t) ;			/* NULL if could not allocate */
}

/*
 * This is always the function passed to the kernel.  'arg' is a pointer to
 * the tlist_t for the timeout.
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void sys_timeout_fcn(ulong arg)
{
    tlist_t		*tp = (tlist_t *) arg ;
#else
static void sys_timeout_fcn(struct timer_list *tmout_tl)
{
    struct lis_timer_list *lis_tl = from_timer(lis_tl,tmout_tl,tl);
    tlist_t             *tp = (tlist_t *) lis_tl->arg ;
#endif

    timo_fcn_t		*fcn = (timo_fcn_t *) NULL ;
    caddr_t		 uarg = NULL ;
    lis_flags_t  	 psw;

    lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;

    if (!remove_timer_from_list(tp))
    {
	/* someone beat us to it */
        lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
	return;
    }
    if (tp->handle != 0 && tp->fcn != NULL)
    {
	fcn        = tp->fcn ;		/* save local copy */
	uarg       = tp->arg ;
    }

    tp->handle = 0 ;			/* entry now available */
    tp->fcn    = NULL ;
    enter_timer_in_list(tp) ;		/* enter in list 0 (free list) */
    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;

    if (fcn != (timo_fcn_t *) NULL)
	fcn(uarg) ;			/* call fcn while not holding lock */
}


/*
 * Find the timer entry associated with the given handle.
 *
 * Caller has timer list locked
 */
static INLINE tlist_t *find_timer_by_handle(int handle)
{
    tlist_t	*tp = NULL ;
    tlist_t	**headp = (tlist_t **) &lis_tlist_heads[handle % THASH] ;

    for (tp = *headp; tp != NULL; tp = tp->next)
    {
	if (tp->handle == handle) return(tp) ;
    }

    return(NULL) ;
}

/*
 * Allocate an unused timer handle.  If there is a timer structure in
 * the list that is available return a pointer to it, else null.
 *
 * In any event return a unique handle.
 *
 * Caller has locked the timer list.
 */
static INLINE int alloc_handle(void)
{
    tlist_t	*tp = NULL ;
    int		 handle ;

    while (1)
    {
	do
	{					/* next handle */
	    handle = (++lis_tlist_handle) & 0x7FFFFFFF ;
	}
	while ((handle % THASH) == 0) ;		/* prevent use of handle 0 */

	tp = find_timer_by_handle(handle) ;
	if (tp == NULL)				/* handle available */
	    break ;
    }

    return(handle) ;
}

/*
 * "timeout" is #defined to be this function in dki.h.
 */
toid_t	_RP lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
		    char *file_name, int line_nr)
{
    tlist_t	*tp ;
    int		 handle ;
    lis_flags_t  psw;
    static tlist_t z ;

    lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;
    tp = alloc_timer(file_name, line_nr) ;	/* available timer struct */
    if (tp == NULL)				/* must allocate a new one */
    {
        lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
	return(0) ;				/* no memory for timer */
    }

    handle = alloc_handle() ;

    *tp = z ;					/* zero out structure */
    tp->handle	= handle ;
    tp->fcn	= timo_fcn ;
    tp->arg	= arg ;
    
    enter_timer_in_list(tp) ;			/* hashed by handle */
    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,14,0)
    lis_tmout(&tp->lis_tl, sys_timeout_fcn, (long) tp, ticks) ;
#else
    lis_tmout(&tp->tl, sys_timeout_fcn, (long) tp, ticks) ;
						/* start system timer */
#endif
    return(handle) ;				/* return handle */

} /* lis_timeout_fcn */

toid_t	_RP lis_untimeout(toid_t id)
{
    tlist_t	*tp ;
    lis_flags_t  psw;

    lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;

    tp = find_timer_by_handle(id) ;
    if (tp != NULL)
    {
	lis_mark_mem(tp, "unused-timer", MEM_TIMER) ;
	remove_timer_from_list(tp) ;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,14,0)
        lis_untmout(&tp->lis_tl.tl);
#else
	lis_untmout(&tp->tl) ;			/* stop system timer */
#endif
	tp->handle = 0 ;			/* make available */
	enter_timer_in_list(tp) ;		/* list 0 is free list */
    }

    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
    return(0);					/* no rtn value specified */

} /* lis_untimeout */

void lis_terminate_dki(void)
{
    lis_flags_t     psw;
    int		    i ;

    /*
     *  In case of buggy drivers, timeouts could still happen.
     *  Better be on the safe side.
     */
    lis_spin_lock_irqsave(&lis_tlist_lock, &psw) ;

    for (i = 0; i < THASH; i++)
    {
	while (lis_tlist_heads[i] != NULL)
	{
	    tlist_t *tp = (tlist_t *) lis_tlist_heads[i];
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,14,0)
            lis_untmout(&tp->lis_tl.tl);
#else
            lis_untmout(&tp->tl);
#endif
	    remove_timer_from_list(tp) ;
	    lis_free_timer(tp);
	}
    }

    lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw) ;
    lis_terminate_timers() ;			/* an mdep routine */
}

void lis_initialize_dki(void)
{
    lis_init_timers(sizeof(tlist_t)) ;		/* an mdep routine */
}


