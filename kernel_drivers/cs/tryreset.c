//-------------------------------------------------------------------
//	tryreset.c
//
//	This module experiments with the 'Reset' capabilities of the
//	Intel 82573L Gigabit Ethernet Controller, writing values for
//	the device's Control Status registers to the kernel log-file
//	while waiting for the device to set certain status-bits.  We
//	recommend installing this module from the text-mode console,
//	in order to watch a dynamic display of status-bits changing.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 08 NOV 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/pci.h>		// for pci_get_device()
#include <linux/sched.h>	// for schedule()

#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x109A	// 82573L controller
//#define DEVICE_ID	0x10B9	// 82572EI controller

#define E1000_CTRL	0x0000	// Device Control register
#define E1000_STATUS	0x0008	// Device Status register

char modname[] = "tryreset";
struct pci_dev	*devp;
unsigned int	iomem_base;
unsigned int	iomem_size;
void		*io;


static int __init my_init( void )
{
	int	stat, ctrl, count = 0;

	// write log-message announcing module's installation
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// detect presence of the Intel Pr01000 controller
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, NULL );
	if ( !devp ) return -ENODEV;
	
	// get location and size of the controller's i/o memory
	iomem_base = pci_resource_start( devp, 0 );
	iomem_size = pci_resource_len( devp, 0 );

	// map the controller's i/o memory into kernel-space
	io = ioremap_nocache( iomem_base, iomem_size );
	if ( !io ) return -ENOSPC;



	//-----------------------------------------------
	// try resetting the gigabit ethernet controller
	//----------------------------------------------- 

	// write zeros to the Device Status register
	iowrite32( 0x00000000, io + E1000_STATUS );

	// initiate Device Reset and Phy Reset and Link Reset
	iowrite32( 0x84000008, io + E1000_CTRL );

	// wait for bit #31 to be set in Device Status
	do	{
		stat = ioread32( io + E1000_STATUS );
		ctrl = ioread32( io + E1000_CTRL );
		
		printk( " %4d: ", ++count );
		printk( " CTRL=%08X  STAT=%08X ", ctrl, stat );
		printk( "\n" );
		schedule();
		if ( signal_pending( current ) ) break;
		}
	while ( ( stat & 0x80000000 ) == 0 ); 
	printk( "\n" );

	// set the Link-Up bit
	iowrite32( 0x00040241, io + E1000_CTRL );

	// wait for bit #1 to be set in Device Status
	do	{
		stat = ioread32( io + E1000_STATUS );
		ctrl = ioread32( io + E1000_CTRL );
		
		printk( " %4d: ", ++count );
		printk( " CTRL=%08X  STAT=%08X ", ctrl, stat );
		printk( "\n" );
		schedule();
		if ( signal_pending( current ) ) break;
		}
	while ( ( stat & 0x00000002 ) == 0 ); 
	printk( "\n" );
	return	0;  //SUCCESS
}


static void __exit my_exit(void )
{
	// unmap the controller's i/o memory
	iounmap( io );

	// write log-message confirming module's removal
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

