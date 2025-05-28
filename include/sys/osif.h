/************************************************************************
*                                                                       *
* Copyright 2022 - IBM Inc. All rights reserved                         *
* SPDX-License-Identifier: LGPL-2.1                                     *
*                                                                       *
*                      Operating System Interface                       *
*************************************************************************
*									*
* These are routines that call kernel routines indirectly.  This allows	*
* driver writers to reference the lis_... names in compiled code	*
* and the osif.c module itself will use the mangled names of the	*
* kernel routines when "modversions" is set.				*
*									*
* Include this file AFTER any linux includes.				*
*									*
* This file is only interpreted for LINUX builds.  User-level and other	*
* portable builds bypass this file.					*
*									*
************************************************************************/
#if defined(LINUX) && !defined(OSIF_H)
#define OSIF_H		/* file included */

#ident "@(#) CSLiS osif.h 7.112 2025-05-28 15:30:00 "
#include <sys/LiS/genconf.h>
#include <linux/version.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif
#if (defined(RHEL_RELEASE_CODE) && RHEL_RELEASE_CODE < 2305)  //For RHEL 9 update 12-2022
#include <stdarg.h>                    /* for va_list */
#endif
#include <linux/wait.h>			/* for struct wait_queue */
#include <linux/timer.h>                /* for struct timer_list */
#include <linux/time.h>
#ifdef CONFIG_PCI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#define ERANGE 34
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,3,18)
#ifndef PCI_STD_NUM_BARS
#define PCI_STD_NUM_BARS	6	/* Number of standard BARs */
#endif
#endif
#if (defined(_S390X_LIS_) || defined(_PPC64_LIS_) )
#if (defined(RHEL_RELEASE_CODE) && RHEL_RELEASE_CODE > 2309)
#define _LINUX_PROPERTY_H_  // omit property.h
#endif		
#endif
#include <linux/pci.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,1,0)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
#include <asm/scatterlist.h>
#else
#include <linux/scatterlist.h>
#endif
#endif
#endif

#if !defined(INCL_FROM_OSIF_DRIVER)

/*
 * Redefine the "official" kernel names of these routines to the
 * "lis_..." names.
 *
 * Modversions has already defined most of these names to be the
 * version-mangled form.  So we have to test each one to see if
 * it has already been defined before redefining to the lis_... form.
 * This is messy but saves many warning messages when compiling
 * with modversions.
 */

#ifdef pcibios_present
#undef pcibios_present
#endif
#define	pcibios_present			lis_pcibios_present
int lis_pcibios_present(void) _RP;

#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)) && CONFIG_PCI)
#ifdef pcibios_init
#undef pcibios_init
#endif
#define	pcibios_init			lis_pcibios_init
void lis_pcibios_init(void) _RP;

#ifdef pcibios_find_class
#undef pcibios_find_class
#endif
#define	pcibios_find_class		lis_pcibios_find_class
int lis_pcibios_find_class(unsigned int   class_code,
			    unsigned short index,
                            unsigned char *bus,
			    unsigned char *dev_fn) _RP;

#ifdef pcibios_find_device
#undef pcibios_find_device
#endif
#define	pcibios_find_device		lis_pcibios_find_device
int lis_pcibios_find_device(unsigned short vendor,
			     unsigned short dev_id,
			     unsigned short index,
			     unsigned char *bus,
			     unsigned char *dev_fn) _RP;

#ifdef pcibios_read_config_byte
#undef pcibios_read_config_byte
#endif
#define	pcibios_read_config_byte	lis_pcibios_read_config_byte
int lis_pcibios_read_config_byte(unsigned char  bus,
				  unsigned char  dev_fn,
				  unsigned char  where,
				  unsigned char *val) _RP;

#ifdef pcibios_read_config_word
#undef pcibios_read_config_word
#endif
#define	pcibios_read_config_word	lis_pcibios_read_config_word
int lis_pcibios_read_config_word(unsigned char   bus,
				  unsigned char   dev_fn,
				  unsigned char   where,
				  unsigned short *val) _RP;

#ifdef pcibios_read_config_dword
#undef pcibios_read_config_dword
#endif
#define	pcibios_read_config_dword	lis_pcibios_read_config_dword
int lis_pcibios_read_config_dword(unsigned char  bus,
				   unsigned char  dev_fn,
				   unsigned char  where,
				   unsigned int  *val) _RP;

