//-------------------------------------------------------------------
//	nic.c
//
//	This Linux device-driver module combines the functions from 
//	our previous 'xmit1000.c' and 'recv1000.c' demos, adding an
//	'ioctl()' function which allows an application to designate
//	the destination MAC-address for any transmitted packets; it
//	also adjusts bits in the Receive Control register (RCTL) to
//	disable the Unicast and Multicast Promiscuous Modes, so the
//	Pro1000 network interface only receives packets sent to it.
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 28 NOV 2007
//-------------------------------------------------------------------


#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/pci.h>		// for pci_get_device()
#include <linux/interrupt.h>	// for request_irq()
#include <asm/uaccess.h>	// for copy_to_user()
#include <asm/io.h>		// for ioremap()


#define VENDOR_ID	0x8086	// Intel Corporation
#define DEVICE_ID	0x109A	// 82573L nic device
//#define DEVICE_ID	0x10B9	// 82572EI nic device
//#define DEVICE_ID	0x107C	// 82541PI nic device
#define KMEM_SIZE       (4<<12)	// kernel memory allocation
#define INTR_MASK   0xFFFFFFFF	// interrupts to recognize


typedef struct	{
		unsigned long long	base_addr;
		unsigned short		pkt_length;
		unsigned char		cksum_off;		
		unsigned char		desc_cmd;
		unsigned char		desc_stat;
		unsigned char		cksum_org;
		unsigned short		special;
		} TX_DESCRIPTOR;


typedef struct	{
		unsigned long long	base_addr;
		unsigned short		pkt_length;
		unsigned short		check_sum;
		unsigned char		desc_stat;
		unsigned char		desc_errs;
		unsigned short		vlan_tag;
		} RX_DESCRIPTOR;



char modname[] = "nic";
char devname[] = "nic";
char pseudorx[] = "nicrx";
char pseudotx[] = "nictx";
int my_major = 97;
struct pci_dev 	*devp;
unsigned char	mac[6], dstn[6];
unsigned int	irq;
unsigned long	iomem_base;
unsigned long	iomem_size;
void		*io, *kmem;
unsigned long	physaddr;
unsigned long	descaddr;
TX_DESCRIPTOR	*txdesc;
RX_DESCRIPTOR	*rxdesc;
wait_queue_head_t wq_rx;


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
	E1000_RCTL 	= 0x0100,	// Receive Control register
	E1000_PSRCTL 	= 0x2170,	// Packet-Split Receive Control
	E1000_RDBAL 	= 0x2800,	// Rx-Descriptor Base-Address (low)	
	E1000_RDBAH 	= 0x2804,	// Rx-Descriptor Base-Address (High)
	E1000_RDLEN 	= 0x2808,	// Rx-Descriptor array Length
	E1000_RDH 	= 0x2810,	// Receive Descriptor Head
	E1000_RDT 	= 0x2818,	// Receive Descriptor Tail
	E1000_RDTR 	= 0x2820,	// Rx-interrupt Delay Timer Register
	E1000_RXDCTL 	= 0x2828,	// Receive Descriptor Control
	E1000_RADV 	= 0x282C,	// Rx-interrupt Absolute Delay Value
	E1000_RXERRC	= 0x400C,	// Receive Errors Count 
	E1000_TPR 	= 0x40D0,	// Total Packets Received
	E1000_RXCSUM 	= 0x5000,	// Receive Checksum Control
	E1000_RFCTL	= 0x5008,	// Receive Filter Control
	E1000_RA 	= 0x5400, 	// Receive-filter Array
	}; 



irqreturn_t my_isr( int irq, void *dev_id )
{
	static int	reps = 0;
	int	intr_cause = ioread32( io + E1000_ICR );

	if ( intr_cause == 0 ) return IRQ_NONE;

	printk( " RECV1000: #%d CAUSE=%08X ", ++reps, intr_cause );
	if ( intr_cause & (1<<0 ) ) printk( "TXDW " );
	if ( intr_cause & (1<<1 ) ) printk( "TXQE " );
	if ( intr_cause & (1<<2 ) ) printk( "LSC " );
	if ( intr_cause & (1<<4 ) ) printk( "RXDMT0 " );
	if ( intr_cause & (1<<6 ) ) printk( "RXO (Receiver Overrun) " );
	if ( intr_cause & (1<<7 ) ) printk( "RXT0 (Receiver Timeout) " );
	if ( intr_cause & (1<<9 ) ) printk( "MDAC " );
	if ( intr_cause & (1<<15 ) ) printk( "TXDLOW " );
	if ( intr_cause & (1<<16 ) ) printk( "SRPD " );
	if ( intr_cause & (1<<17 ) ) printk( "ACK " );
	printk( "\n" );

	wake_up_interruptible( &wq_rx );
	iowrite32( intr_cause, io + E1000_ICR );

	return	IRQ_HANDLED;
}


