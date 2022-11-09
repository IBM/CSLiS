/* #
   # Copyright 2022 - IBM Inc. All rights reserved
   # SPDX-License-Identifier: LGPL-2.1
# */
#ident "@(#) CSLiS getmsg.c 7.11 2022-10-26 15:30:00 "

extern int
getpmsg(int fd, void *ctlptr, void *dataptr, int *bandp, int *flagsp) ;

int	getmsg(int fd, void *ctlptr, void *dataptr, int *flagsp)
{
    return(getpmsg(fd, ctlptr, dataptr, (void *) 0, flagsp)) ;
}
