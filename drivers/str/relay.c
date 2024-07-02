/*                               -*- Mode: C -*- 
 * relay.c --- A simple relay pushable module
 * Author          : Dave Grothe
 * Created On      : Dec 30, 1995
 * Last Modified By: Jeff L Smith
 * RCS Id          : $Id: relay.c,v 2022/11/07  jefsmith@us.ibm.com
 * Purpose         : relay messages just to test pushable modules
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  David Grothe <dave@gcom.com>
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
 */

#ident "@(#) CSLiS relay.c 7.111 2024-05-07 15:30:00 "


/*  -------------------------------------------------------------------  */

/*
 * The module that goes by the name "relay3" is a separately loadable
 * module that is not configured into LiS.  Used for testing.
 */

#ifdef LIS_OBJNAME  /* This must be defined before including module.h on Linux 6.8 */
#define _LINUX_IF_H
#define IFNAMSIZ        16
#endif 

#include <sys/LiS/module.h>	/* first ... */

#include <sys/stream.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
#include <sys/osif.h>
#endif

/*  -------------------------------------------------------------------  */
/*			  Module definition structs                      */

/* Module info for the relay module
 */
static struct module_info relay_minfo =
{
  0,				/* id */
  "relay",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

static struct module_info relay2_minfo =
{
  0,				/* id */
  "relay2",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

static struct module_info relay3_minfo =
{
  0,				/* id */
  "relay3",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

/* These are the entry points to the driver: open, close, write side put and
 * service procedures and read side service procedure.
 */
static int   _RP relay_open  (queue_t *,dev_t*,int,int, cred_t *);
static int   _RP relay_close (queue_t *, int, cred_t *);
static int   _RP relay_wput  (queue_t *, mblk_t *);
static int   _RP relay_rput  (queue_t *, mblk_t *);
#if 0
static int   _RP relay_wsrv  (queue_t *);
static int   _RP relay_rsrv  (queue_t *);
#endif

/* qinit structures (rd and wr side) 
 */
static struct qinit relay_rinit =
{
  relay_rput,			/* put */       
  NULL,				/* service  */  
  relay_open,			/* open */      
  relay_close,			/* close */     
  NULL,				/* admin */     
  &relay_minfo,			/* info */      
  NULL				/* stat */      
};

static struct qinit relay_winit =
{
  relay_wput,                    /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &relay_minfo, 			/* info */      
  NULL				/* stat */      
};

/* streamtab for the relay modules
 */
struct streamtab relay_info =
{
  &relay_rinit,			/* read queue */
  &relay_winit,			/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};

static struct qinit relay2_rinit =
{
  relay_rput,			/* put */       
  NULL,				/* service  */  
  relay_open,			/* open */      
  relay_close,			/* close */     
  NULL,				/* admin */     
  &relay2_minfo,		/* info */      
  NULL				/* stat */      
};

static struct qinit relay2_winit =
{
  relay_wput,                   /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &relay2_minfo, 		/* info */      
  NULL				/* stat */      
};

/* streamtab for the relay modules
 */
struct streamtab relay2_info =
{
  &relay2_rinit,		/* read queue */
  &relay2_winit,		/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};

static struct qinit relay3_rinit =
{
  relay_rput,			/* put */       
  NULL,				/* service  */  
  relay_open,			/* open */      
  relay_close,			/* close */     
  NULL,				/* admin */     
  &relay3_minfo,		/* info */      
  NULL				/* stat */      
};

static struct qinit relay3_winit =
{
  relay_wput,                   /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &relay3_minfo, 		/* info */      
  NULL				/* stat */      
};

/* streamtab for the relay modules
 */
struct streamtab relay3_info =
{
  &relay3_rinit,		/* read queue */
  &relay3_winit,		/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};

/*  -------------------------------------------------------------------  */
/*			    Module implementation                        */
/*  -------------------------------------------------------------------  */

/*  -------------------------------------------------------------------  */
/*				relay_open				 */
/*  -------------------------------------------------------------------  */
static int _RP
relay_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
  printk("relay_open(dev=0x%x, flag=0x%x, sflag=0x%x)\n",
				  DEV_TO_INT(*devp), flag, sflag) ;
  if (sflag == CLONEOPEN)
      return(EINVAL) ;

  return 0;					/* success */

}

/*  -------------------------------------------------------------------  */
/*				relay_wput				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_wput (queue_t *q, mblk_t *mp)
{
#ifdef DEBUG
    lis_print_msg(mp, "relay_wput", PRINT_DATA_RDWR) ;
#endif
    putnext(q, mp) ;			/* relay downstream */
    return(0) ;
}

/*  -------------------------------------------------------------------  */
/*				relay_rput				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_rput (queue_t *q, mblk_t *mp)
{
#ifdef DEBUG
    lis_print_msg(mp, "relay_rput", PRINT_DATA_RDWR) ;
#endif
    putnext(q, mp) ;			/* relay upstream */
    return(0) ;
}

#if 0
/*  -------------------------------------------------------------------  */
/*				relay_wsrv				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_wsrv (queue_t *q)
{
    /* not used */
    return(0) ;
}
#endif

#if 0
/*  -------------------------------------------------------------------  */
/*				relay_rsrv				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_rsrv (queue_t *q)
{
    /* not used */
    return(0) ;
}
#endif

/*  -------------------------------------------------------------------  */
/*				relay_close				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_close (queue_t *q, int dummy, cred_t *credp)
{
    printk("relay_close\n") ;
    return 0;
}

/*  -------------------------------------------------------------------  */
/*				Module Routines				 */
/*  -------------------------------------------------------------------  */

#ifdef MODULE

#ifdef KERNEL_2_5
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
int relay3_init_module(void)
#else
int __init relay3_init_module(void)
#endif
#else
int init_module(void)
#endif
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    printk("relay3.init_module: Attempt to register.\n");
    int ret = lis_register_strmod(&relay3_info, "relay3");
#else
    int ret;

    printk("relay3.init_module: Attempt to register.\n");
    ret = lis_register_strmod(&relay3_info, "relay3");
#endif
    if (ret < 0)
    {
	printk("relay3.init_module: Unable to register module.\n");
	return ret;
    }
    return 0;
}

#ifdef KERNEL_2_5
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
void relay3_cleanup_module(void)
#else
void __exit relay3_cleanup_module(void)
#endif
#else
void cleanup_module(void)
#endif
{
    if (lis_unregister_strmod(&relay3_info) < 0)
	printk("relay3.cleanup_module: Unable to unregister module.\n");
    else
	printk("relay3.cleanup_module: Unregistered, ready to be unloaded.\n");
    return;
}

#ifndef KM26
#ifdef KERNEL_2_5
module_init(relay3_init_module) ;
module_exit(relay3_cleanup_module) ;
#endif
#endif

#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL and additional rights");
#endif

#endif			/* MODULE */
