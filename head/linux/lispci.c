/************************************************************************
*                          LiS PCI Interface                            *
*************************************************************************
*									*
* This interface was created to insulate the Linux STREAMS programmer	*
* from the constantly changing PCI interface provided by the Linux	*
* kernel.  Most of these routines are defined even if the the           *
* underlying kernel does not support PCI (CONFIG_PCI not defined).      *
* We still call underlying kernel PCI function which, in turn, return   *
* an error such as EIO.                                                 *
*									*
*    Copyright (C) 2000  David Grothe, Gcom, Inc <dave@gcom.com>	*
*                                                                       *
* Copyright 2022 - IBM Inc. All rights reserved                         *
* SPDX-License-Identifier: LGPL-2.1                                     *
*									*
* This library is free software; you can redistribute it and/or		*
* modify it under the terms of the GNU Library General Public		*
* License as published by the Free Software Foundation; either		*
* version 2 of the License, or (at your option) any later version.	*
* 									*
* This library is distributed in the hope that it will be useful,	*
* but WITHOUT ANY WARRANTY; without even the implied warranty of	*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*
* Library General Public License for more details.			*
* 									*
* You should have received a copy of the GNU Library General Public	*
* License along with this library; if not, write to the			*
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,		*
* Cambridge, MA 02139, USA.						*
*									*
************************************************************************/

#ident "@(#) CSLiS lispci.c 7.111 2024-05-07 15:30:00 "

#include <sys/stream.h>		/* gets all the right LiS stuff included */
#include <sys/lispci.h>		/* LiS PCI header file */
#include <sys/lismem.h>		/* mainly needed for 2.2 kernels */

# ifdef RH_71_KLUDGE			/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM			/* b_page has semi-circular reference */
# endif
#include <linux/pci.h>		/* kernel PCI header file */

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
#include <sys/osif.h>
#else
#define PCI_DMA_BIDIRECTIONAL   DMA_BIDIRECTIONAL
#define PCI_DMA_TODEVICE        DMA_TO_DEVICE
#define PCI_DMA_FROMDEVICE      DMA_FROM_DEVICE
#define PCI_DMA_NONE            DMA_NONE
#endif


static lis_pci_dev_t	*lis_pci_dev_list = NULL ;
static lis_pci_dev_t 	*lis_pci_dev_list_end = NULL ;

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)

/************************************************************************
*                       lis_map_pci_device                              *
*************************************************************************
*									*
* Given a pointer to one of our pci device structures and to one of the	*
* kernel's structures, map theirs into ours.				*
*									*
************************************************************************/
void	_RP lis_map_pci_device(lis_pci_dev_t *p, struct pci_dev *kp)
{
    int		i ;

    /*
     * Copy some data from kernel structure into ours
     */
    p->kern_ptr		= kp ;			/* to kernel's structure */
    p->bus		= kp->bus->number ;
    p->dev_fcn		= kp->devfn ;
    p->vendor		= kp->vendor ;
    p->device		= kp->device ;
    p->class		= kp->class ;
    p->hdr_type		= kp->hdr_type ;
    p->irq		= kp->irq ;


    {
	int	nn = LIS_PCI_MEM_CNT ;		/* LiS size */

	if (nn > DEVICE_COUNT_RESOURCE)		/* kernel size */
	    nn = DEVICE_COUNT_RESOURCE ;	/* reduce loop count */

	for (i = 0; i < nn; i++)
	{
	    p->mem_addrs[i] = kp->resource[i].start ;
	}
    }

} /* lis_map_pci_device */

/************************************************************************
*                          lis_pci_find_device                          *
*************************************************************************
*									*
* Find a device of the given vendor and device id and return a pointer	*
* to a lis_pci_dev_t structure for it.					*
*									*
************************************************************************/
lis_pci_dev_t   * _RP lis_pci_find_device(unsigned vendor, unsigned device,
                                     lis_pci_dev_t *previous_struct)
{
    lis_pci_dev_t	*p ;
    struct pci_dev	*kp ;
    struct pci_dev	*prev_kp ;

    if(previous_struct == NULL)
    {
	prev_kp = NULL;
	p = lis_pci_dev_list;
    }
    else
    {
	prev_kp = previous_struct->kern_ptr ;
	p = previous_struct->next;
    }

    while(p != NULL) 
    {
	if (p->vendor == vendor && p->device == device) return(p) ;
	p = p->next;
    }
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,8)
    kp = pci_find_device(vendor, device, prev_kp) ;
    if (kp == NULL) return(NULL) ;
