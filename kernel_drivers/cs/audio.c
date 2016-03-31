//-------------------------------------------------------------------
//	audio.c		   (Modification of our 'tigon3.c' module)
//
//	This module creates a pseudo-file named '/proc/audio' that
//	lets users view the PCI Configuration Space for the sound- 
//	controller device in our HRN-235/HRN-535 workstations.  It
//	is based on an earlier module which performed that service
//	for the Broadcom 'tigon3' ethernet interface controller.
//
//	NOTE: Written and tested on Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 27 OCT 2007
//	revised on: 15 NOV 2007 -- for Intel ICH6 audio controller
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/utsname.h>	// for utsname()
#include <linux/pci.h>		// for pci_get_device()
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/io.h>		// for inl(), outl()


#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x266E	// ICH6 audio controller 


char legend[] = "Intel ICH6 Audio Controller";
char modname[] = "audio";
struct pci_dev	*devp;


int my_get_info( char *buf, char **start, off_t off, int count )
{
	struct new_utsname	*uts = utsname();
	u32	datum;
	int	i, len = 0;

	len += sprintf( buf+len, "\n\n %50s \n\n", legend );
	len += sprintf( buf+len, "\n %48s \n", "PCI CONFIGURATION SPACE" );
	for (i = 0; i < 256; i+=4)
		{
		if ( ( i % 32 ) == 0 ) 
			len += sprintf( buf+len, "\n %04X: ", i );
		pci_read_config_dword( devp, i, &datum );
		len += sprintf( buf+len, "%08X ", datum );
		}
	len += sprintf( buf+len, "\n\n" );

	len += sprintf( buf+len, " WORKSTATION: %s ", uts->nodename );
	len += sprintf( buf+len, "\n\n" );

	return	len;
}


static int __init my_init( void )
{
	// display confirmation message
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// detect the audio device 
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, NULL );
	if ( !devp ) return -ENODEV;

	// create pseudo-file in the '/proc' directoey
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}


static void __exit my_exit(void )
{
	// delete the pseudo-file
	remove_proc_entry( modname, NULL );

	// display confirmation message
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

