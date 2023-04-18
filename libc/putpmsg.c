/* #
   # Copyright 2022 - IBM Inc. All rights reserved
   # SPDX-License-Identifier: LGPL-2.1
# */
#include <sys/stropts.h>		/* must be LiS version */
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#ifdef BLD32OVER64
typedef struct strbuf6 {
    int       maxlen;
    int       len;
    unsigned long long buf;
} strbuf6_t;

typedef struct putpmsg_args6
{
    int                  fd ;
    int                  pad;
    unsigned long long   ctl ;
    unsigned long long   dat ;
    int                  band ;
    int                  flags ;

} putpmsg_args6_t ;

int	lis_putpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int band, int flags)
{
    putpmsg_args6_t	args ;
    strbuf6_t ctl6;
    strbuf6_t * ptrc6 = 0;
    strbuf6_t dat6;
    strbuf6_t * ptrd6 = 0;

    if (ctlptr)
    {
      ctl6.maxlen = ctlptr->maxlen;
      ctl6.len    = ctlptr->len;
      ctl6.buf    = (unsigned long long)(unsigned long)ctlptr->buf;
      ptrc6 = &ctl6;
    }

    if (dataptr)
    {
      dat6.maxlen = dataptr->maxlen;
      dat6.len    = dataptr->len;
      dat6.buf    = (unsigned long long)(unsigned long)dataptr->buf;
      ptrd6 = &dat6;
    }

    memset((void*)&args,0,sizeof(putpmsg_args6_t));
    args.fd	= fd ;
    args.ctl	= (unsigned long long)(unsigned long)ptrc6 ;
    args.dat	= (unsigned long long)(unsigned long)ptrd6 ;
    args.band	= band ;
    args.flags	= flags ;

    return(ioctl(fd, I_LIS_PUTMSG, &args));
}
#else
int	lis_putpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int band, int flags)
{
    putpmsg_args_t	args ;

    args.fd	= fd ;
    args.ctl	= ctlptr ;
    args.dat	= dataptr ;
    args.band	= band ;
    args.flags	= flags ;

    return(ioctl(fd, I_LIS_PUTMSG, &args));
}
#endif
int	putpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int band, int flags)
{
   	return lis_putpmsg(fd, ctlptr, dataptr, band, flags);
}