#else
    kp = pci_get_device(vendor, device, prev_kp) ;
    if (kp == NULL) return(NULL) ;
    pci_dev_put(kp);    
#endif

    /*
     * Allocate our own structure that will correspond to the kernel
     * structure.
     */
    p = (lis_pci_dev_t *) ALLOCF(sizeof(*p),"lis_pci_dev_t ");
    if (p == NULL) return(NULL) ;		/* allocation failed */

    memset(p, 0, sizeof(*p)) ;
    if(lis_pci_dev_list_end == NULL) /* first one */
    {
	lis_pci_dev_list = p;
	lis_pci_dev_list_end = p;
    }
    else
    {
	lis_pci_dev_list_end->next = p;		/* remember it */
    }
    p->next = NULL;
    lis_pci_dev_list_end = p ;

    lis_map_pci_device(p, kp) ;			/* copy info */
    return(p) ;

} /* lis_pci_find_device */


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
/************************************************************************
*                          lis_pci_find_class                           *
*************************************************************************
*									*
* Find a class in a manner similar to finding devices.			*
*									*
************************************************************************/
lis_pci_dev_t   * _RP lis_pci_find_class(unsigned class,
                                     lis_pci_dev_t *previous_struct)
{
    lis_pci_dev_t	*p ;
    struct pci_dev	*kp ;
    struct pci_dev	*prev_kp ;

    for (p = lis_pci_dev_list; p != NULL; p = p->next)
    {
	if (p->class == class) return(p) ;
    }

    prev_kp = previous_struct == NULL ? NULL : previous_struct->kern_ptr ;
    kp = pci_find_class(class, prev_kp) ;
    if (kp == NULL) return(NULL) ;

    /*
     * Allocate our own structure that will correspond to the kernel
     * structure.
     */
    p = (lis_pci_dev_t *) ALLOCF(sizeof(*p),"lis_pci_dev_t ");
    if (p == NULL) return(NULL) ;		/* allocation failed */

    memset(p, 0, sizeof(*p)) ;
    p->next = lis_pci_dev_list ;		/* remember it */
    lis_pci_dev_list = p ;

    lis_map_pci_device(p, kp) ;			/* copy info */
    return(p) ;

} /* lis_pci_find_class */
#endif

/************************************************************************
*                          lis_pci_find_slot                            *
*************************************************************************
*									*
* Find the structure by slot number.					*
*									*
************************************************************************/
lis_pci_dev_t   * _RP lis_pci_find_slot(unsigned bus, unsigned dev_fcn)
{
    lis_pci_dev_t	*p ;
    struct pci_dev	*kp ;

    for (p = lis_pci_dev_list; p != NULL; p = p->next)
    {
	if (p->bus == bus && p->dev_fcn == dev_fcn) return(p) ;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
    kp = pci_find_slot(bus, dev_fcn) ;
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
    kp = pci_get_slot((struct pci_bus *)bus,dev_fcn);
#else
    kp = NULL;
#endif
#endif
    if (kp == NULL) return(NULL) ;

    /*
     * Allocate our own structure that will correspond to the kernel
     * structure.
     */
    p = (lis_pci_dev_t *) ALLOCF(sizeof(*p),"lis_pci_dev_t ");
    if (p == NULL) return(NULL) ;		/* allocation failed */

    memset(p, 0, sizeof(*p)) ;
    p->next = lis_pci_dev_list ;		/* remember it */
    lis_pci_dev_list = p ;

    lis_map_pci_device(p, kp) ;			/* copy info */
    return(p) ;

} /* lis_pci_find_slot */

#endif  /* end kernel 6.0.0 check */

/************************************************************************
*                        lis_pci_read_config_byte                       *
*************************************************************************
*									*
* Read a byte from config space for the device.				*
*									*
************************************************************************/
int               _RP lis_pci_read_config_byte(lis_pci_dev_t *dev,
                                          unsigned       index,
                                          unsigned char *rtn_val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_read_config_byte(dev->kern_ptr, index, rtn_val)) ;

} /* lis_pci_read_config_byte */