#ifdef pcibios_write_config_byte
#undef pcibios_write_config_byte
#endif
#define	pcibios_write_config_byte	lis_pcibios_write_config_byte
int lis_pcibios_write_config_byte(unsigned char  bus,
				   unsigned char  dev_fn,
				   unsigned char  where,
				   unsigned char  val) _RP;

#ifdef pcibios_write_config_word
#undef pcibios_write_config_word
#endif
#define	pcibios_write_config_word	lis_pcibios_write_config_word
int lis_pcibios_write_config_word(unsigned char   bus,
				   unsigned char   dev_fn,
				   unsigned char   where,
				   unsigned short  val) _RP;

#ifdef pcibios_write_config_dword
#undef pcibios_write_config_dword
#endif
#define	pcibios_write_config_dword	lis_pcibios_write_config_dword
int lis_pcibios_write_config_dword(unsigned char  bus,
				    unsigned char  dev_fn,
				    unsigned char  where,
				    unsigned int   val) _RP;

#endif /* < 2.4 && CONFIG_PCI */
#ifdef pcibios_strerror
#undef pcibios_strerror
#endif
#define	pcibios_strerror		lis_pcibios_strerror
const char *lis_pcibios_strerror(int error) _RP;

#ifdef CONFIG_PCI

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,1,0)
#ifdef pci_find_device
#undef pci_find_device
#endif
#define	pci_find_device			lis_osif_pci_find_device
struct pci_dev	*lis_osif_pci_find_device(unsigned int vendor,
				 unsigned int device,
				 struct pci_dev *from)_RP;
#endif /* > 2.1 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#ifdef pci_find_class
#undef pci_find_class
#endif
#define	pci_find_class			lis_osif_pci_find_class
struct pci_dev	*lis_osif_pci_find_class(unsigned int class,
				 struct pci_dev *from)_RP;
#endif /* < 2.5 */

#ifdef pci_find_slot
#undef pci_find_slot
#endif
#define	pci_find_slot			lis_osif_pci_find_slot
struct pci_dev	*lis_osif_pci_find_slot(unsigned int bus, 
				unsigned int devfn)_RP;

#ifdef pci_read_config_byte
#undef pci_read_config_byte
#endif
#define	pci_read_config_byte		lis_osif_pci_read_config_byte
int	lis_osif_pci_read_config_byte(struct pci_dev *dev, 
				u8 where, u8 *val)_RP;

#ifdef pci_read_config_word
#undef pci_read_config_word
#endif
#define	pci_read_config_word		lis_osif_pci_read_config_word
int	lis_osif_pci_read_config_word(struct pci_dev *dev, 
				u8 where, u16 *val)_RP;

#ifdef pci_read_config_dword
#undef pci_read_config_dword
#endif
#define	pci_read_config_dword		lis_osif_pci_read_config_dword
int	lis_osif_pci_read_config_dword(struct pci_dev *dev, 
				u8 where, u32 *val)_RP;

#ifdef pci_write_config_byte
#undef pci_write_config_byte
#endif
#define	pci_write_config_byte		lis_osif_pci_write_config_byte
int	lis_osif_pci_write_config_byte(struct pci_dev *dev, 
				u8 where, u8 val)_RP;

#ifdef pci_write_config_word
#undef pci_write_config_word
#endif
#define	pci_write_config_word		lis_osif_pci_write_config_word
int	lis_osif_pci_write_config_word(struct pci_dev *dev, 
				u8 where, u16 val)_RP;

#ifdef pci_write_config_dword
#undef pci_write_config_dword
#endif
#define	pci_write_config_dword		lis_osif_pci_write_config_dword
int	lis_osif_pci_write_config_dword(struct pci_dev *dev, 
				u8 where, u32 val)_RP;

#ifdef pci_set_master
#undef pci_set_master
#endif
#define	pci_set_master			lis_osif_pci_set_master
void	lis_osif_pci_set_master(struct pci_dev *dev)_RP;

#ifdef pci_enable_device
#undef pci_enable_device
#endif
#define pci_enable_device               lis_osif_pci_enable_device
int     lis_osif_pci_enable_device (struct pci_dev *dev)_RP;