ssize_t my_write( struct file *file, const char *buf, size_t len, loff_t *pos )
{
	int	txtail = ioread32( io + E1000_TDT );
	char	*packet = (char*)phys_to_virt( txdesc[ txtail ].base_addr );

	// we cannot transmit more than 1500 bytes
	if ( len > 1500 ) len = 1500;

	// copy user's data into our packet-buffer
	if ( copy_from_user( packet + 16, buf, len ) ) return -EFAULT;
	
	// setup the packet-header
	memcpy( packet+0, dstn, 6 );	// destination address
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


ssize_t my_read( struct file *file, char *buf, size_t len, loff_t *pos )
{
	static int	rxhead = 0;
	unsigned char	*from = phys_to_virt( rxdesc[ rxhead ].base_addr );	
	unsigned int	count;

	// sleep if no new packets have been received
	if ( ioread32( io + E1000_RDH ) == rxhead )
		if ( wait_event_interruptible( wq_rx,
			ioread32( io + E1000_RDH ) != rxhead ) ) return -EINTR;

	// get the number of actual data-bytes in this packet	
	count = *(unsigned short*)(from + 14);

	// we cannot copy more bytes than the user's buffer can hold
	if ( count > len ) count = len;

	// copy packet's data to the user's buffer	
	if ( copy_to_user( buf, from+16, count ) ) return -EFAULT;

	// advance our array-index variable to the next descriptor
	rxhead = (1 + rxhead) % 8;

	// tell the kernel how many bytes were transferred
	return	count;
}


int my_ioctl( struct inode *inode, struct file *file, 
			unsigned int request, unsigned long address )
{
	unsigned char	*from = (unsigned char *)address;	

	switch ( request )
		{
		case 0:	// set packets' destination MAC-address
			if ( copy_from_user( dstn, from, 6 ) ) return -EFAULT;
			return 0;  // SUCCESS

		case 1:	// get packets' destination MAC-address
			if ( copy_to_user( from, dstn, 6 ) ) return -EFAULT;
			return 0;  // SUCCESS

		default:
			break;
		}

	return -EINVAL;  
}





struct file_operations my_fops = {
				owner:		THIS_MODULE,
				write:		my_write,
				read:		my_read,
				ioctl:		my_ioctl,
				};



int my_get_info_tx( char *buf, char **start, off_t off, int count )
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









int my_get_info_rx( char *buf, char **start, off_t off, int count )
{
	int	i, head, tail, n_pkts_recv, n_recv_errs, len = 0;

	// read the Receive-Descriptor Queue's head and tail indices
	head = ioread32( io + E1000_RDH );
	tail = ioread32( io + E1000_RDT );

	// read some RX-related statistical counters
	n_pkts_recv = ioread32( io + E1000_TPR );
	n_recv_errs = ioread32( io + E1000_RXERRC );

	// show the RX-Descriptor Queue
	len += sprintf( buf+len, "\n  Receive-Descriptor Buffer-Area " );
	len += sprintf( buf+len, "(head=%d, tail=%d) \n\n", head, tail );
	for (i = 0; i < 8; i++)
		{
		int	status = rxdesc[i].desc_stat;
		int	errors = rxdesc[i].desc_errs;
		len += sprintf( buf+len, "  #%-2d ", i );
		len += sprintf( buf+len, "%08lX: ", (long)(rxdesc + i) );
		len += sprintf( buf+len, "%016llX ", rxdesc[i].base_addr );
		len += sprintf( buf+len, "%04X ", rxdesc[i].pkt_length );
		len += sprintf( buf+len, "%04X ", rxdesc[i].check_sum );
		len += sprintf( buf+len, "%02X ", rxdesc[i].desc_stat );
		len += sprintf( buf+len, "%02X ", rxdesc[i].desc_errs );
		len += sprintf( buf+len, "%04X ", rxdesc[i].vlan_tag );
		if ( status & (1<<0) ) len += sprintf( buf+len, " DD" );
		if ( status & (1<<1) ) len += sprintf( buf+len, " EOP" );
		if ( status & (1<<2) ) len += sprintf( buf+len, " IXSM" );
		if ( status & (1<<3) ) len += sprintf( buf+len, " VP" );
		if ( status & (1<<5) ) len += sprintf( buf+len, " TCPCS" );
		if ( status & (1<<6) ) len += sprintf( buf+len, " IPCS" );
		if ( status & (1<<7) ) len += sprintf( buf+len, " PIF" );
		len += sprintf( buf+len, " " );
		if ( errors & (1<<0) ) len += sprintf( buf+len, " CE" );
		if ( errors & (1<<2) ) len += sprintf( buf+len, " FE" );
		if ( errors & (1<<5) ) len += sprintf( buf+len, " TCPE" );
		if ( errors & (1<<6) ) len += sprintf( buf+len, " IPE" );
		if ( errors & (1<<7) ) len += sprintf( buf+len, " RXE" );
		len += sprintf( buf+len, "\n" );
		}
	len += sprintf( buf+len, "\n" );

	// show the statistical counters
	len += sprintf( buf+len, "  packets received = %d ", n_pkts_recv );
	len += sprintf( buf+len, "  recv_error_count = %d ", n_recv_errs );
	len += sprintf( buf+len, "\n\n" );

	return	len;
}




static int __init my_init( void )
{
	int	rx_control, tx_control, i;
	u16	pci_cmd;

	// issue confirmation-message to the log-file
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d) \n", my_major );

	// detect the ethernet controller
	devp = pci_get_device( VENDOR_ID, DEVICE_ID, NULL );
	if ( !devp ) return -ENODEV;
	
	// remap the controller's i/o-memory to kernel-space
	iomem_base = pci_resource_start( devp, 0 );
	iomem_size = pci_resource_len( devp, 0 );
	io = ioremap_nocache( iomem_base, iomem_size );
	if ( !io ) return -EBUSY;
	printk( " iomem_base=%08lX ", iomem_base );
	printk( " iomem_size=%08lX ", iomem_size );
	printk( "\n" );

	// report the controller's hardware MAC-address
	memcpy( mac, io + E1000_RA, 6 );
	printk( " MAC-ADDRESS: " );
	for (i = 0; i < 6; i++) 
		{
		printk( "%02X", mac[i] );
		if ( i < 5 ) printk( ":" );
		}
	printk( "\n" );

	// allocate storage for receive-buffers and descriptor-array
	kmem = kzalloc( KMEM_SIZE, GFP_KERNEL );
	if ( !kmem ) return -ENOMEM;
	physaddr = virt_to_phys( kmem );
	printk( " kernel memory at physical address 0x%08lX \n", physaddr );

	// setup receive and transmit descriptors
	descaddr = physaddr + (9 * 0x600);
	rxdesc = (RX_DESCRIPTOR*)phys_to_virt( descaddr + 0x100 );
	txdesc = (TX_DESCRIPTOR*)phys_to_virt( descaddr + 0x200);
	for (i = 0; i < 8; i++)
		{
		rxdesc[i].base_addr = physaddr + (i * 0x600);
		rxdesc[i].pkt_length = 0;	
		rxdesc[i].check_sum = 0;	
		rxdesc[i].desc_stat = 0;	
		rxdesc[i].desc_errs = 0;	
		rxdesc[i].vlan_tag= 0;	
		}
	init_waitqueue_head( &wq_rx );
	for (i = 0; i < 8; i++)
		{
		txdesc[i].base_addr = physaddr + 0x3000;
		txdesc[i].pkt_length = 0;	
		txdesc[i].desc_cmd = (1<<0)|(1<<1)|(1<<3); // EOP, IFCS, RS
		txdesc[i].desc_stat = 0;	
		txdesc[i].special = 0;	
		}

	// insure Bus Master bit is set in PCI Command Register
	pci_read_config_word( devp, 4, &pci_cmd );
	pci_cmd |= (1 << 2);
	pci_write_config_word( devp, 4, pci_cmd );

	// install interrupt handler
	irq = devp->irq;
	if ( request_irq( irq, my_isr, IRQF_SHARED, modname, &modname ) < 0 )
		{ kfree( kmem ); iounmap( io ); return -EBUSY; }

	// reset the network controller
	iowrite32( 0xFFFFFFFF, io + E1000_IMC );
	iowrite32( 0x040C0241, io + E1000_CTRL );
	iowrite32( 0x000C0241, io + E1000_CTRL );
	udelay( 10000 );

	// TODO: clear the multicast table array
	// TODO: clear the statistical counters
	// TODO: reprogram the filter-address array

	// configure the receive engine 
	rx_control = 0;
	rx_control |= (0<<1);	// EN-bit (Enable Receive Engine)
	rx_control |= (1<<2);	// SPB-bit (Store Bad Packets)
//	rx_control |= (1<<3);	// UPE-bit (Unicast Promiscuous Enable) 
//	rx_control |= (1<<4);	// MPE-bit (Multicast Promiscuous Enable)
	rx_control |= (0<<5);	// LPE-bit (Long Packet Enable)
	rx_control |= (0<<6);	// LBM=0 (LoopBack Mode off) 
	rx_control |= (0<<8);	// RDMTS=0 (Rx-Descriptor Min Thresh Size)
	rx_control |= (0<<10);	// DTYPE=0 (Descriptor=Type)
	rx_control |= (0<<12);	// MO=0 (Multicast Offset)
	rx_control |= (1<<15);	// BAM-bit (Broadcast Address Enable) 
	rx_control |= (0<<16);	// BSIZE=0 (Receive Buffer Size = 2048)
	rx_control |= (0<<18);	// VLE=0 (VLAN Flter Eable)
	rx_control |= (0<<19);	// CFIEN=0 (Cannonical Form Indicator Enable)
	rx_control |= (0<<20);	// CFI=0 (Canonical Form Indicator bit-value)
	rx_control |= (0<<22);	// DPF=0 (Discard Pause Frames)
	rx_control |= (0<<23);	// PMCF=0 (Pass MAC Control Frames)
	rx_control |= (0<<25);	// BSEX=0 (Buffer Size Extension)
	rx_control |= (1<<26);	// SECRC-bit (Strip Ethernet CRC)
	rx_control |= (0<<27);	// FLXBUF=0 (Flexible Buffer-size)
	iowrite32( rx_control, io + E1000_RCTL );

	iowrite32( 0x000C0241, io + E1000_CTRL );	// Device Control
	iowrite32( 0x1C1401C0, io + E1000_CTRL_EXT );	// Extended Control
	iowrite32( descaddr + 0x100, io + E1000_RDBAL );	
	iowrite32( 0x00000000, io + E1000_RDBAH );
	iowrite32( 0x00000080, io + E1000_RDLEN );
	iowrite32( 0x00000000, io + E1000_RDH );
	iowrite32( 0x00000008, io + E1000_RDT );
	iowrite32( 0x01010000, io + E1000_RXDCTL );

	// disable receive-checksum offloading and packet-splitting
	iowrite32( 0x00000000, io + E1000_RXCSUM );	// Rx-Checksum Control
	iowrite32( 0x00000000, io + E1000_RFCTL );	// Rx-Filter Control
	iowrite32( 0x00000000, io + E1000_PSRCTL );	// Packet-Split Rx Ctrl

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
	iowrite32( descaddr + 0x200, io + E1000_TDBAL );
	iowrite32( 0x00000000, io + E1000_TDBAH );
	iowrite32( 0x00000080, io + E1000_TDLEN );
	iowrite32( 0x00000000, io + E1000_TDH );
	iowrite32( 0x00000000, io + E1000_TDT );
	iowrite32( 0x01010000, io + E1000_TXDCTL );

	// enable the receive engine
	iowrite32( rx_control | (1<<1), io + E1000_RCTL );

	// now turn on the 'transmit' engine
	iowrite32( tx_control | (1<<1), io + E1000_TCTL );

	// enable interrupts
	iowrite32( 0xFFFFFFFF, io + E1000_ICR );
	iowrite32( INTR_MASK, io + E1000_IMS );

	// create pseudo-file and register this device-driver
	create_proc_info_entry( pseudorx, 0, NULL, my_get_info_rx );
	printk( " created pseudo-file \'/proc/%s\' \n", pseudorx );
	create_proc_info_entry( pseudotx, 0, NULL, my_get_info_tx );
	printk( " created pseudo-file \'/proc/%s\' \n", pseudotx );
	return	register_chrdev( my_major, devname, &my_fops );
}










static void __exit my_exit(void )
{
	int	rx_control = ioread32( io + E1000_RCTL );
	int	tx_control = ioread32( io + E1000_TCTL );

	// disable the transmit engine 
	iowrite32( tx_control & ~(1<<1), io + E1000_TCTL );

	// disable the receive engine 
	iowrite32( rx_control & ~(1<<1), io + E1000_RCTL );

	// disable interrupts
	iowrite32( INTR_MASK, io + E1000_IMC );

	// release this driver's kernel resources
	free_irq( irq, modname );
	unregister_chrdev( my_major, devname );
	remove_proc_entry( pseudotx, NULL );
	remove_proc_entry( pseudorx, NULL );
	kfree( kmem );
	iounmap( io );

	// issue confirmation-message to the log-file
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

