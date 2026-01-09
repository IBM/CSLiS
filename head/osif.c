/************************************************************************
*                   Operating System Interface                          *
*************************************************************************
*									*
* The routines in this file provide a filtered interface to certain	*
* operating system routines.  These are routines that drivers are	*
* most likely to use such as PCI BIOS calls and mapping addresses.	*
*									*
* This code loads as a loadable module, but it has no open routine.	*
*									*
* Additions to this file are welcome.					*
*									*
*	Copyright (c) 1999 David Grothe <dave@gcom.com>			*
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
*								 	*
* You should have received a copy of the GNU Library General Public	*
* License along with this library; if not, write to the	Free Software	*
* Foundation, Inc., 59 Temple Place - Suite 330, Cambridge, MA 02139 USA*
*									*
************************************************************************/

#ident "@(#) CSLiS osif.c 7.113 2025-12-11 15:30:00 "

#include <sys/stream.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif		/* Linux config defines */

#ifdef STR
#undef STR				/* collides with irq.h */
#endif

# ifdef RH_71_KLUDGE			/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM			/* b_page has semi-circular reference */
# endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,3,18)
#ifndef PCI_STD_NUM_BARS
#define PCI_STD_NUM_BARS	6	/* Number of standard BARs */
#endif
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,14,0)
#define _LINUX_IF_H
#define IFNAMSIZ        16
#define PCI_DMA_BIDIRECTIONAL	DMA_BIDIRECTIONAL
#define PCI_DMA_TODEVICE	DMA_TO_DEVICE
#define PCI_DMA_FROMDEVICE	DMA_FROM_DEVICE
#define PCI_DMA_NONE		DMA_NONE
#define __iovec_defined 1
#endif

#if (defined(_S390X_LIS_) || defined(_PPC64_LIS_) )
#if ((defined(RHEL_RELEASE_CODE) && RHEL_RELEASE_CODE > 2309) || \
     (LINUX_VERSION_CODE > KERNEL_VERSION(6,10,0))) /* RHEL 9.6 or RHEL 10, SLES 16 */
#define _LINUX_PROPERTY_H_  // omit property.h
#endif
#endif

#include <linux/pci.h>		/* PCI BIOS functions */
#include <linux/sched.h>		/* odd place for request_irq */
#include <linux/ioport.h>		/* request_region */
#include <asm/dma.h>
#include <linux/slab.h>
#include <linux/timer.h>                /* add_timer, del_timer */
#include <linux/ptrace.h>		/* for pt_regs */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
#include <linux/vmalloc.h>
#endif
#include <asm/io.h>			/* ioremap, virt_to_phys */
#include <asm/irq.h>			/* disable_irq, enable_irq */
#include <asm/atomic.h>			/* the real kernel routines */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
#include <asm/scatterlist.h>
#else
#include <linux/scatterlist.h>
#endif

#endif
#include <linux/delay.h>
#include <linux/time.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#include <linux/bios32.h>		/* old style PCI routines */
#include <linux/mm.h>			/* vremap */
#endif
#if (defined(RHEL_RELEASE_CODE) && RHEL_RELEASE_CODE < 2305)  //For RHEL 9 update 12-2022
#include <stdarg.h>                    /* for va_list */
#endif

#define	INCL_FROM_OSIF_DRIVER		/* do not change routine names */
#include <sys/osif.h>




/************************************************************************
*                       PCI BIOS Functions                              *
*************************************************************************
*									*
* These are filtered calls to the kernel's PCI BIOS routines.		*
* Whether the kernel supports PCI or not is based on CONFIG_PCI from    *
* autoconf.h.                                                           *
*									*
************************************************************************/

int _RP lis_pcibios_present(void)
{
#ifdef CONFIG_PCI
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
    return(pcibios_present()) ;
#else
    /* reasonable guess since CONFIG_PCI was defined */
    return(1);
#endif
#else
    return(-ENOSYS) ;
#endif
}

#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)) && defined(CONFIG_PCI))
/*
 * PCO BIOS init routine is not an exported symbol, but we define it
 * anyway in case some old driver was calling it.
 */