#ifdef pci_disable_device
#undef pci_disable_device
#endif
#define pci_disable_device               lis_osif_pci_disable_device
void    lis_osif_pci_disable_device (struct pci_dev *dev)_RP;

#ifdef pci_module_init
#undef pci_module_init
#endif
#define pci_module_init       lis_osif_pci_module_init
#ifdef pci_unregister_driver
#undef pci_unregister_driver
#endif
#define pci_unregister_driver lis_osif_pci_unregister_driver
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,3,0)
int	lis_osif_pci_module_init( struct pci_driver *p )_RP;
void	lis_osif_pci_unregister_driver( struct pci_driver *p )_RP;
#else						/* older kernel */
int	lis_osif_pci_module_init( void *p )_RP;
void	lis_osif_pci_unregister_driver( void *p )_RP;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#ifdef pci_alloc_consistent
#undef pci_alloc_consistent
#endif
#define pci_alloc_consistent lis_osif_pci_alloc_consistent
extern void *lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
				dma_addr_t *dma_handle)_RP;

#ifdef pci_free_consistent
#undef pci_free_consistent
#endif
#define pci_free_consistent lis_osif_pci_free_consistent
extern void lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size,
                                void *vaddr, dma_addr_t dma_handle)_RP;
#endif /* >= 2.4 */

#ifdef pci_map_single
#undef pci_map_single
#endif
#define pci_map_single lis_osif_pci_map_single
extern dma_addr_t lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr,
				size_t size, int direction)_RP;

#ifdef pci_unmap_single
#undef pci_unmap_single
#endif
#define pci_unmap_single lis_osif_pci_unmap_single
extern void lis_osif_pci_unmap_single(struct pci_dev *hwdev,
			    dma_addr_t dma_addr, size_t size, int direction)_RP;

#ifdef pci_map_sg
#undef pci_map_sg
#endif
#define pci_map_sg lis_osif_pci_map_sg
extern int lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg,
				int nents, int direction)_RP;

#ifdef pci_unmap_sg
#undef pci_unmap_sg
#endif
#define pci_unmap_sg lis_osif_pci_unmap_sg
extern void lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg,
				int nents, int direction)_RP;

#ifdef pci_dma_sync_single
#undef pci_dma_sync_single
#endif
#define pci_dma_sync_single lis_osif_pci_dma_sync_single
extern void lis_osif_pci_dma_sync_single(struct pci_dev *hwdev,
			dma_addr_t dma_handle, size_t size, int direction)_RP;

#ifdef pci_dma_sync_sg
#undef pci_dma_sync_sg
#endif
#define pci_dma_sync_sg lis_osif_pci_dma_sync_sg
extern void lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev,
		   struct scatterlist *sg, int nelems, int direction)_RP;

#ifdef pci_dma_supported
#undef pci_dma_supported
#endif
#define pci_dma_supported lis_osif_pci_dma_supported
extern int lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask)_RP;

#ifdef pci_set_dma_mask
#undef pci_set_dma_mask
#endif
#define pci_set_dma_mask lis_osif_pci_set_dma_mask
extern int lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask)_RP;

#ifdef sg_dma_address
#undef sg_dma_address
#endif
#define sg_dma_address lis_osif_sg_dma_address
extern dma_addr_t lis_osif_sg_dma_address(struct scatterlist *sg)_RP;

#ifdef sg_dma_len
#undef sg_dma_len
#endif
#define sg_dma_len lis_osif_sg_dma_len
extern size_t lis_osif_sg_dma_len(struct scatterlist *sg)_RP;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,13)
#ifdef pci_map_page
#undef pci_map_page
#endif
#define pci_map_page lis_osif_pci_map_page
extern dma_addr_t lis_osif_pci_map_page(struct pci_dev *hwdev,
				struct page *page, unsigned long offset,
				size_t size, int direction)_RP;

#ifdef pci_unmap_page
#undef pci_unmap_page
#endif
#define pci_unmap_page lis_osif_pci_unmap_page
extern void lis_osif_pci_unmap_page(struct pci_dev *hwdev,
				dma_addr_t dma_address, size_t size,
				int direction)_RP;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#ifdef pci_dac_set_dma_mask
#undef pci_dac_set_dma_mask
#endif
#define pci_dac_set_dma_mask lis_osif_pci_dac_set_dma_mask
extern int lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask)_RP;

