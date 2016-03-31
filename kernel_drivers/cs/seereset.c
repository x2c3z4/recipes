//-------------------------------------------------------------------
//	seereset.c
//
//	This module installs a pseudo-file (named '/proc/seereset')
//	which resets the Intel 82573L Pro/1000 ethernet controller,
//	then shows its record of state-changes for three registers. 
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	date begun: 11 NOV 2007
//	completion: 12 NOV 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/pci.h>		// for pci_get_device()
#include <linux/utsname.h>	// for utsname()

#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x109A	// 82573L controller
//#define DEVICE_ID	0x10B9	// 82572EI controller

#define KMEM_SIZE     	(4<<12)
#define E1000_CTRL	0x0000
#define E1000_STATUS	0x0008
#define E1000_ICR	0x00C0
#define RESET_CMD   0x84140248
#define LOOP_BOUND     1000000

char modname[] = "seereset";
struct new_utsname     *uts;
struct pci_dev	*devp;
unsigned int	iomem_base;
unsigned int	iomem_size;
void		*io, *kmem;
char title[] = "Responses of Pro/1000 ethernet controller on node";

int my_get_info( char *buf, char **start, off_t off, int count )
{
	unsigned int	*ctrl = (unsigned int*)(kmem + 0x0000);
	unsigned int	*stat = (unsigned int*)(kmem + 0x1000);
	unsigned int	*icrr = (unsigned int*)(kmem + 0x2000);
	unsigned int	*indx = (unsigned int*)(kmem + 0x3000);
	int		i = 0, j, k, len = 0;

	// read the Interrupt Cause Read register (to clear it)
	ioread32( io + E1000_ICR );

	// record the initial contents of our three registers
	ctrl[i] = ioread32( io + E1000_CTRL );	
	stat[i] = ioread32( io + E1000_STATUS );	
	icrr[i] = ioread32( io + E1000_ICR );	
	indx[i] = i;
	++i;

	// clear 'writable' bits in controller's STATUS register
	iowrite32( 0x00000000, io + E1000_STATUS );

	// record the modified contents of our three registers
	ctrl[i] = ioread32( io + E1000_CTRL );	
	stat[i] = ioread32( io + E1000_STATUS );	
	icrr[i] = ioread32( io + E1000_ICR );	
	indx[i] = i;
	++i;
	
	// output specified reset-command to the CTRL register
	iowrite32( RESET_CMD, io + E1000_CTRL );

	// record the three registers' sequence of state-changes 
	for (j = i; j < LOOP_BOUND; j++)   // don't loop forever
		{	
		ctrl[i] = ioread32( io + E1000_CTRL );	
		stat[i] = ioread32( io + E1000_STATUS );	
		icrr[i] = ioread32( io + E1000_ICR );	
		indx[i] = j;
		if (( ctrl[i] != ctrl[i-1] )||( stat[i] != stat[i-1] )
			||( icrr[i] != icrr[i-1] )) ++i;
		if ( i == 1024 ) break;	// don't overflow storage
		}

	// display an explanatory title for our pseudo-file's output 
	len += sprintf( buf+len, "\n %s \'%s\' ", title, uts->nodename );
	len += sprintf( buf+len, "\n when 0x%08X was written ", RESET_CMD );
	len += sprintf( buf+len, "to its Device Control register (CTRL) \n" );
	len += sprintf( buf+len, "\n " );

	// exhibit our record of the registers' state-changes
	for (k = 0; k < i; k++)
		{
		if ( len + 80 > count ) break;	// don't overflow 'buf'
		len += sprintf( buf+len, " CTRL=%08X ", ctrl[k] );
		len += sprintf( buf+len, " STATUS=%08X ", stat[k] );
		len += sprintf( buf+len, " ICR=%08X ", icrr[k] );
		len += sprintf( buf+len, " #%d ", indx[k] );
		len += sprintf( buf+len, "\n " );
		}	
	len += sprintf( buf+len, "\n" );
	return	len;
}


static void __exit my_exit(void )
{
	remove_proc_entry( modname, NULL );
	kfree( kmem );
	iounmap( io );

	printk( "<1>Removing \'%s\' module\n", modname );
}


static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// confirm the Pro/1000 controller's presence
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, NULL );
	if ( !devp ) return -ENODEV;

	// get the controller's i/o-memory location and size
	iomem_base = pci_resource_start( devp, 0 );
	iomem_size = pci_resource_len( devp, 0 );

	// map the controller's i/o-memory into kernel-space
	io = ioremap_nocache( iomem_base, iomem_size );
	if ( !io ) return -ENOSPC;

	// allocate storage for our registers' observed values
	kmem = kmalloc( KMEM_SIZE, GFP_KERNEL );
	if ( !kmem ) { iounmap( io ); return -ENOMEM; }

	// clear the allocated memory-region
	memset( kmem, 0, KMEM_SIZE );

	// obtain this workstation's node-name (for documentaion)
	uts = utsname();

	// install our pseudo-file in the usual directory
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

