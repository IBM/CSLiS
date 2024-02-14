/*                               -*- Mode: C -*- 
 *
 * Copyright 2022 - IBM Inc. All rights reserved
 * SPDX-License-Identifier: LGPL-2.1
 *
 * strlog.h --- streams strlog cntl handling
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: 
 * Last Modified On: 
 * Update Count    : 0
 * RCS Id          : $Id: strlog.h,v 1.1.1.1 2005/04/12 20:27:06 ragnar Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) Unknown, Use with caution!
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : here you have utilites to handle str strlogs.
 *                 : 
 * ----------------______________________________________________
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

#ifndef _STRLOG_H
#define _STRLOG_H 1

#ident "@(#) CSLiS strlog.h 7.11 2022-10-26 15:30:00 "


/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */


/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/*  -------------------------------------------------------------------  */
/* strlog - send a message to STREAMS log driver
 *
 * STATUS: incomplete, untested
 */
#ifdef __KERNEL__
extern int
lis_strlog(short mid, short sid, char level, unsigned short flags,
	   char *fmt, ... );
#endif				/* __KERNEL__ */



/*  -------------------------------------------------------------------  */
#endif /*!_STRLOG_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
