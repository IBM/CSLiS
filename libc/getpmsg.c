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

typedef struct getpmsg_args6
{
    int                  fd ;
    int                  pad;
    unsigned long long   ctl ;
    unsigned long long   dat ;
    unsigned long long   bandp ;
    unsigned long long   flagsp ;

} getpmsg_args6_t ;

int	lis_getpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int *bandp, int *flagsp)
{
    strbuf6_t ctl6;
    strbuf6_t * ptrc6 = 0;
    strbuf6_t dat6;
    strbuf6_t * ptrd6 = 0;
    getpmsg_args6_t  args ;
    int rc;

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

    memset((void*)&args,0,sizeof(getpmsg_args6_t));
    args.fd	= fd ;
    args.ctl	= (unsigned long long)(unsigned long)ptrc6 ;
    args.dat	= (unsigned long long)(unsigned long)ptrd6 ;
    args.bandp	= (unsigned long long)(unsigned long)bandp ;
    args.flagsp	= (unsigned long long)(unsigned long)flagsp ;

    rc = ioctl(fd, I_LIS_GETMSG, &args);

    if (ctlptr)
    {
      ctlptr->len  = ctl6.len;
    }
    if (dataptr)
    {
      dataptr->len = dat6.len;
    }

    return(rc);
}
#else
int	lis_getpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int *bandp, int *flagsp)
{
    getpmsg_args_t	args ;

    args.fd	= fd ;
    args.ctl	= ctlptr ;
    args.dat	= dataptr ;
    args.bandp	= bandp ;
    args.flagsp	= flagsp ;

    return(ioctl(fd, I_LIS_GETMSG, &args));
}
#endif
int	getpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int *bandp, int *flagsp)
{
    return lis_getpmsg(fd, ctlptr, dataptr, bandp, flagsp);
}
