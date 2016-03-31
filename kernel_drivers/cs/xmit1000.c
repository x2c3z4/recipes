//-------------------------------------------------------------------
//	xmit1000.c
//
//	Transmits a packet via the Intel 82573L Pro1000 controller.
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 18 NOV 2007
//	revised on: 24 NOV 2007 -- to include data-length in packet
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/pci.h>		// for pci_get_device()
#include <linux/delay.h>	// for udelay()
#include <asm/uaccess.h>	// for copy_from_user()
#include <asm/io.h>		// for ioremap()

#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x109A	// 82573L nic device
//#define DEVICE_ID	0x10B9	// 82572EI nic device
//#define DEVICE_ID	0x107C	// 82541PI nic device
#define KMEM_SIZE	(4<<10)	// size of kernel memory allocation

typedef struct	{
		unsigned long long	base_addr;
		unsigned short		pkt_length;
		unsigned char		cksum_off;		
		unsigned char		desc_cmd;
		unsigned char		desc_stat;
		unsigned char		cksum_org;
		unsigned short		special;
		} TX_DESCRIPTOR;

enum	{
	E1000_CTRL 	= 0x0000,	// Device Control register
	E1000_STATUS 	= 0x0008, 	// Device Status register
	E1000_CTRL_EXT 	= 0x0018, 	// Device Control Extension
	E1000_ICR	= 0x00C0,	// Interrupt Cause Read
	E1000_ICS	= 0x00C8,	// Interrupt Cause Set
	E1000_IMS	= 0x00D0,	// Interrupt Mask Set/Read
	E1000_IMC	= 0x00D8,	// Interrupt Mask Clear
	E1000_TCTL	= 0x0400,	// Transmit Control register
	E1000_TIPG	= 0x0410,	// Transmit Inter-Packet Gap
	E1000_TDBAL	= 0x3800,	// Transmit Descriptor Base (Low)
	E1000_TDBAH	= 0x3804,	// Transmit Descriptor Base (High)
	E1000_TDLEN	= 0x3808,	// Transmit Descriptor Length
	E1000_TDH	= 0x3810,	// Transmit Descriptor Head
	E1000_TDT	= 0x3818,	// Transmit Descriptor Tail
	E1000_TIDV	= 0x3820,	// Transmit Interrupt Delay Value
	E1000_TXDCTL	= 0x3828,	// Transmit Descriptor Control
	E1000_TADV	= 0x382C,	// Tx-interrupt Absolute Delay Value
	E1000_TPT	= 0x40D4,	// Total Packets Transmitted
	E1000_RA	= 0x5400,	// Receive-filter Array
	};


char modname[] = "xmit1000";
char devname[] = "nic";
int my_major = 97;
struct pci_dev	*devp;
unsigned char	mac[6];
unsigned long	iomem_base;
unsigned long	iomem_size;
unsigned long	physaddr;
unsigned long	descaddr;
void		*kmem;
void		*io;
TX_DESCRIPTOR	*txdesc;


int my_get_info( char *buf, char **start, off_t off, int count )
{
	int	i, head, tail, n_xmit_packets, len = 0;

	// read the Transmit-Descriptor Queue's head and tail indices
	head = ioread32( io + E1000_TDH );
	tail = ioread32( io + E1000_TDT );

	// read some TX-related statistical counters
	n_xmit_packets = ioread32( io + E1000_TPT );

	// show the TX-Descriptor Queue
	len += sprintf( buf+len, "\n  Transmit-Descriptor Buffer-Area " );
	len += sprintf( buf+len, "(head=%d, tail=%d) \n\n", head, tail );
	for (i = 0; i < 8; i++)
		{
		int	status = txdesc[i].desc_stat;
		len += sprintf( buf+len, "  #%-2d ", i );
		len += sprintf( buf+len, "%08lX: ", (long)(txdesc + i) );
		len += sprintf( buf+len, "%016llX ", txdesc[i].base_addr );
		len += sprintf( buf+len, "%04X ", txdesc[i].pkt_length );
		len += sprintf( buf+len, "%02X ", txdesc[i].cksum_off ); 	
		len += sprintf( buf+len, "%02X ", txdesc[i].desc_cmd ); 	
		len += sprintf( buf+len, "%02X ", txdesc[i].desc_stat ); 	
		len += sprintf( buf+len, "%02X ", txdesc[i].cksum_org ); 	
		len += sprintf( buf+len, "%04X ", txdesc[i].special );
		if ( status & (1<<0) ) len += sprintf( buf+len, "DD " );	
		if ( status & (1<<1) ) len += sprintf( buf+len, "EC " );	
		if ( status & (1<<2) ) len += sprintf( buf+len, "LC " );	
		len += sprintf( buf+len, "\n" );
		}
	len += sprintf( buf+len, "\n" );

	// show the statistical counters
	len += sprintf( buf+len, "  packets_sent = %d ", n_xmit_packets );
	len += sprintf( buf+len, "\n\n" );

	return	len;
}


