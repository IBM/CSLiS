/*                               -*- Mode: C -*- 
 * events.c --- streams events
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: events.c,v 1.1.1.1 2005/04/12 20:27:05 ragnar Exp $
 * Purpose         : provide streams events
 *                 : 
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995   Francisco J. Ballesteros, Denis Froschauer
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
 *    You can reach us by email to
 *    nemo@ordago.uc3m.es, 100541.1151@compuserve.com
 */

#ident "@(#) CSLiS events.c 7.11 2022-10-26 15:30:00 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/strport.h>
#include <sys/strconfig.h>	/* config definitions */
#include <sys/LiS/share.h>	/* streams shared defs*/
#include <sys/LiS/events.h>		/* module interface */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

struct strevent *lis_sefreelist; /* list of free stream events */
struct strevent *lis_secachep;   /* reserve store of free stream events */

#if 0
/* see long comment in header --nemo*/
struct strinfo lis_strinfo[]; /* keeps track of allocated events	*/
#endif

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* The event list is implemented with a pointer to an event node.
 * the list will have a dummy node at head where next & prev will be used
 * as pointers to first and last of the list.  The list is linked circularly,
 * such that the next of the last element points to the first element.
 * The list is sorted by pid.
 * Every events for the same pid are in the same strevent node.
 * When last event is removed the node will be freed.
 *
 * It would be better to use a hash w/ ovf lists table. --nemo.
 */


/* get events for pid in list
 */
short
lis_get_elist_ent( strevent_t *list, pid_t pid )
{
    strevent_t *ev = NULL;

    /* should think what kind of search is faster: fwd, bck, sorted,...*/
    if (list == NULL) return(0) ;

    for(ev = list->se_next ; ev != list && ev->se_pid < pid; ev=ev->se_next )
	;
    if (ev != list && ev->se_pid == pid)
	return(ev->se_evs);
    else
	return(0);
}/*lis_get_elist_ent*/

/*  -------------------------------------------------------------------  */

/* add event to list
 */
int
lis_add_to_elist( strevent_t **list, pid_t pid, short events )
{
    strevent_t *ev = NULL;

    if (*list != NULL)
    {
	for (ev=(*list)->se_next;
	     ev != *list && ev->se_pid < pid;
	     ev=ev->se_next
	    );    
    }

    if (ev == NULL || ev == *list)		/* no slot for pid in list */
    {
	if ((ev = (strevent_t*)ALLOCF(sizeof(strevent_t),"event "))==NULL)
	    return(-ENOMEM);

	if (!*list)			/* create dummy head node */
	{
	    strevent_t *hd;
	    if ((hd = (strevent_t*)ALLOCF(sizeof(strevent_t),"event-head ")
					 )==NULL)
	    {
		FREE(ev);
		return(-ENOMEM);
	    }
	    (*list=hd)->se_pid=0;
	    hd->se_next=hd->se_prev=hd;		/* empty list */
	}

	/* link node last in the list */
	ev->se_prev=(*list)->se_prev;
	(*list)->se_prev->se_next=ev;
	((*list)->se_prev=ev)->se_next=*list;

	ev->se_pid=pid;
	ev->se_evs=0;
    }
    else if (ev->se_pid!=pid){	/* link node in the middle of the list */
	strevent_t *new;
	if ((new = (strevent_t*)ALLOCF(sizeof(strevent_t),"event "))==NULL){
	    return(-ENOMEM);
	}
	new->se_prev=ev->se_prev;
	new->se_next=ev;
	ev->se_prev->se_next=new;
	ev->se_prev=new;
	ev = new ;				/* use new element */
	ev->se_pid=pid;
	ev->se_evs=0;
    }
    ev->se_evs|=events;
    return(0);
}/*lis_add_to_elist*/

/*  -------------------------------------------------------------------  */

/* del event from list
 * rets non-zero if not-found
 */
int
lis_del_from_elist( strevent_t **list, pid_t pid, short events )
{
    strevent_t *ev = NULL;     

    if (*list != NULL)
    {
	for (ev=(*list)->se_next;
	     ev != *list && ev->se_pid < pid;
	     ev=ev->se_next
	    );    
    }

    if (ev == NULL || ev == *list || ev->se_pid != pid )
	return(1);

    if ( (ev->se_evs &= ~events) == 0 ){	/* unlink */
	if (ev->se_next)			/* should always be true */
	    ev->se_next->se_prev=ev->se_prev;
	if (ev->se_prev)			/* should always be true */
	    ev->se_prev->se_next=ev->se_next;
	FREE(ev);
    }
    return(0);
}/*lis_del_from_elist*/

/*  -------------------------------------------------------------------  */
/* Free the entire elist
 */
void
lis_free_elist( strevent_t **list )
{
    strevent_t  *ev;     
    strevent_t	*nxt ;

    for (ev = *list; ev != NULL; )
    {
	nxt = ev->se_next ;
	FREE(ev) ;
	ev = nxt ;
	if (ev == *list) break ;		/* all done */
    }

    *list = NULL ;

}/*lis_free_elist*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
