/*
 *
 * Copyright 2022 - IBM Inc. All rights reserved
 * SPDX-License-Identifier: LGPL-2.1
 *
 * IOCTL definitions for mtdrv
 */

#ident "@(#) CSLiS mtdrv.h 7.11 2022-10-26 15:30:00 "

#define	MTDRV_IOCTL(n)	(('M' << 8) | (n))
#define MTDRV_SET_CLONE_DEV	MTDRV_IOCTL(1)
#define MTDRV_SET_OPEN_SLEEP	MTDRV_IOCTL(2)