/************************************************************************
*                        lis_pci_read_config_word                       *
*************************************************************************
*									*
* Read a word from config space for the device.				*
*									*
************************************************************************/
int               _RP lis_pci_read_config_word(lis_pci_dev_t  *dev,
                                          unsigned        index,
                                          unsigned short *rtn_val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_read_config_word(dev->kern_ptr, index, rtn_val)) ;

} /* lis_pci_read_config_word */

/************************************************************************
*                        lis_pci_read_config_dword                      *
*************************************************************************
*									*
* Read a dword from config space for the device.			*
*									*
************************************************************************/
int               _RP lis_pci_read_config_dword(lis_pci_dev_t *dev,
                                          unsigned       index,
                                          unsigned long *rtn_val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_read_config_dword(dev->kern_ptr, index, (u32 *) rtn_val)) ;

} /* lis_pci_read_config_dword */

/************************************************************************
*                       lis_pci_write_config_byte                       *
*************************************************************************
*									*
* Write a byte from config space for the device.			*
*									*
************************************************************************/
int               _RP lis_pci_write_config_byte(lis_pci_dev_t *dev,
                                          unsigned       index,
                                          unsigned char  val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_write_config_byte(dev->kern_ptr, index, val)) ;

} /* lis_pci_write_config_byte */

/************************************************************************
*                        lis_pci_write_config_word                      *
*************************************************************************
*									*
* Write a word from config space for the device.			*
*									*
************************************************************************/
int               _RP lis_pci_write_config_word(lis_pci_dev_t  *dev,
                                          unsigned        index,
                                          unsigned short  val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_write_config_word(dev->kern_ptr, index, val)) ;

} /* lis_pci_write_config_word */

/************************************************************************
*                        lis_pci_write_config_dword                     *
*************************************************************************
*									*
* Write a dword from config space for the device.			*
*									*
************************************************************************/
int               _RP lis_pci_write_config_dword(lis_pci_dev_t *dev,
                                          unsigned        index,
                                          unsigned long    val)
{
    if (dev == NULL || dev->kern_ptr == NULL) return(-EINVAL) ;
    return(pci_write_config_dword(dev->kern_ptr, index, val)) ;

} /* lis_pci_write_config_dword */

/************************************************************************
*                          lis_pci_enable_device                        *
*************************************************************************
*									*
* Enable the device.  Sometimes this is needed to resolve IRQ routing	*
* differences between different machines.				*
*									*
************************************************************************/
int  _RP lis_pci_enable_device (lis_pci_dev_t *dev)
{
    return (pci_enable_device (dev->kern_ptr));
}

/************************************************************************
*                         lis_pci_disable_device                        *
*************************************************************************
*									*
* Disable the device.  Undoes what lis_pci_enable_device does.		*
*									*
************************************************************************/
void  _RP lis_pci_disable_device (lis_pci_dev_t *dev)
{
    pci_disable_device (dev->kern_ptr);
}

/************************************************************************
*                         lis_pci_set_master                            *
*************************************************************************
*									*
* Set bus master capability for the device.				*
*									*
************************************************************************/
void              _RP lis_pci_set_master(lis_pci_dev_t *dev)
{
    if (dev == NULL || dev->kern_ptr == NULL) return ;
    pci_set_master(dev->kern_ptr) ;

} /* lis_pci_set_master */

/************************************************************************
*                             lis_pci_cleanup                           *
*************************************************************************
*									*
* Called at LiS module exit to deallocate structures.			*
*									*
************************************************************************/
void	 lis_pci_cleanup(void)
{
    lis_pci_dev_t	*p ;
    lis_pci_dev_t	*np ;

    for (p = lis_pci_dev_list; p != NULL; p = np)
    {
	np = p->next ;
	FREE(p) ;
    }

    lis_pci_dev_list = NULL ;

} /* lis_pci_cleanup */

/************************************************************************
*                         DMA Memory Allocation				*
************************************************************************/

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)

void    * _RP lis_pci_alloc_consistent(lis_pci_dev_t  *dev,
				  size_t          size,
				  lis_dma_addr_t *dma_handle)
{
#ifdef CONFIG_PCI
    dma_addr_t	*dp = (dma_addr_t *) dma_handle->opaque ;
    void	*vaddr ;
    
    dma_handle->size = size ;
    dma_handle->dev  = dev ;
    vaddr = pci_alloc_consistent(dev->kern_ptr, size, dp) ;
    dma_handle->vaddr = vaddr ;

    return(vaddr) ;
#else
    /*
     * pci_alloc_consistent() has no stub routine for non-PCI systems.
     */
    return (0);
#endif
}

