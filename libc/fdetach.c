/* #
   # Copyright 2022 - IBM Inc. All rights reserved
   # SPDX-License-Identifier: LGPL-2.1
# */
#ident "@(#) CSLiS fdetach.c 7.11 2022-10-26 15:30:00 "

#define _REENTRANT
#define _THREAD_SAFE
#define _XOPEN_SOURCE 500

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>


#define DUMMY_STREAM "/dev/fifo.0"    /* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

int fdetach( const char *path )
{
    int ffd, error = 0;

    errno = 0;

    if ((ffd = open( DUMMY_STREAM, DUMMY_MODE )) < 0)  return -1;

    if (ioctl( ffd, I_LIS_FDETACH, path ) < 0)  error = errno;

    close(ffd);

    if (error) {
	errno = error;  return -1;
    }
    return 0;
}