void _RP lis_pcibios_init(void)
{
}

int _RP lis_pcibios_find_class(unsigned int   class_code,
			    unsigned short index,
                            unsigned char *bus,
			    unsigned char *dev_fn)
{
    return(pcibios_find_class(class_code, index, bus, dev_fn)) ;
}

int _RP lis_pcibios_find_device(unsigned short vendor,
			     unsigned short dev_id,
			     unsigned short index,
			     unsigned char *bus,
			     unsigned char *dev_fn)
{
    return(pcibios_find_device(vendor, dev_id, index, bus, dev_fn)) ;
}

int _RP lis_pcibios_read_config_byte(unsigned char  bus,
				  unsigned char  dev_fn,
				  unsigned char  where,
				  unsigned char *val)
{
    if (where != PCI_INTERRUPT_LINE) 
	return pcibios_read_config_byte(bus, dev_fn, where, val);
    else {
	struct pci_dev *dev;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
	if ((dev = pci_find_slot(bus, dev_fn)) != NULL) {
	    *val = dev->irq;
	    return PCIBIOS_SUCCESSFUL;
	}
#else
	if ((dev = pci_get_slot(bus, dev_fn)) != NULL) {
	    *val = dev->irq;
	    return PCIBIOS_SUCCESSFUL;
	}
#endif

    }
    *val = 0;
    return PCIBIOS_DEVICE_NOT_FOUND;
}

int _RP lis_pcibios_read_config_word(unsigned char   bus,
				  unsigned char   dev_fn,
				  unsigned char   where,
				  unsigned short *val)
{
    return(pcibios_read_config_word(bus, dev_fn, where, val)) ;
}

int _RP lis_pcibios_read_config_dword(unsigned char  bus,
				   unsigned char  dev_fn,
				   unsigned char  where,
				   unsigned int  *val)
{
    return(pcibios_read_config_dword(bus, dev_fn, where, val)) ;
}

int _RP lis_pcibios_write_config_byte(unsigned char  bus,
				   unsigned char  dev_fn,
				   unsigned char  where,
				   unsigned char  val)
{
    return(pcibios_write_config_byte(bus, dev_fn, where, val)) ;
}

int _RP lis_pcibios_write_config_word(unsigned char   bus,
				   unsigned char   dev_fn,
				   unsigned char   where,
				   unsigned short  val)
{
    return(pcibios_write_config_word(bus, dev_fn, where, val)) ;
}

int _RP lis_pcibios_write_config_dword(unsigned char  bus,
				    unsigned char  dev_fn,
				    unsigned char  where,
				    unsigned int   val)
{
    return(pcibios_write_config_dword(bus, dev_fn, where, val)) ;
}
#endif /* < 2.4 && CONFIG_PCI */

const char * _RP lis_pcibios_strerror(int error)
{
    switch (error)
    {
    case PCIBIOS_SUCCESSFUL:		return("PCIBIOS_SUCCESSFUL") ;
    case PCIBIOS_FUNC_NOT_SUPPORTED:	return("PCIBIOS_FUNC_NOT_SUPPORTED") ;
    case PCIBIOS_BAD_VENDOR_ID:		return("PCIBIOS_BAD_VENDOR_ID") ;
    case PCIBIOS_DEVICE_NOT_FOUND:	return("PCIBIOS_DEVICE_NOT_FOUND") ;
    case PCIBIOS_BAD_REGISTER_NUMBER:	return("PCIBIOS_BAD_REGISTER_NUMBER") ;
    case PCIBIOS_SET_FAILED:		return("PCIBIOS_SET_FAILED") ;
    case PCIBIOS_BUFFER_TOO_SMALL:	return("PCIBIOS_BUFFER_TOO_SMALL") ;
    }

    {
	static char msg[100] ;	/* not very re-entrant */
	sprintf(msg, "PCIBIOS Error #%d", error) ;
	return(msg) ;
    }
}

/*
 * End of old PCI BIOS routines
 */


