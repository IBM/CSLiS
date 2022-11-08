/* #
   # Copyright 2022 - IBM Inc. All rights reserved
   # SPDX-License-Identifier: LGPL-2.1
# */
#ident "@(#) CSLiS fattach.c 7.11 2022-10-26 15:30:00 "

#include <stropts.h>
#include <sys/ioctl.h>


int fattach( int fd, const char *path )
{
    return (ioctl( fd, I_LIS_FATTACH, path ));
}

