/* #
   # Copyright 2022 - IBM Inc. All rights reserved
   # SPDX-License-Identifier: LGPL-2.1
# */
#ident "@(#) CSLiS putmsg.c 7.11 2022-10-26 15:30:00 "

extern int
putpmsg(int fd, void *ctlptr, void *dataptr, int band, int flags) ;

int	putmsg(int fd, void *ctlptr, void *dataptr, int flags)
{
    return(putpmsg(fd, ctlptr, dataptr, -1, flags)) ;
}