#if defined(CONFIG_PCI)

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,1,0)
struct pci_dev  * _RP lis_osif_pci_find_device(unsigned int vendor,
				 unsigned int device,
                                 struct pci_dev *from)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    return(pci_find_device(vendor, device, from)) ;
#else
struct pci_dev * temp_dev;
    temp_dev = pci_get_device(vendor, device, from);
    if (temp_dev)
       pci_dev_put(temp_dev);
    return(temp_dev) ;
#endif
}
#endif /* > 2.1 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
struct pci_dev  * _RP lis_osif_pci_find_class(unsigned int class,
					 struct pci_dev *from)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    return(pci_find_class(class, from)) ;
#else
    return(pci_get_class(class, from)) ;
#endif
}
#endif

struct pci_dev  * _RP lis_osif_pci_find_slot(unsigned int bus, 
					unsigned int devfn)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    return(pci_find_slot(bus, devfn)) ;
#else
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0))
    return(pci_get_slot((struct pci_bus *)bus, devfn)) ;
#else
    return(NULL);
#endif
#endif
}

int      _RP lis_osif_pci_read_config_byte(struct pci_dev *dev, 
					u8 where, u8 *val)
{
    return(pci_read_config_byte(dev, where, val)) ;
}

int      _RP lis_osif_pci_read_config_word(struct pci_dev *dev, 
					u8 where, u16 *val)
{
    return(pci_read_config_word(dev, where, val)) ;
}

int      _RP lis_osif_pci_read_config_dword(struct pci_dev *dev, 
					u8 where, u32 *val)
{
    return(pci_read_config_dword(dev, where, val)) ;
}

int      _RP lis_osif_pci_write_config_byte(struct pci_dev *dev, 
					u8 where, u8 val)
{
    return(pci_write_config_byte(dev, where, val)) ;
}

int      _RP lis_osif_pci_write_config_word(struct pci_dev *dev, 
					u8 where, u16 val)
{
    return(pci_write_config_word(dev, where, val)) ;
}

int      _RP lis_osif_pci_write_config_dword(struct pci_dev *dev, 
					u8 where, u32 val)
{
    return(pci_write_config_dword(dev, where, val)) ;
}

void     _RP lis_osif_pci_set_master(struct pci_dev *dev)
{
    pci_set_master(dev) ;
}

int  _RP lis_osif_pci_enable_device (struct pci_dev *dev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
    return(0) ;					/* pretend success */
#else
    return (pci_enable_device (dev));
#endif
}

void  _RP lis_osif_pci_disable_device (struct pci_dev *dev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
    return ;					/* pretend success */
#else
    pci_disable_device (dev);
#endif
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
int  _RP lis_osif_pci_module_init( void *p )
{
    return ( 0 );
}

void  _RP lis_osif_pci_unregister_driver( void *p )
{
}
#else						/* 2.4 kernel */
int  _RP lis_osif_pci_module_init( struct pci_driver *p )
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
    return pci_module_init( p );
#else
    return pci_register_driver( p );
#endif
}

void  _RP lis_osif_pci_unregister_driver( struct pci_driver *p )
{
    pci_unregister_driver( p );
}
#endif /* < 2.3 */
#endif          /* CONFIG_PCI */

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)

	 /***************************************
	 *        PCI DMA Mapping Fcns		*
	 ***************************************/

void * _RP lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
	                                  dma_addr_t *dma_handle)
{
#ifdef CONFIG_PCI
    return(pci_alloc_consistent(hwdev, size, dma_handle));
#else
    return 0;
#endif
}

void  _RP lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size,
	                                void *vaddr, dma_addr_t dma_handle)
{
#ifdef CONFIG_PCI
    pci_free_consistent(hwdev, size, vaddr, dma_handle);
#else
    return;
#endif
}
#endif /* >= 2.4 */
#endif /* end kernel 6.0.0. check */

#ifdef CONFIG_PCI

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)
dma_addr_t  _RP lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr,
	                                        size_t size, int direction)
{
    return(pci_map_single(hwdev, ptr, size, direction)) ;
}

