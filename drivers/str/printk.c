/************************************************************************
*                          Printk Driver                                *
*************************************************************************
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*                                                                       *
* Copyright 2022 - IBM Inc. All rights reserved                         *
* SPDX-License-Identifier: LGPL-2.1                                     *
*									*
* This driver, /dev/printk, allows user level programs to write to	*
* the kernel's log.  When a user program writes to /dev/printk it	*
* turns into a kernel printk() call.  The purpose of this driver is	*
* to allow user level test programs of STREAMS to get their messages	*
* in sequence with the debug messages originating from the kernel.	*
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

#ident "@(#) CSLiS printk.c 7.111 2024-05-07 15:30:00 "

#include <sys/stream.h>
#ifdef LIS_OBJNAME  /* This must be defined before including module.h on Linux 6.8 */
#define _LINUX_IF_H
#define IFNAMSIZ        16
#endif
#include <sys/osif.h>

static struct module_info printk_minfo =
{
  0,				/* id */
  "printk",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  50000,			/* high water mark */
  40000				/* low water mark */
};

static int   _RP printk_open  (queue_t *, dev_t*, int, int, cred_t *);
static int   _RP printk_close (queue_t *, int, cred_t *);
static int   _RP printk_wput (queue_t *, mblk_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit printk_rinit =
{
  NULL,				/* put */       
  NULL,				/* service  */  
  printk_open,			/* open */      
  printk_close,			/* close */     
  NULL,				/* admin */     
  &printk_minfo,		/* info */      
  NULL				/* stat */      
};

static struct qinit printk_winit =
{
  printk_wput, 	                /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &printk_minfo, 		/* info */      
  NULL				/* stat */      
};

/* streamtab for the printk driver.
 */
struct streamtab printk_info =
{
  &printk_rinit,		/* read queue */
  &printk_winit,		/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};


/*
 * Open routine grants all opens
 */
static int _RP
printk_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
    (void) q ;					/* compiler happiness */
    (void) devp ;				/* compiler happiness */
    (void) flag ;				/* compiler happiness */
    (void) sflag ;				/* compiler happiness */
    (void) credp ;				/* compiler happiness */

    return(0) ;					/* success */

} /* printk_open */


static int _RP
printk_close (queue_t *q, int dummy, cred_t *credp)
{
    (void) q ;					/* compiler happiness */
    (void) dummy ;
    (void) credp ;

    return(0) ;
}

static int  _RP
printk_wput (queue_t *q, mblk_t *msg)
{
    mblk_t		*mp ;
    unsigned char	*p ;
    unsigned char	 c ;

    (void) q;				/* compiler happiness */

    if (msg->b_datap->db_type != M_DATA)
    {
	freemsg(msg) ;
	return(0) ;
    }

    for (mp = msg; mp != NULL; mp = mp->b_cont)
    {
	if (mp->b_rptr >= mp->b_wptr) continue ;

	p = mp->b_wptr - 1 ;			/* to last chr of msg */
	c = *p ;				/* fetch last chr */
	*p = 0 ;				/* last chr to NUL */
	printk("%s%c", mp->b_rptr, c) ;		/* write msg */
    }

    freemsg(msg) ;				/* done with msg */
    return(0) ;

} /* printk_wput  */

