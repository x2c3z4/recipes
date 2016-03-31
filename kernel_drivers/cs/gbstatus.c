//-------------------------------------------------------------------
//	gbstatus.c
//
//	This module will create a pseudo-file named '/proc/gbstatus' 
//	that lets users see the current value in the STATUS register
//	of the Intel Pro1000 Gigabit Ethernet Controller.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 06 NOV 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/pci.h>		// for pgi_get_device()

#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x109A	// 82573L controller
//#define DEVICE_ID	0x10B9	// 82572EI controller

#define E1000_STATUS	0x0008	// Device Status register

char modname[] = "gbstatus";
struct pci_dev	*devp;
unsigned int	iomem_base;
unsigned int	iomem_size;
void		*io;

char	*link[] = { "DOWN", "UP" };
char	*speed[] = { "10-Mbps", "100-Mbps", "1000-Mbps", "1000-Mbps" };
char	*duplex[] = { "HALF-DUPLEX", "FULL-DUPLEX" };

int my_get_info( char *buf, char **start, off_t off, int count )
{
	unsigned int	status;
	int		len = 0;

	// read the ethernet controller's Device Status Register
	status = ioread32( io + E1000_STATUS );

	// display the 32-bit register-value in hexadecimal format
	len += sprintf( buf+len, "\n Intel Pro1000 GbE Controller Status: " );
	len += sprintf( buf+len, "0x%08X \n", status );
	len += sprintf( buf+len, "\n" );

	// Show the interpretations of the FD, LU, and SPEED fields
	len += sprintf( buf+len, "   Link is %s: ", link[ (status >> 1)&1 ] );
	len += sprintf( buf+len, " speed %s ", speed[ (status >> 6)&3 ] );
	len += sprintf( buf+len, "(%s) \n", duplex[ (status >> 0)&1 ] );
	len += sprintf( buf+len, "\n" );

	return	len;
}


static int __init my_init( void )
{
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

	// install our pseudo-file in the '/proc' directory
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}


static void __exit my_exit(void )
{
	// delete our pseudo-file
	remove_proc_entry( modname, NULL );

	// unmap the controller's i/o memory
	iounmap( io );

	// write log-message confirming module's removal
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