void  _RP lis_osif_pci_unmap_single(struct pci_dev *hwdev,
			    dma_addr_t dma_addr, size_t size, int direction)
{
    pci_unmap_single(hwdev, dma_addr, size, direction) ;
}

int  _RP lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg,
	                             int nents, int direction)
{
    return(pci_map_sg(hwdev, sg, nents, direction)) ;
}

void  _RP lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg,
	                                int nents, int direction)
{
    pci_unmap_sg(hwdev, sg, nents, direction) ;
}

void  _RP lis_osif_pci_dma_sync_single(struct pci_dev *hwdev,
			   dma_addr_t dma_handle, size_t size, int direction)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    if ((direction == PCI_DMA_BIDIRECTIONAL) || 
	(direction == PCI_DMA_TODEVICE))
    {
	pci_dma_sync_single_for_device(hwdev, dma_handle, size, direction) ;
    }
    if ((direction == PCI_DMA_BIDIRECTIONAL) ||
	(direction == PCI_DMA_FROMDEVICE))
    {
	pci_dma_sync_single_for_cpu(hwdev, dma_handle, size, direction) ;
    }
#else
    pci_dma_sync_single(hwdev, dma_handle, size, direction) ;
#endif
}

void  _RP lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev,
			   struct scatterlist *sg, int nelems, int direction)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    if ((direction == PCI_DMA_BIDIRECTIONAL) || 
	(direction == PCI_DMA_TODEVICE))
    {
	pci_dma_sync_sg_for_device(hwdev, sg, nelems, direction) ;
    }
    if ((direction == PCI_DMA_BIDIRECTIONAL) ||
	(direction == PCI_DMA_FROMDEVICE))
    {
	pci_dma_sync_sg_for_cpu(hwdev, sg, nelems, direction) ;
    }
#else
    pci_dma_sync_sg(hwdev, sg, nelems, direction) ;
#endif
}

int  _RP lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    return(pci_set_dma_mask(hwdev, mask)) ;
#else
    return(pci_dma_supported(hwdev, mask)) ;
#endif
}

int  _RP lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
    return(pci_set_dma_mask(hwdev, mask)) ;
}

#endif /* end kernel 6.0.0. check */

dma_addr_t  _RP lis_osif_sg_dma_address(struct scatterlist *sg)
{
    return(sg_dma_address(sg)) ;
}

size_t  _RP lis_osif_sg_dma_len(struct scatterlist *sg)
{
    return(sg_dma_len(sg)) ;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,0,0)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)      /* 2.4.13 or later */

dma_addr_t  _RP lis_osif_pci_map_page(struct pci_dev *hwdev,
				struct page *page, unsigned long offset,
				size_t size, int direction)
{
    return(pci_map_page(hwdev, page, offset, size, direction)) ;
}

void  _RP lis_osif_pci_unmap_page(struct pci_dev *hwdev,
				dma_addr_t dma_address, size_t size,
				int direction)
{
    pci_unmap_page(hwdev, dma_address, size, direction) ;
}

#endif /* end kernel 6.0.0. check */

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,5,0) 
int  _RP lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
    return(pci_dac_set_dma_mask(hwdev, mask)) ;
}

int  _RP lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask)
{
    return(pci_dac_dma_supported(hwdev, mask)) ;
}


#if (!defined(_PPC64_LIS_))
dma64_addr_t  _RP lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev,
			struct page *page, unsigned long offset,
			int direction)
{
    return(pci_dac_page_to_dma(pdev, page, offset, direction)) ;
}

struct page * _RP lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev,
					dma64_addr_t dma_addr)
{
    return(pci_dac_dma_to_page(pdev, dma_addr)) ;
}

unsigned long  _RP lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev,
					dma64_addr_t dma_addr)
{
    return(pci_dac_dma_to_offset(pdev, dma_addr)) ;
}

