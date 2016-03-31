//-------------------------------------------------------------------
//	baudrate.c
//
//	This module creates a pseudo-file named '/proc/baudrate' to
//	allow users a convenient way of checking the UART settings.
//	It also implements an 'ioctl()' method for a character-mode 
//	Linux device-driver allowing applications to query or alter 
//	the serial-UART's current baudrate.  You are invited to add
//	additional IOCTL services which, for example, could provide 
//	a way for application programs to query and/or modify other  
//	serial-UART communication parameters (e.g., Line-Control).
//
//	NOTE: Written and tested using Linux kernel version 2.6.22.
//
//	programmer: ALLAN CRUSE
//	written on: 18 OCT 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/uaccess.h>	// for copy_to/from_user()
#include <asm/io.h>		// for inb(), outb()

#define UART_BASE	0x03F8
#define CLOCKRATE	115200

#define IOCTL_GET_BAUDRATE	0
#define IOCTL_SET_BAUDRATE	1

char modname[] = "baudrate";
char parity[] = "NONENMNS";

int my_get_info( char *buf, char **start, off_t off, int count )
{
	int	line_control, divisor_latch, baudrate, len = 0;

	// read the UART's communication parameters
	line_control = inb( UART_BASE + 3 );	// LINE CONTROL
	outb( line_control | 0x80, UART_BASE + 3 );   // DLAB=1
	divisor_latch = inw( UART_BASE + 0 );  // DIVISOR LATCH
	outb( line_control, UART_BASE + 3 );	// LINE CONTROL

	// calculate:  BAUD RATE = CLOCKRATE / DIVISOR_LATCH
	baudrate = CLOCKRATE / divisor_latch;

	// display the status report
	len += sprintf( buf+len, "\n  serial-UART communication parameters: " );
	len += sprintf( buf+len, " baudrate=%d  ", baudrate );
	len += sprintf( buf+len, "data-format: %d", 5 + ( line_control & 3 ) ); 
	len += sprintf( buf+len, "-%c", parity[ ( line_control >> 3 ) & 7 ] );
	len += sprintf( buf+len, "-%d \n", 1 + (( line_control >> 2 ) & 1) );
	len += sprintf( buf+len, "\n" );

	return	len;
}

int my_ioctl( struct inode *inode, struct file *file, 
		unsigned int request, unsigned long address )
{
	int	line_control, divisor_latch, baud_rate = 0;
	int	*where = (int*)address;

	switch ( request )
		{
		case IOCTL_GET_BAUDRATE:

			line_control = inb( UART_BASE + 3 );
			outb( inb( UART_BASE + 3 ) | 0x80, UART_BASE + 3 );
			divisor_latch = inw( UART_BASE + 0 );
			outb( line_control, UART_BASE + 3 );

			if ( divisor_latch > 0 )  	
				baud_rate = CLOCKRATE / divisor_latch;

			if ( copy_to_user( where, &baud_rate, 4 ) ) 
				return -EFAULT;
			return	0;


		case IOCTL_SET_BAUDRATE:
		
			if ( copy_from_user( &baud_rate, where, 4 ) )
				return -EFAULT; 
		
			if (( baud_rate <= 0 )||( baud_rate > CLOCKRATE ))
				return -EINVAL;

			divisor_latch = CLOCKRATE / baud_rate;

			line_control = inb( UART_BASE + 3 );
			outb( inb( UART_BASE + 3 ) | 0x80, UART_BASE + 3 );
			outw( divisor_latch, UART_BASE + 0 );
			outb( line_control, UART_BASE + 3 );
			return	0;
		}
	return	-EINVAL;	
}















struct file_operations my_fops = {
			 	owner:	THIS_MODULE,
				ioctl:	my_ioctl,
				};


char devname[] = "uart";
int my_major = 84;


static int __init my_init( void )
{
	int	scratch, absent = 0;

	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// verify the presence of a primary 16550 serial UART
	scratch = inb( UART_BASE + 7 );
	outb( 0x55, UART_BASE + 7 );
	if ( inb( UART_BASE + 7 ) != 0x55 ) absent |= 1;
	outb( 0xAA, UART_BASE + 7 );
	if ( inb( UART_BASE + 7 ) != 0xAA ) absent |= 1;
	outb( scratch, UART_BASE + 7 );
	if ( absent ) return -ENODEV;

	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	register_chrdev( my_major, devname, &my_fops );
}


static void __exit my_exit(void )
{
	unregister_chrdev( my_major, devname );
	remove_proc_entry( modname, NULL );

	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