void    * _RP lis_pci_free_consistent(lis_dma_addr_t *dma_handle)
{
#ifdef CONFIG_PCI
    /*
     * pci_free_consistent() has no stub routine for non-PCI systems.
     */
    if (dma_handle->vaddr != NULL)
	pci_free_consistent(dma_handle->dev->kern_ptr,
			    dma_handle->size,
			    dma_handle->vaddr,
			    *((dma_addr_t *) dma_handle->opaque)) ;
    dma_handle->vaddr = NULL ;
#endif
    return(NULL) ;
}

#endif /* end kernel 6.0.0 check */

u32      _RP lis_pci_dma_handle_to_32(lis_dma_addr_t *dma_handle)
{
    u32		*p = (u32 *) dma_handle->opaque ;

    return(*p) ;
}

u64      _RP lis_pci_dma_handle_to_64(lis_dma_addr_t *dma_handle)
{
    u64		*p = (u64 *) dma_handle->opaque ;

    return(*p) ;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)

void     _RP lis_pci_map_single(lis_pci_dev_t *dev,
			   void         *ptr,
			   size_t        size,
			   lis_dma_addr_t *dma_handle,
			   int           direction)
{
    dma_addr_t	*dp = (dma_addr_t *) dma_handle->opaque ;

    switch (direction)
    {
    case LIS_SYNC_FOR_CPU:	direction = PCI_DMA_FROMDEVICE ; break ;
    case LIS_SYNC_FOR_DMA:	direction = PCI_DMA_TODEVICE ; break ;
    case LIS_SYNC_FOR_BOTH:	direction = PCI_DMA_BIDIRECTIONAL ; break ;
    default:			return ;
    }

    dma_handle->size      = size ;
    dma_handle->dev       = dev ;
    dma_handle->vaddr     = ptr ;
    dma_handle->direction = direction ;
#ifdef CONFIG_PCI
    *dp = pci_map_single(dev->kern_ptr, ptr, size, direction);
#endif
}

void * _RP lis_pci_unmap_single(lis_dma_addr_t *dma_handle)
{
#ifdef CONFIG_PCI
    if (dma_handle->vaddr != NULL)
	pci_unmap_single(dma_handle->dev->kern_ptr,
			 *((dma_addr_t *) dma_handle->opaque),
			 dma_handle->size,
			 dma_handle->direction);
#endif

    dma_handle->vaddr = NULL ;
    return(NULL) ;
}

/************************************************************************
*                      lis_pci_dma_sync_single                          *
*************************************************************************
*									*
* Synchronize memory with DMA.						*
*									*
************************************************************************/
void  _RP lis_pci_dma_sync_single(lis_dma_addr_t     *dma_handle,
			     size_t              size,
			     int                 direction)
{
    switch (direction)
    {
    case LIS_SYNC_FOR_CPU:	direction = PCI_DMA_FROMDEVICE ; break ;
    case LIS_SYNC_FOR_DMA:	direction = PCI_DMA_TODEVICE ; break ;
    case LIS_SYNC_FOR_BOTH:	direction = PCI_DMA_BIDIRECTIONAL ; break ;
    default:			return ;
    }

    if (size > dma_handle->size)
	size = dma_handle->size ;

#ifdef CONFIG_PCI
    pci_dma_sync_single(dma_handle->dev->kern_ptr,
			*((dma_addr_t *) dma_handle->opaque),
		        size,
			direction);
#endif
}

/************************************************************************
*                            Miscellaneous                              *
************************************************************************/

int      _RP lis_pci_dma_supported(lis_pci_dev_t *dev, u64 mask)
{
#ifdef CONFIG_PCI
    return(pci_dma_supported(dev->kern_ptr, mask)) ;
#else
    return(NULL);
#endif
}

int      _RP lis_pci_set_dma_mask(lis_pci_dev_t *dev, u64 mask)
{
    return(pci_set_dma_mask(dev->kern_ptr, mask)) ;
}

#endif  /* end kernel 6.0.0 check */ 

/************************************************************************
*                            Memory Barrier                             *
*************************************************************************
*									*
* Our own routine.  Kernel versioning takes care of differences.	*
*									*
************************************************************************/
void  _RP lis_membar(void)
{
    barrier();
    mb();
}