#ifdef pci_dac_dma_supported
#undef pci_dac_dma_supported
#endif
#define pci_dac_dma_supported lis_osif_pci_dac_dma_supported
extern int lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask)_RP;

#if (!defined(_PPC64_LIS_))
#ifdef pci_dac_page_to_dma
#undef pci_dac_page_to_dma
#endif
#define pci_dac_page_to_dma lis_osif_pci_dac_page_to_dma
extern dma64_addr_t lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev,
			struct page *page, unsigned long offset,
			int direction)_RP;

#ifdef pci_dac_dma_to_page
#undef pci_dac_dma_to_page
#endif
#define pci_dac_dma_to_page lis_osif_pci_dac_dma_to_page
extern struct page *lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev,
					dma64_addr_t dma_addr)_RP;

#ifdef pci_dac_dma_to_offset
#undef pci_dac_dma_to_offset
#endif
#define pci_dac_dma_to_offset lis_osif_pci_dac_dma_to_offset
extern unsigned long lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev,
					dma64_addr_t dma_addr)_RP;

#ifdef pci_dac_dma_sync_single
#undef pci_dac_dma_sync_single
#endif
#define pci_dac_dma_sync_single lis_osif_pci_dac_dma_sync_single
extern void lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev,
		    dma64_addr_t dma_addr, size_t len, int direction)_RP;
#endif /* not _PPC64_LIS_ */
#endif		/* 2.5.0 */
#endif /* >= 2.4.13 */

#endif /* CONFIG_PCI */

#ifdef request_irq
#undef request_irq
#endif
#define	request_irq			lis_request_irq
#ifdef free_irq
#undef free_irq
#endif
#define	free_irq			lis_free_irq
#ifdef disable_irq
#undef disable_irq
#endif
#define	disable_irq			lis_disable_irq
#ifdef enable_irq
#undef enable_irq
#endif
#define	enable_irq			lis_enable_irq
#ifdef cli
# undef cli
#endif
#define cli                   		lis_osif_cli
#ifdef sti
# undef sti
#endif
#define sti                   		lis_osif_sti



#ifdef ioremap
#undef ioremap
#endif
#define	ioremap				lis_ioremap
#ifdef ioremap_nocache
#undef ioremap_nocache
#endif
#define	ioremap_nocache			lis_ioremap_nocache
#ifdef iounmap
#undef iounmap
#endif
#define	iounmap				lis_iounmap
#ifdef vremap
#undef vremap
#endif
#define	vremap				lis_vremap
#ifdef virt_to_phys
#undef virt_to_phys
#endif
#define virt_to_phys			lis_virt_to_phys
#ifdef phys_to_virt
#undef phys_to_virt
#endif
#define phys_to_virt			lis_phys_to_virt

#ifdef check_region
#undef check_region
#endif
#define	check_region			lis_check_region
#ifdef request_region
#undef request_region
#endif
#define	request_region			lis_request_region
#ifdef release_region
#undef release_region
#endif
#define	release_region			lis_release_region

#ifdef add_timer
#undef add_timer
#endif
#define add_timer			lis_add_timer
#ifdef del_timer
#undef del_timer
#endif
#define del_timer			lis_del_timer

#ifdef do_gettimeofday
#undef do_gettimeofday
#endif
#define do_gettimeofday			lis_osif_do_gettimeofday
#ifdef do_settimeofday
#undef do_settimeofday
#endif
#define do_settimeofday			lis_osif_do_settimeofday

#ifdef kmalloc
#undef kmalloc
#endif
#define	kmalloc				lis_kmalloc
#ifdef kfree
#undef kfree
#endif
#define	kfree				lis_kfree
#ifdef vmalloc
#undef vmalloc
#endif
#define	vmalloc				lis_vmalloc
#ifdef vfree
#undef vfree
#endif
#define	vfree				lis_vfree

#ifdef request_dma
#undef request_dma
#endif
#define	request_dma			lis_request_dma
#ifdef free_dma
#undef free_dma
#endif
#define	free_dma			lis_free_dma

#ifdef udelay
#undef udelay
#endif
#define	udelay				lis_udelay
#ifdef jiffies
#undef jiffies
#endif
#define	jiffies				lis_jiffies()