void  _RP lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev,
			    dma64_addr_t dma_addr, size_t len, int direction)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    if ((direction == PCI_DMA_BIDIRECTIONAL) || 
	(direction == PCI_DMA_TODEVICE))
    {
	pci_dac_dma_sync_single_for_device(pdev, dma_addr, len, direction) ;
    }
    if ((direction == PCI_DMA_BIDIRECTIONAL) ||
	(direction == PCI_DMA_FROMDEVICE))
    {
	pci_dac_dma_sync_single_for_cpu(pdev, dma_addr, len, direction) ;
    }
#else
    pci_dac_dma_sync_single(pdev, dma_addr, len, direction) ;
#endif
}
#endif		/* not _PPC64_LIS_ */
#endif			/* 2.5.0 */
#endif                                  /* 2.4.13 */

#endif          /* CONFIG_PCI */

/************************************************************************
*                        IRQ Routines                                   *
************************************************************************/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
/*
 * Because of the register parameter passing in the 2.6 kernel we need
 * an irq routine that is compiled with the kernel's register style to
 * pass to the kernel's request_irq function.  This routine then calls
 * the LiS style irq handler using the LiS parameter passing convention.
 *
 * So we construct a "device id" as the pointer to one of these structures
 * in which we save the user's original dev_id parameter which we then
 * pass back to the user's interrupt handler.  We are going to appropriate
 * the lis_incr_lock spin lock for our list protection.
 *
 * We make a list of these structures that can be cleaned up at LiS
 * termination time.
 */
typedef struct lis_devid
{
    struct lis_devid	*link ;
    lis_int_handler	 handler ;
    void		*dev_id ;		/* user's dev_id param */
    int			 irq ;
} lis_devid_t ;

extern lis_spin_lock_t  lis_incr_lock ;

lis_devid_t	*lis_devid_list ;

#if !defined(__s390__)
/*
 * This routine is compiled with the kernel's parameter passing convention.
 * It calls the STREAMS driver's handler using LiS parameter passing
 * convention.
 */
static irqreturn_t lis_khandler(int irq, void *dev_id, struct pt_regs *regs)
{
    lis_devid_t		*dv = (lis_devid_t *) dev_id ;

    return(dv->handler(irq, dv->dev_id, regs)) ;
}
#endif
/*
 * Called at LiS termination time to clean up the list
 */
void lis_free_devid_list(void)
{
    lis_devid_t		*dv = lis_devid_list ;
    lis_devid_t		*nxt ;

    lis_devid_list = NULL ;

    while (dv != NULL)
    {
	nxt = dv->link ;
	if (dv->handler && dv->dev_id && dv->irq)
	{
	    printk("LiS freeing IRQ%u\n", dv->irq) ;
#if !defined(__s390__)
	    free_irq(dv->irq, dv) ;
#endif
	}
	FREE(dv) ;
	dv = nxt ;
    }
}

#endif /* > 2.5 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)

int      lis_irqreturn_handled = IRQ_HANDLED;
int      lis_irqreturn_not_handled = IRQ_NONE;

#else

int      lis_irqreturn_handled = 1;
int      lis_irqreturn_not_handled = 0;

#endif

int  _RP lis_request_irq(unsigned int  irq,
		     lis_int_handler handler,
	             unsigned long flags,
		     const char   *device,
		     void         *dev_id)
{
#if !defined(__s390__)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    int			  ret ;
    lis_devid_t		 *dv ;
    lis_flags_t		  psw ;

    dv = ALLOC(sizeof(*dv)) ;
    if (dv == NULL)
	return(-ENOMEM) ;

    dv->handler = handler ;
    dv->dev_id  = dev_id ;
    dv->irq     = irq ;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    ret = request_irq(irq, lis_khandler, flags, device, dv) ;
#else
    ret = request_irq(irq, (irq_handler_t) lis_khandler, flags, device, dv) ;
#endif
    if (ret == 0)
    {
	lis_spin_lock_irqsave(&lis_incr_lock, &psw) ;
	dv->link = lis_devid_list ;
	lis_devid_list = dv ;
	lis_spin_unlock_irqrestore(&lis_incr_lock, &psw) ;
    }
    else
	FREE(dv) ;

    return(ret) ;
#else
    typedef void	(*khandler) (int, void *, struct pt_regs *) ;
    khandler	kparam = (khandler) handler ;

    return(request_irq(irq, kparam, flags, device, dev_id)) ;
#endif
#else
return(0); 
#endif

}

