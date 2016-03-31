//-------------------------------------------------------------------
//	igbe.c
//
//	This module creates a pseudo-file named '/proc/igbe' showing
//	the values in some Intel GigaBit Ethernet Pro1000 controller 
//	registers of known relevance for reception and transmission,
//	helpful as a diagnostic aid in debugging device-driver code.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	date begun: 01 JUL 2007
//	completion: 11 JUL 2007
//	revised on: 15 JUL 2007 -- to include some added Rx-controls
//	revised on: 06 OCT 2007 -- to rearrange display of registers
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/utsname.h>	// for utsname()
#include <linux/pci.h>		// for pci_get_device()

#define VENDOR_ID	0x8086	// Intel Corporation
//#define DEVICE_ID	0x107C	// 82541PI controller
//#define DEVICE_ID	0x10B9	// 82572EI controller
#define DEVICE_ID	0x109A	// 82573L controller

enum {	e_CTRL   = 0x0000,  	e_STATUS = 0x0008,	
	e_CTRL_EXT = 0x0018,	e_RA = 0x5400, 
	e_ICR    = 0x00C0,	e_ICS    = 0x00C8,
	e_IMS    = 0x00D0,	e_IMC    = 0x00D8,
	e_RCTL   = 0x0100,  	e_TCTL   = 0x0400,
	e_RDBAL  = 0x2800,  	e_TDBAL  = 0x3800,
	e_RDBAH  = 0x2804,  	e_TDBAH  = 0x3804,
	e_RDLEN  = 0x2808,  	e_TDLEN  = 0x3808,
	e_RDH    = 0x2810,  	e_TDH    = 0x3810,
	e_RDT    = 0x2818,  	e_TDT    = 0x3818,
	e_RDTR   = 0x2820,  	e_TIDV   = 0x3820,
	e_RXDCTL = 0x2828,  	e_TXDCTL = 0x3828,
	e_RADV   = 0x282C,  	e_TADV   = 0x382C,
	e_RDBAL1  = 0x2900,  	e_TDBAL1  = 0x3900,
	e_RDBAH1  = 0x2904,  	e_TDBAH1  = 0x3904,
	e_RDLEN1  = 0x2908,  	e_TDLEN1  = 0x3908,
	e_RDH1    = 0x2910,  	e_TDH1    = 0x3910,
	e_RDT1    = 0x2918,  	e_TDT1    = 0x3918,
	e_RDTR1   = 0x2920,  	e_TIDV1   = 0x3920,
	e_RXDCTL1 = 0x2928,  	e_TXDCTL1 = 0x3928,
	e_RADV1   = 0x292C,  	e_TADV1   = 0x392C,
	e_RXCSUM = 0x5000,	e_RFCTL	  = 0x5008,
	e_PSRCTL = 0x2170,	e_TSPMT  = 0x3830,
	e_FCRTL  = 0x2160, 	e_FCRTH  = 0x2168,
	e_TIPG   = 0x0410,	e_FCT    = 0x0030,
	e_FCAL   = 0x0028,	e_FCAH   = 0x002C,
	e_FCTTV  = 0x0170,	e_TXCW	 = 0x0178, 	
	};

char modname[] = "igbe";
struct pci_dev *devp = NULL;
unsigned long	mmbase;
unsigned long	mmsize;
unsigned char	mac[6];
void		*igbe;