#ifdef printk
#undef printk
#endif
#define	printk				lis_printk
#ifdef sprintf
#undef sprintf
#endif
#define sprintf				lis_sprintf
#ifdef vsprintf
#undef vsprintf
#endif
#define	vsprintf			lis_vsprintf

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#ifdef sleep_on
#undef sleep_on
#endif
#define	sleep_on			lis_sleep_on
#ifdef interruptible_sleep_on
#undef interruptible_sleep_on
#endif
#define	interruptible_sleep_on		lis_interruptible_sleep_on
#ifdef sleep_on_timeout
#undef sleep_on_timeout
#endif
#define	sleep_on_timeout		lis_sleep_on_timeout
#ifdef interruptible_sleep_on_timeout
#undef interruptible_sleep_on_timeout
#endif
#define	interruptible_sleep_on_timeout	lis_interruptible_sleep_on_timeout
#endif /* < 2.5 */

#ifdef wait_event
#undef wait_event
#endif
#define	wait_event			lis_wait_event
#ifdef wait_event_interruptible
#undef wait_event_interruptible
#endif
#define	wait_event_interruptible	lis_wait_event_interruptible
#ifdef wake_up
#undef wake_up
#endif
#define	wake_up				lis_wake_up
#ifdef wake_up_interruptible
#undef wake_up_interruptible
#endif
#define	wake_up_interruptible		lis_wake_up_interruptible


#endif				/* !defined(INCL_FROM_OSIF_DRIVER) */


/*
 * IRQ routines
 */
#if defined(_LINUX_PTRACE_H)
#define	OSIF_REGS_T	struct pt_regs
#else
#define	OSIF_REGS_T	void
#endif

/*
 * Pre 2.6 kernels use a void routine for interrupt handling.  In 2.6
 * it is an int routine.  For portability in LiS we will always use
 * an int handler.  It will be called as a void in earlier kernels.
 * This should allow you do move your driver back and forth between
 * 2.4 and 2.6 without recompilation.
 *
 * If you need to, use one of the typedefs here to cast your routine
 * pointer to the proper type to keep the compiler from squawking.
 */
typedef int  _RP (*lis_int_handler) (int, void *, OSIF_REGS_T *) ;
typedef void _RP (*lis_void_handler)(int, void *, OSIF_REGS_T *) ;

/*
 * In 2.6 the kernel wants the irq handler to return 1 if it handled the
 * interrupt and 0 if not.  LiS will provide a pair of variables that
 * hold these values for portability.  The externs are here and the
 * variables are in osif.c.  The variables will exist even on a 2.4
 * kernel so your handler can be portable.
 */
extern int	lis_irqreturn_handled ;
extern int	lis_irqreturn_not_handled ;

int  lis_request_irq(unsigned int  irq,
		      lis_int_handler handler,
	              unsigned long flags,
		      const char   *device,
		      void         *dev_id) _RP;
void lis_free_irq(unsigned int irq, void *dev_id) _RP;
void lis_disable_irq(unsigned int irq) _RP;
void lis_enable_irq(unsigned int irq) _RP;
void lis_osif_cli( void )_RP;
void lis_osif_sti( void )_RP;


#if defined(_SPARC_LIS_) || defined(_SPARC64_LIS_)

/*
 * On the sparc we have the whole physical IO address space mapped at all
 * times, so ioremap() and ioremap_nocache() do not need to do anything.
 */
#undef ioremap
#undef ioremap_nocache
#undef iounmap

extern __inline__ void *sparc_ioremap(unsigned long offset, unsigned long size)
{
	return __va(offset);
}

#define	ioremap				sparc_ioremap
#define ioremap_nocache(offset, size)	sparc_ioremap((offset), (size))
#define iounmap(ptr)			/* nothing to do */

#endif

/*
 * Memory mapping routines
 */
void *lis_ioremap(unsigned long offset, unsigned long size) _RP;
void *lis_ioremap_nocache(unsigned long offset, unsigned long size) _RP;
void  lis_iounmap(void *addr) _RP;
void *lis_vremap(unsigned long offset, unsigned long size) _RP;
unsigned long lis_virt_to_phys(volatile void *addr) _RP;
void         *lis_phys_to_virt(unsigned long addr) _RP;

/*
 * I/O port routines <linux/ioport.h>
 */