ssize_t my_write( struct file *file, const char *buf, size_t len, loff_t *pos )
{
	unsigned char	*packet = (unsigned char*)kmem;
	int		txtail = ioread32( io + E1000_TDT );

	// we cannot transmit more than 1500 bytes
	if ( len > 1500 ) len = 1500;

	// copy user's data into our packet-buffer
	if ( copy_from_user( packet + 16, buf, len ) ) return -EFAULT;
	
	// setup the packet-header
	memset( packet+0, 0xFF, 6 );	// broadcast address
	memcpy( packet+6, mac, 6 );	// source address
	packet[12] = 0x08;		// Type/Length LSB
	packet[13] = 0x00;		// Type/Length MSB
	memcpy( packet+14, &len, 2 );	// length of actual data
	
	// setup the next descriptor's length and status fields
	txdesc[ txtail ].pkt_length = len + 16;	
	txdesc[ txtail ].desc_stat = 0;
	
	// give ownership of the next descriptor to the controller
	txtail = (1 + txtail) % 8;
	iowrite32( txtail, io + E1000_TDT );

	// tell the kernel how many user-bytes were transferred
	return	len;
}


struct file_operations my_fops	= {
				  owner:	THIS_MODULE,
				  write:	my_write,
				  };


static void __exit my_exit(void )
{
	int	tx_control; 

	// disable the transmit engine
	tx_control = ioread32( io + E1000_TCTL );
	tx_control &= ~(1<<1);
	iowrite32( tx_control, io + E1000_TCTL );

	// release this driver's kernel resources
	unregister_chrdev( my_major, devname );
	remove_proc_entry( modname, NULL );
	kfree( kmem );
	iounmap( io );

	// issue confirmation message to the log-file
	printk( "<1>Removing \'%s\' module\n", modname );
}

static int __init my_init( void )
{
	int	tx_control, i;

	// issue confirmation message to the log-file
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d) \n", my_major );

	// detect the ethernet controller
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, NULL );
	if ( !devp ) return -ENODEV;
	printk( " Intel Pro/1000 Gigabit Ethernet Controller \n" );

	// remap the controller's i/o-memory to kernel-space
	iomem_base = pci_resource_start( devp, 0 );
	iomem_size = pci_resource_len( devp, 0 );
	io = ioremap_nocache( iomem_base, iomem_size );
	if ( !io ) return -ENOSPC;
	printk( " iomem_base=%08lX ", iomem_base );
	printk( " iomem_size=%08lX ", iomem_size );
	printk( "\n" );
	
	// report the controller's hardware MAC-address
	memcpy( mac, io + E1000_RA, 6 );
	printk( " Ethernet MAC-address: " );
	for (i = 0; i < 6; i++)
		{
		printk( "%02X", mac[i] );
		if ( i < 5 ) printk( ":" );
		}
	printk( "\n" );

	// allocate storage for transmit-buffer and descriptor-array
	kmem = kzalloc( KMEM_SIZE, GFP_KERNEL );
	if ( !kmem ) return -ENOMEM;
	physaddr = virt_to_phys( kmem );
	printk( " kernel memory at physical address 0x%08lX \n", physaddr );

	// setup transmit descriptors
	descaddr = physaddr + 0x600;
	txdesc = (TX_DESCRIPTOR*)phys_to_virt( descaddr ); 
	for (i = 0; i < 8; i++)
		{
		txdesc[i].base_addr = physaddr;
		txdesc[i].pkt_length = 0;
		txdesc[i].desc_cmd = (1<<0)|(1<<1)|(1<<3); // EOP, IFCS, RS 
		txdesc[i].desc_stat = 0;
		txdesc[i].special = 0;
		}	

	// reset the network controller
	iowrite32( 0xFFFFFFFF, io + E1000_IMC );
	iowrite32( 0x040C0241, io + E1000_CTRL );
	iowrite32( 0x000C0241, io + E1000_CTRL );
	udelay( 10000 );

	// configure the transmit engine
	tx_control = 0;
	tx_control |= (0<<1);	// EN-bit
	tx_control |= (1<<3);	// PSP-bit (Pad Short Packets) 
	tx_control |= (15<<4);	// CT=15 (Collision Threshold)
	tx_control |= (63<<12);	// COLD=63 (Collision Distance) 
	tx_control |= (0<<22);	// SWXOFF-bit (Software XOFF Transmit)
	tx_control |= (1<<24);	// RTLC-bit (Re-Transmit on Late Collision)
	tx_control |= (0<<25);	// UNORTX=0 (Underrun No Re-Transmit)
	tx_control |= (0<<26);	// TXCSCMT=0 (TxDesc Minimum Threshold)
	tx_control |= (0<<28);	// MULR-bit (Multiple Request Support) 
	iowrite32( tx_control, io + E1000_TCTL );  // Transmit Control

	iowrite32( 0x000C0241, io + E1000_CTRL );
	iowrite32( 0x1C1401C0, io + E1000_CTRL_EXT );
	iowrite32( 0x00602008, io + E1000_TIPG );
	iowrite32(   descaddr, io + E1000_TDBAL );
	iowrite32( 0x00000000, io + E1000_TDBAH );
	iowrite32( 0x00000080, io + E1000_TDLEN );
	iowrite32( 0x00000000, io + E1000_TDH );
	iowrite32( 0x00000000, io + E1000_TDT );
	iowrite32( 0x01010000, io + E1000_TXDCTL );

	// now turn on the 'transmit' engine
	iowrite32( tx_control | (1<<1), io + E1000_TCTL );

	// create pseudo-file and register this device-driver
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	register_chrdev( my_major, devname, &my_fops );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

