//-------------------------------------------------------------------
//	anchor.c
//
//	To get the MAC address of this station's network interface.
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 28 OCT 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/utsname.h>	// for utsname()
#include <linux/pci.h>		// for pci_get_device()
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/io.h>		// for inl(), outl()

#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x109A	// 82573L ANCHOR
#define HW_ADDRESS	0x5400	// offset to MAC-ADDRESS

char legend[] = "INTEL Pro1000 Gigabit Ethernet Controller";
char modname[] = "anchor";
struct pci_dev	*devp;
unsigned char 	mac[8];
unsigned long 	membase, memsize;
void	*io;

int my_get_info( char *buf, char **start, off_t off, int count )
{
	struct new_utsname	*uts = utsname();
	u32	datum;
	int	i, len = 0;

	len += sprintf( buf+len, "\n\n %58s \n\n", legend );
	len += sprintf( buf+len, "\n %48s \n", "PCI CONFIGURATION SPACE" );
	for (i = 0; i < 256; i+=4)
		{
		if ( ( i % 32 ) == 0 ) 
			len += sprintf( buf+len, "\n %04X: ", i );
		pci_read_config_dword( devp, i, &datum );
		len += sprintf( buf+len, "%08X ", datum );
		}
	len += sprintf( buf+len, "\n\n" );

	len += sprintf( buf+len, " MAC-ADDRESS: " );
	for (i = 0; i < 6; i++)
		{
		len += sprintf( buf+len, "%02X", mac[i] );
		if ( i < 5 ) len += sprintf( buf+len, ":" );
		}
	len += sprintf( buf+len, "   WORKSTATION: %s ", uts->nodename );
	len += sprintf( buf+len, "\n\n" );

	return	len;
}


static int __init my_init( void )
{
	// display confirmation message
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// detect the tigon3 device 
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, NULL );
	if ( !devp ) return -ENODEV;

	// remap the device's I/O memory
	membase = pci_resource_start( devp, 0 );
	memsize = pci_resource_len( devp, 0 );
	io = ioremap_nocache( membase, memsize );
	if ( !io ) return -EBUSY;

	// copy device's hardware address to 'mac[]' array
	mac[0] = *(unsigned char*)(io + HW_ADDRESS + 0);
	mac[1] = *(unsigned char*)(io + HW_ADDRESS + 1);
	mac[2] = *(unsigned char*)(io + HW_ADDRESS + 2);
	mac[3] = *(unsigned char*)(io + HW_ADDRESS + 3);
	mac[4] = *(unsigned char*)(io + HW_ADDRESS + 4);
	mac[5] = *(unsigned char*)(io + HW_ADDRESS + 5);

	// create pseudo-file in the '/proc' directoey
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}


static void __exit my_exit(void )
{
	// delete the pseudo-file
	remove_proc_entry( modname, NULL );

	// unmap the device's I/O memory
	iounmap( io );

	// display confirmation message
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