int  lis_check_region(unsigned int from, unsigned int extent) _RP;
void lis_request_region(unsigned int from,
			 unsigned int extent,
			 const char  *name) _RP;
void lis_release_region(unsigned int from, unsigned int extent) _RP;


/*
 * Memory allocator <linux/malloc.h>
 */
void *lis_kmalloc(size_t nbytes, int type) _RP;
void  lis_kfree(const void *ptr) _RP;
void *lis_vmalloc(unsigned long size)_RP;
void  lis_vfree(void *ptr) _RP;


/*
 * DMA routines <asm/dma.h>
 */
int  lis_request_dma(unsigned int dma_nr, const char *device_id) _RP;
void lis_free_dma(unsigned int dma_nr) _RP;

/*
 * Delay routine in <linux/delay.h> and <asm/delay.h>
 */
void lis_udelay(long micro_secs) _RP;
unsigned long lis_jiffies(void) _RP;

/*
 * Printing routines.
 */
#define PRINTF_LIKE(a,b)	__attribute__ ((format (printf, a, b)))
int lis_printk(const char *fmt, ...) PRINTF_LIKE(1,2) _RP;
int lis_sprintf(char *bfr, const char *fmt, ...) PRINTF_LIKE(2,3) _RP;
int lis_vsprintf(char *bfr, const char *fmt, va_list args) PRINTF_LIKE(2,0) _RP;

/*
 * Timer routines.
 */
void lis_add_timer(struct timer_list * timer)_RP;
int  lis_del_timer(struct timer_list * timer)_RP;

/*
 * Time routines in <linux/time.h>
 */
void lis_osif_do_gettimeofday( struct timeval *tp ) _RP;
void lis_osif_do_settimeofday( struct timeval *tp ) _RP;

/*
 * Sleep/wakeup routines
 *
 * Note: It it only legitimate to use these in open and close
 * routines in STREAMS.  DO NOT sleep in put or service routines.
 */
#define	OSIF_WAIT_Q_ARG		wait_queue_head_t *wq
#define	OSIF_WAIT_E_ARG		wait_queue_head_t  wq
void lis_sleep_on(OSIF_WAIT_Q_ARG) _RP;
void lis_interruptible_sleep_on(OSIF_WAIT_Q_ARG) _RP;
void lis_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout) _RP;
void lis_interruptible_sleep_on_timeout(OSIF_WAIT_Q_ARG, long timeout) _RP;
void lis_wait_event(OSIF_WAIT_E_ARG, int condition) _RP;
void lis_wait_event_interruptible(OSIF_WAIT_E_ARG, int condition) _RP;
void lis_wake_up(OSIF_WAIT_Q_ARG) _RP;
void lis_wake_up_interruptible(OSIF_WAIT_Q_ARG) _RP;


/*
 * Wrapped functions.
 *
 * These are some common functions that drivers might use and want to wrap
 * so that LiS can change the parameter passing convention prior to calling
 * the kernel's version of the function.
 */
extern char * _RP __wrap_strcpy(char *,const char *);
extern char * _RP __wrap_strncpy(char *,const char *, __kernel_size_t);
extern char * _RP __wrap_strcat(char *, const char *);
extern char * _RP __wrap_strncat(char *, const char *, __kernel_size_t);
extern int _RP __wrap_strcmp(const char *,const char *);
extern int _RP __wrap_strncmp(const char *,const char *,__kernel_size_t);
extern int _RP __wrap_strnicmp(const char *, const char *, __kernel_size_t);
extern char * _RP __wrap_strchr(const char *,int);
extern char * _RP __wrap_strrchr(const char *,int);
extern char * _RP __wrap_strstr(const char *,const char *);
extern __kernel_size_t _RP __wrap_strlen(const char *);
extern void * _RP __wrap_memset(void *,int,__kernel_size_t);
extern void * _RP __wrap_memcpy(void *,const void *,__kernel_size_t);
extern int _RP __wrap_memcmp(const void *,const void *,__kernel_size_t);
extern int _RP __wrap_sprintf(char *, const char *, ...);
extern int _RP __wrap_snprintf(char *, size_t, const char *, ...);
extern int _RP __wrap_vsprintf(char *, const char *, va_list);
extern int _RP __wrap_vsnprintf(char *, size_t, const char *, va_list);

#endif			/* from top of file */