void  _RP lis_free_irq(unsigned int irq, void *dev_id)
{
#if !defined(__s390__)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    lis_devid_t		*dv = lis_devid_list ;
    lis_flags_t		 psw ;

    lis_spin_lock_irqsave(&lis_incr_lock, &psw) ;
    for (; dv != NULL; dv = dv->link)
    {
	if (dv->dev_id == dev_id)
	{
	    free_irq(irq, dv) ;
	    dv->handler = NULL ;
	    dv->dev_id  = NULL ;
	    dv->irq     = 0 ;
	    lis_spin_unlock_irqrestore(&lis_incr_lock, &psw) ;
	    FREE(dv);
	    return ;
	}
    }
    lis_spin_unlock_irqrestore(&lis_incr_lock, &psw) ;
#else
    free_irq(irq, dev_id) ;
#endif
#endif
}

void  _RP lis_enable_irq(unsigned int irq)
{
#if !defined(__s390__)
    enable_irq(irq) ;
#endif
}

void  _RP lis_disable_irq(unsigned int irq)
{
#if !defined(__s390__)
    disable_irq(irq) ;
#endif
}

void  _RP lis_osif_cli( void )
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    lis_splstr() ;
#else
    cli( );
#endif

}
void  _RP lis_osif_sti( void )
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    lis_spl0() ;
#else
    sti( );
#endif
}



/************************************************************************
*                       Memory Mapping Routines                         *
*************************************************************************
*									*
* These are a subset of what is available.  Feel free to add more.	*
*									*
* The 2.0.x routines will call 2.2.x routines if we are being compiled	*
* for 2.2.x, and vice versa.  This gives drivers something in the way	*
* of version independence.  Wish the kernel guys would tend to that	*
* a bit more.								*
*									*
************************************************************************/

void * _RP lis_ioremap(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
    return(vremap(offset, size)) ;
#elif !defined(__s390__)
    return(ioremap(offset, size)) ;
#else
    return(NULL) ;
#endif
}

void * _RP lis_ioremap_nocache(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
    return(vremap(offset, size)) ;
#elif !defined(__s390__)
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)    
    return(ioremap_nocache(offset, size)) ;
#else
    return(ioremap_cache(offset, size)) ; // default behavior moved to nocache for this
#endif    
#else
    return(NULL) ;
#endif
}

void _RP lis_iounmap(void *ptr)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
    vfree(ptr) ;
#elif !defined(__s390__)
    iounmap(ptr) ;
#endif
}

void * _RP lis_vremap(unsigned long offset, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
    return(vremap(offset, size)) ;
#elif !defined(__s390__)
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)    
    return(ioremap_nocache(offset, size)) ;
#else
    return(ioremap_cache(offset, size)) ; // default behavior moved to nocache for this
#endif
#else
    return(NULL) ;
#endif
}

unsigned long  _RP lis_virt_to_phys(volatile void *addr)
{
    return(virt_to_phys(addr)) ;
}

void * _RP lis_phys_to_virt(unsigned long addr)
{
    return(phys_to_virt(addr)) ;
}


/************************************************************************
*                       I/O Ports Routines                              *
************************************************************************/

int  _RP lis_check_region(unsigned int from, unsigned int extent)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    if (request_region(from,extent,"LiS-checking"))
    {
	release_region(from,extent) ;
	return(0) ;
    }
    return(-EBUSY) ;
#else
    return(check_region(from, extent)) ;
#endif
}

void  _RP lis_request_region(unsigned int from,
			 unsigned int extent,
			 const char  *name)
{
    request_region(from, extent, name) ;
}

void  _RP lis_release_region(unsigned int from, unsigned int extent)
{
    release_region(from, extent) ;
}

/************************************************************************
*                    Memory Allocation Routines                         *
************************************************************************/

void * _RP lis_kmalloc(size_t nbytes, int type)
{
    return(kmalloc(nbytes, type)) ;
}