int my_get_info( char *buf, char **start, off_t off, int count )
{
	struct new_utsname 	*uts = utsname();
	int			i, len = 0;

	len += sprintf( buf+len, "\n Intel PRO1000 registers on " );
	len += sprintf( buf+len, "\'%s\' (MAC=", uts->nodename );
	for (i = 0; i < 6; i++)
		{
		len += sprintf( buf+len, "%02X", mac[i] );
		if ( i < 5 ) len += sprintf( buf+len, ":" );
		}
	len += sprintf( buf+len, ") \n" );

	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "   CTRL=%08X ", *(int*)(igbe+e_CTRL) );
	len += sprintf( buf+len, "  STATUS=%08X ", *(int*)(igbe+e_STATUS) );
	len += sprintf( buf+len, "  CTRL_EXT=%08X ", *(int*)(igbe+e_CTRL_EXT) );

	len += sprintf( buf+len, "\n\n" );
	len += sprintf( buf+len, "   RCTL=%08X ", *(int*)(igbe+e_RCTL) );
	len += sprintf( buf+len, "                     " );
	len += sprintf( buf+len, "   TCTL=%08X ", *(int*)(igbe+e_TCTL) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "  RDBAL=%08X ", *(int*)(igbe+e_RDBAL) );
	len += sprintf( buf+len, "  RDBAL1=%08X ", *(int*)(igbe+e_RDBAL1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "  TDBAL=%08X ", *(int*)(igbe+e_TDBAL) );
	len += sprintf( buf+len, "  TDBAL1=%08X ", *(int*)(igbe+e_TDBAL1) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "  RDBAH=%08X ", *(int*)(igbe+e_RDBAH) );
	len += sprintf( buf+len, "  RDBAH1=%08X ", *(int*)(igbe+e_RDBAH1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "  TDBAH=%08X ", *(int*)(igbe+e_TDBAH) );
	len += sprintf( buf+len, "  TDBAH1=%08X ", *(int*)(igbe+e_TDBAH1) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "  RDLEN=%08X ", *(int*)(igbe+e_RDLEN) );
	len += sprintf( buf+len, "  RDLEN1=%08X ", *(int*)(igbe+e_RDLEN1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "  TDLEN=%08X ", *(int*)(igbe+e_TDLEN) );
	len += sprintf( buf+len, "  TDLEN1=%08X ", *(int*)(igbe+e_TDLEN1) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "    RDH=%08X ", *(int*)(igbe+e_RDH) );
	len += sprintf( buf+len, "    RDH1=%08X ", *(int*)(igbe+e_RDH1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "    TDH=%08X ", *(int*)(igbe+e_TDH) );
	len += sprintf( buf+len, "    TDH1=%08X ", *(int*)(igbe+e_TDH1) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "    RDT=%08X ", *(int*)(igbe+e_RDT) );
	len += sprintf( buf+len, "    RDT1=%08X ", *(int*)(igbe+e_RDT1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "    TDT=%08X ", *(int*)(igbe+e_TDT) );
	len += sprintf( buf+len, "    TDT1=%08X ", *(int*)(igbe+e_TDT1) );

	len += sprintf( buf+len, "\n\n" );
	len += sprintf( buf+len, "   RDTR=%08X ", *(int*)(igbe+e_RDTR) );
	len += sprintf( buf+len, "   RDTR1=%08X ", *(int*)(igbe+e_RDTR1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "   TIDV=%08X ", *(int*)(igbe+e_TIDV) );
	len += sprintf( buf+len, "   TIDV1=%08X ", *(int*)(igbe+e_TIDV1) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, " RXDCTL=%08X ", *(int*)(igbe+e_RXDCTL) );
	len += sprintf( buf+len, " RXDCTL1=%08X ", *(int*)(igbe+e_RXDCTL1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, " TXDCTL=%08X ", *(int*)(igbe+e_TXDCTL) );
	len += sprintf( buf+len, " TXDCTL1=%08X ", *(int*)(igbe+e_TXDCTL1) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "   RDAV=%08X ", *(int*)(igbe+e_RADV) );
	len += sprintf( buf+len, "   RDAV1=%08X ", *(int*)(igbe+e_RADV1) );
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "   TADV=%08X ", *(int*)(igbe+e_TADV) );
	len += sprintf( buf+len, "   TADV1=%08X ", *(int*)(igbe+e_TADV1) );
	len += sprintf( buf+len, "\n\n" );
	len += sprintf( buf+len, " RXCSUM=%08X  ", *(int*)(igbe+e_RXCSUM) );
	len += sprintf( buf+len, " PSRCTL=%08X ", *(int*)(igbe+e_PSRCTL) );    	
	len += sprintf( buf+len, "   " );
	len += sprintf( buf+len, "   TXCW=%08X  ", *(int*)(igbe+e_TXCW) );
	len += sprintf( buf+len, "  TSPMT=%08X ", *(int*)(igbe+e_TSPMT) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "  RFCTL=%08X ", *(int*)(igbe+e_RFCTL ) );    	
	len += sprintf( buf+len, "                     " );
	len += sprintf( buf+len, "   TIPG=%08X ", *(int*)(igbe+e_TIPG) );

	len += sprintf( buf+len, "\n\n" );
	len += sprintf( buf+len, "    ICR=%08X ", *(int*)(igbe+e_ICR   ) );
	len += sprintf( buf+len, "                     " );
	len += sprintf( buf+len, "   FCAL=%08X  ", *(int*)(igbe+e_FCAL) );
	len += sprintf( buf+len, "  FCRTL=%08X  ", *(int*)(igbe+e_FCRTL ) );  	
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "    ICS=%08X ", *(int*)(igbe+e_ICS   ) );
	len += sprintf( buf+len, "                     " );
	len += sprintf( buf+len, "   FCAH=%08X  ", *(int*)(igbe+e_FCAH) );
	len += sprintf( buf+len, "  FCRTH=%08X  ", *(int*)(igbe+e_FCRTH ) ); 	
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "    IMS=%08X ", *(int*)(igbe+e_IMS   ) );
	len += sprintf( buf+len, "                     " );
	len += sprintf( buf+len, "    FCT=%08X  ", *(int*)(igbe+e_FCT) );
	len += sprintf( buf+len, "  FCTTV=%08X  ", *(int*)(igbe+e_FCTTV) );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "    IMC=%08X ", *(int*)(igbe+e_IMC   ) );
	len += sprintf( buf+len, "\n\n" );
	return	len;
}


static int __init igbe_init( void )
{
	int	i;

	// show confirmation message
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// map the device's i/o memory
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, devp );
	if ( !devp ) return -ENODEV;
	mmbase = pci_resource_start( devp, 0 );
	mmsize = pci_resource_len( devp, 0 );
	igbe = ioremap_nocache( mmbase, mmsize );
	if ( !igbe ) return -EBUSY;

	// copy the device's hardware-address
	memcpy( mac, igbe+e_RA, 6 );

	// confirm device-presence and show its MAC-address
	printk( "Intel PRO1000 Gigibit Ethernet Controller: " );
	printk( "%08lX-%08lX \n", mmbase, mmbase+mmsize );	
	printk( "  MAC-ADDRESS: " );
	for (i = 0; i < 6; i++) 
		{ 
		printk( "%02X", mac[i] );
		if ( i < 5 ) printk( ":" );
		}
	printk( "\n" );

	// install our pseudo-file
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}


static void __exit igbe_exit(void )
{
	// delete our pseudo-file
	remove_proc_entry( modname, NULL );
	
	// unmap the device's i/o memory
	iounmap( igbe );

	// show confirmation of module's removal
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( igbe_init );
module_exit( igbe_exit );
MODULE_LICENSE("GPL"); 

