//-------------------------------------------------------------------
//	myserial.c
//
//	This driver gives a minimal working solution for Project #2.
//	Its 'module_init()' routine programs the UART's baudrate and 
//	its data-format, configuring the UART to operate in non-FIFO 
//	polled-mode (i.e., without any interrupts); the 'read()' and 
//	'write()' functions use the RTS/CTS signals for handshaking, 
//	and the null-modem cable's DSR/DTS signal-lines aren't used.
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 24 OCT 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/fs.h>		// for register_chrdev()
#include <linux/sched.h>	// for schedule()
#include <asm/uaccess.h>	// for copy_to/from_user()
#include <asm/io.h>		// for inb(), outb()

#define UARTBASE 	0x03F8
#define DIVISOR_LATCH	(UARTBASE+0)
#define TRANSMIT_DATA	(UARTBASE+0)
#define RECEIVE_DATA	(UARTBASE+0)
#define INTR_ENABLE	(UARTBASE+1)
#define INTR_IDENTIFY	(UARTBASE+2)
#define FIFO_CONTROL	(UARTBASE+2)
#define LINE_CONTROL	(UARTBASE+3)
#define MODEM_CONTROL	(UARTBASE+4)
#define LINE_STATUS	(UARTBASE+5)
#define MODEM_STATUS	(UARTBASE+6)
#define UART_SCRATCH	(UARTBASE+7)

char modname[] = "myserial";
char devname[] = "uart";
int my_major = 84;

ssize_t my_read( struct file *file, char *buf, size_t len, loff_t *pos )
{
	int	datum;

	// raise the 'Clear-To-Send' indicator
	outb( 0x02, MODEM_CONTROL );	// set RTS=1 

	// spin until some received data is ready 
	while ( ( inb( LINE_STATUS ) & 0x01 ) == 0x00 )
		{
		schedule();	// yield the CPU to other tasks
		if ( signal_pending( current ) ) return -EINTR;
		}

	// lower the 'Clear-To-Send' indicator
	outb( 0x00, MODEM_CONTROL );	// set RTS = 0 

	// input one received character and put it into user's buffer
	datum = inb( RECEIVE_DATA );
	if ( put_user( datum, buf ) ) return -EFAULT;

	// tell the kernel that one byte has been transferred	
	return	1;
}

ssize_t my_write( struct file *file, const char *buf, size_t len, loff_t *pos )
{
	int	datum;

	// turn on the 'Request-To-Send' signal
	outb( 0x02, MODEM_CONTROL );	// RTS=1

	// wait until the 'Clear-To-Send' status is 'true'
	while ( ( inb( MODEM_STATUS )&0x10 ) == 0x00 )
		{
		schedule();	// yield the CPU to other tasks
		if ( signal_pending( current ) ) return -EINTR;
		}

	// turn off the Request-To-Send signal
	outb( 0x00, MODEM_CONTROL );	// RTS=0

	// spin if necessary until THRE==1
	while ( ( inb( LINE_STATUS ) & 0x20 ) == 0x00 );

	// get a byte from the user's buffer and output it to the UART
	if ( get_user( datum, buf ) ) return -EFAULT;
	outb( datum, TRANSMIT_DATA );

	// tell the kerrnel that one data-byte was transferred
	return	1;
}

struct file_operations my_fops = { owner:	THIS_MODULE,
				   read:	my_read,
				   write:	my_write, };
				
static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d) \n", my_major );

	// configure the UART for non-FIFO polled-mode operation
	outb( 0x00, INTR_ENABLE );	// no interrupts
	outb( 0x00, FIFO_CONTROL ); 	// non-FIFO mode
	outb( 0x83, LINE_CONTROL );	// set DLAB=1
	outw( 0x0001, DIVISOR_LATCH );	// maximum baudrate
	outb( 0x03, LINE_CONTROL );	// 8-N-1 data-frame
	outb( 0x00, MODEM_CONTROL );	// RTS=0 

	return	register_chrdev( my_major, devname, &my_fops );
}

static void __exit my_exit(void )
{
	unregister_chrdev( my_major, devname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