void   _RP lis_kfree(const void *ptr)
{
    kfree((void *)ptr) ;
}

void * _RP lis_vmalloc(unsigned long size)
{
    return(vmalloc(size)) ;
}

void   _RP lis_vfree(void *ptr)
{
    vfree(ptr) ;
}

/************************************************************************
*                        DMA Routines                                   *
************************************************************************/

int   _RP lis_request_dma(unsigned int dma_nr, const char *device_id)
{
#if defined(MAX_DMA_CHANNELS)
    return(request_dma(dma_nr, device_id)) ;
#else
    return(-ENXIO) ;
#endif
}

void  _RP lis_free_dma(unsigned int dma_nr)
{
#if defined(MAX_DMA_CHANNELS)
    free_dma(dma_nr) ;
#endif
}

/************************************************************************
*                         Delay Routines                                *
************************************************************************/

void  _RP lis_udelay(long micro_secs)
{
    udelay(micro_secs) ;
}

unsigned long  _RP lis_jiffies(void)
{
    return(jiffies) ;
}

/************************************************************************
*                         Time Routines                                 *
************************************************************************/
void  _RP lis_osif_do_gettimeofday( struct timeval *tp )
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)	
    do_gettimeofday(tp) ;
#else    
    struct timespec64 now;

    ktime_get_real_ts64
	    (&now);
    tp->tv_sec = now.tv_sec;
    tp->tv_usec = now.tv_nsec/1000;
#endif
}

void  _RP lis_osif_do_settimeofday( struct timeval *tp )
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0) // No timespec in 5.11 or higher
    	struct timespec ts ;

    ts.tv_sec = tp->tv_sec ;
    ts.tv_nsec = tp->tv_usec * 1000 ;
#endif    
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
    do_settimeofday(&ts) ;
    
#else   // for deprecated ddo_settimeofday64()
     struct timespec64 ts64;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
     ts64 = timespec_to_timespec64(ts);
#else  // the kernel is now fullu 64-bit timespec
    ts64.tv_sec = tp->tv_sec ;
    ts64.tv_nsec = tp->tv_usec * 1000 ;
#endif     
     do_settimeofday64(&ts64);
#endif	

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)

    do_settimeofday(tp) ;

#endif
}


/************************************************************************
*                         Printing Routines                             *
************************************************************************/
int  _RP lis_printk(const char *fmt, ...)
{
    extern char	    lis_cmn_err_buf[];
    va_list	    args;
    int		    ret ;

    va_start (args, fmt);
    ret = vsprintf (lis_cmn_err_buf, fmt, args);
    va_end (args);

    printk("%s", lis_cmn_err_buf) ;
    return(ret) ;
}

int  _RP lis_sprintf(char *bfr, const char *fmt, ...)
{
    va_list	    args;
    int		    ret ;

    va_start (args, fmt);
    ret = vsprintf (bfr, fmt, args);
    va_end (args);

    return(ret) ;
}

int  _RP lis_vsprintf(char *bfr, const char *fmt, va_list args)
{
    return(vsprintf (bfr, fmt, args));
}

/************************************************************************
*                      Sleep/Wakeup Routines                            *
* The sleep_on_ routines are depricated in 2.5.  It is better to have   *
* the drivers converted to use the wait_event_ interface (also          *
* available prior to 2.5) than to convert sleep_on_ calls to            *
* wait_event_ calls.                                                    *
************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
void  _RP lis_sleep_on(OSIF_WAIT_Q_ARG)
{
    sleep_on(wq) ;
}

void  _RP lis_interruptible_sleep_on(OSIF_WAIT_Q_ARG)
{
    interruptible_sleep_on(wq) ;
}

void  _RP lis_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout)
{
    sleep_on_timeout(wq, timeout) ;
}

void  _RP lis_interruptible_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout)
{
    interruptible_sleep_on_timeout(wq, timeout) ;
}
#endif /* < 2.5 */

void  _RP lis_wait_event(OSIF_WAIT_E_ARG, int condition)
{
    wait_event(wq, condition) ;
}

void  _RP lis_wait_event_interruptible(OSIF_WAIT_E_ARG, int condition)
{
    wait_event_interruptible(wq, condition) ;
}

void  _RP lis_wake_up(OSIF_WAIT_Q_ARG)
{
    wake_up(wq) ;
}

void  _RP lis_wake_up_interruptible(OSIF_WAIT_Q_ARG)
{
    wake_up_interruptible(wq) ;
}

/************************************************************************
*                             Timer Routines                            *
************************************************************************/
void _RP
lis_add_timer(struct timer_list * timer)
{
    add_timer(timer);
}

int  _RP
lis_del_timer(struct timer_list * timer)
{
    return del_timer(timer);
}

/*
 * Prototype in dki.h (with other timer stuff), not osif.h
 */
unsigned _RP lis_usectohz(unsigned usec)
{
    return( usec / (1000000/HZ) ) ;
}

/************************************************************************
*                        Wrapped Functions                              *
************************************************************************/

#define __real_strcpy	strcpy
#define __real_strncpy	strncpy
#define __real_strcat	strcat
#define __real_strncat	strncat
#define __real_strcmp	strcmp
#define __real_strncmp	strncmp
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
#define __real_strnicmp	strnicmp
#endif
#define __real_strchr	strchr
#define __real_strrchr	strrchr
#define __real_strstr	strstr
#define __real_strlen	strlen
#define __real_memset	memset
#define __real_memcpy	memcpy
#define __real_memcmp	memcmp

char * _RP __wrap_strcpy(char *d,const char *s)
{
    return(__real_strcpy(d,s));
}

char * _RP __wrap_strncpy(char *d,const char *s, __kernel_size_t l)
{
    return(__real_strncpy(d,s,l));
}

char * _RP __wrap_strcat(char *d, const char *s)
{
    return(__real_strcat(d,s));
}

char * _RP __wrap_strncat(char *d, const char *s, __kernel_size_t l)
{
    return(__real_strncat(d,s,l));
}

int _RP __wrap_strcmp(const char *a,const char *b)
{
    return(__real_strcmp(a,b));
}

int _RP __wrap_strncmp(const char *a,const char *b,__kernel_size_t l)
{
    return(__real_strncmp(a,b,l));
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
int _RP __wrap_strnicmp(const char *a, const char *b, __kernel_size_t l)
{
    return(__real_strnicmp(a,b,l));
}
#endif
char * _RP __wrap_strchr(const char *s,int c)
{
    return(__real_strchr(s,c));
}

char * _RP __wrap_strrchr(const char *s,int c)
{
    return(__real_strrchr(s,c));
}

char * _RP __wrap_strstr(const char *a,const char *b)
{
    return(__real_strstr(a,b));
}

__kernel_size_t _RP __wrap_strlen(const char *s)
{
    return(__real_strlen(s));
}

void * _RP __wrap_memset(void *d,int v,__kernel_size_t l)
{
    return(__real_memset(d,v,l));
}

void * _RP __wrap_memcpy(void *d,const void *s,__kernel_size_t l)
{
    return(__real_memcpy(d,s,l));
}

int _RP __wrap_memcmp(const void *a,const void *b,__kernel_size_t l)
{
    return(__real_memcmp(a,b,l));
}


int _RP __wrap_sprintf(char *p, const char *fmt, ...)
{
    va_list	 args;
    int		 ret ;

    va_start (args, fmt);
    ret = vsprintf (p, fmt, args);
    va_end (args);
    return(ret) ;
}

int _RP __wrap_snprintf(char *p, size_t len, const char *fmt, ...)
{
    va_list	 args;
    int		 ret ;

    va_start (args, fmt);
    ret = vsnprintf (p, len, fmt, args);
    va_end (args);
    return(ret) ;
}

int _RP __wrap_vsprintf(char *p, const char *fmt, va_list args)
{
    return(vsprintf (p, fmt, args));
}

int _RP __wrap_vsnprintf(char *p, size_t len, const char *fmt, va_list args)
{
    return(vsnprintf (p, len, fmt, args));
}


