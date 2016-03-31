//-------------------------------------------------------------------
//	tasklet.c
//
//	This device-driver module illustrates a use of the kernel's
//	'tasklet' mechanism for deferring some of the work involved
//	in responding to the serial UART's 'data receive' interrupt
//	when FIFO-mode is enabled (i.e., this isr's 'bottom half').
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 22 OCT 2007 
//-------------------------------------------------------------------


#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/interrupt.h>	// for schedule_tasklet()
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
#define UART_IRQ	0x04
#define RINGSIZE	64


char modname[] = "tasklet";
char devname[] = "uart";
int my_major = 84;


struct semaphore sem_busy;
wait_queue_head_t wq_read;
struct tasklet_struct rx_tasklet;


struct ringbuf	{
		int	rxhead, rxtail;
		int	rxbuf[ RINGSIZE ];
		} dev_info;









irqreturn_t my_isr( int irq, void *dev_id )
{
	// read the UART interrupt-identification register
	int	intr_identify = inb( INTR_IDENTIFY )&0x0F;

	// exit immediately if no interrupt is pending
	if ( intr_identify == 0x01 ) return IRQ_NONE;

	// otherwise the UART's receiver-FIFO holds data
	// since we did not enable other UART interrupts
	
	// step 1: immediately tell the sender to pause  
	outb( 0x09, MODEM_CONTROL );	       // RTS=0

	// step 2: then schedule 'bottom half' handling
	tasklet_schedule( &rx_tasklet );  
	
	// step 3: finally, resume the interrupted task
	return	IRQ_HANDLED;
}





void my_rx_handler( unsigned long data )
{
	struct ringbuf	*mydev = (struct ringbuf *)data;
	int	i;

	// move all the receiver-FIFO's data into our rx-ringbuffer 
	for (i = 0; i < 16; i++)
		{
		// input byte from FIFO and store in ringbuffer
		mydev->rxbuf[ mydev->rxtail ] = inb( RECEIVE_DATA );
		// advance the ringbuffer array's 'tail' index
		mydev->rxtail = (1 + mydev->rxtail) % RINGSIZE;
		// stop when the receiver-FIFO has been emptied
		if ( ( inb( LINE_STATUS )&0x01 ) == 0x00 ) break;
		// otherwise make sure the ringbuffer is yet full
		}
		
	// then awaken any sleeping reader-tasks
	wake_up_interruptible( &wq_read );
}








int my_open( struct inode *inode, struct file *file )
{
	// we will allow only one user at a time
	down_interruptible( &sem_busy );

	// reset our rx-ringbuffer to 'empty'
	dev_info.rxhead = 0;
	dev_info.rxtail = 0;

	// raise the 'Data Terminal Ready' signal-line	
	outb( 0x09, MODEM_CONTROL );	// set DTR=1, RTS=0

	return 0;	// SUCCESS
}



ssize_t my_read( struct file *file, char *buf, size_t len, loff_t *pos )
{
	int	i, datum;

	// if no data is in our rx-ringbuffer, issue 'Request-To-Send'
	if ( dev_info.rxhead == dev_info.rxtail ) 
		outb( 0x0B, MODEM_CONTROL );	// issue RTS=1 and DTR=1
		
	// then sleep until some data is present in the rx-ringbuffer
	if ( wait_event_interruptible( wq_read, 
		dev_info.rxhead != dev_info.rxtail ) ) return -ERESTARTSYS;
		
	// stop the sender from transmitting more data
	outb( 0x09, MODEM_CONTROL );	// set RTS=0 

	// extract data from our rx-ringbuffer and deliver it to the user
	for (i = 0; i < len; i++)
		{
		// stop whenever our rx-ringbuffer becomes empty
		if ( dev_info.rxhead == dev_info.rxtail ) break;
		// copy the byte at the front of our rx-ringbuffer 
		datum = dev_info.rxbuf[ dev_info.rxhead ];
		// put that copied byte into the user's buffer
		if ( put_user( datum, buf+i ) ) return -EFAULT;
		// advance our ringbuffer array's 'head' index
		dev_info.rxhead = (1 + dev_info.rxhead) % RINGSIZE;
		}

	// in case our rx-ringbuffer is empty, issue 'Request-To-Send'
	if ( dev_info.rxhead == dev_info.rxtail ) 
		outb( 0x0B, MODEM_CONTROL );	// set RTS=1

	// tell kernel how many bytes were put into user's buffer
	return	i;
}




ssize_t my_write( struct file *file, const char *buf, size_t len, loff_t *pos )
{
	int	datum;

	// issue 'Request-To-Send'
	outb( 0x0A, MODEM_CONTROL );	// RTS=1, DTR=0

	// do busy-waiting until 'Clear-To-Send' becomes 'true'
	while ( ( inb( MODEM_STATUS )&0x10 ) == 0x00 )
		{
		schedule();
		if ( signal_pending( current ) ) return -EINTR;
		}

	// turn off the 'Request-To-Send' signal
	outb( 0x09, MODEM_CONTROL );	// RTS=0, DTR=1

	// do busy-waiting if necessary until THRE==1
	while ( ( inb( LINE_STATUS ) & 0x20 ) == 0x00 );

	// output a data-byte from the user's buffer to the UART
	if ( get_user( datum, buf ) ) return -EFAULT;
	outb( datum, TRANSMIT_DATA );

	// tell the kernel that we transferred one byte
	return	1;
}




int my_close( struct inode *inode, struct file *file )
{
	// lower the 'Request-To-Send' signal-line
	outb( 0x09, MODEM_CONTROL );	// set DTR=1, RTS=0

	// release the semaphore
	up( &sem_busy );

	return	0;	// SUCCESS
}



struct file_operations my_fops = {
				owner:		THIS_MODULE,
				open:		my_open,
				read:		my_read,
				write:		my_write,
				release:	my_close,
				};				





int my_get_info( char *buf, char **start, off_t off, int count )
{
	int	i, j, ch, len = 0;

	// display the current contents of our rx-ringbuffer
	for (i = 0; i < RINGSIZE; i+=16)
		{
		len += sprintf( buf+len, "\n %04X: ", i );
		for (j = 0; j < 16; j++)
			{
			ch = dev_info.rxbuf[ i+j ];
			len += sprintf( buf+len, "%02X ", ch );
			}
		for (j = 0; j < 16; j++)
			{
			ch = dev_info.rxbuf[ i+j ];
			if (( ch < 0x20 )||( ch > 0x7E )) ch = '.';
			len += sprintf( buf+len, "%c", ch );
			}
		}
	len += sprintf( buf+len, "\n\n" );
	
	// display the array's current 'head' and 'tail' indices
	len += sprintf( buf+len, " rxhead=0x%04X ", dev_info.rxhead );
	len += sprintf( buf+len, " rxtail=0x%04X ", dev_info.rxtail );
	len += sprintf( buf+len, "\n\n" );
	return	len;
}



static void __exit my_exit(void )
{
	// disable further UART interrupts and empty the FIFOs
	outb( 0x00, MODEM_CONTROL );
	outb( 0x00, INTR_ENABLE );
	outb( 0x00, FIFO_CONTROL );

	// flush any stale data
	while ( inb( LINE_STATUS )&0x01 ) inb( RECEIVE_DATA );
	inb( MODEM_STATUS );
	inb( INTR_IDENTIFY );

	// remove our UART interrupt-handler (including 'tasklet')
	free_irq( UART_IRQ, &dev_info );
	tasklet_kill( &rx_tasklet );

	// unregister this driver and delete our '/proc' pseudo-file
	unregister_chrdev( my_major, devname );
	remove_proc_entry( modname, NULL );

	printk( "<1>Removing \'%s\' module\n", modname );
}


static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d) \n", my_major );

	// initialize this driver's data-structures
	init_MUTEX( &sem_busy );
	init_waitqueue_head( &wq_read );
	tasklet_init( &rx_tasklet, my_rx_handler, (unsigned long)&dev_info );

	// install our handler for the UART's interrupts
	if ( request_irq( UART_IRQ, &my_isr, IRQF_DISABLED, 
			modname, &dev_info ) ) return -EBUSY;

	// configure the UART's operating mode
	outb( 0x00, INTR_ENABLE );
	outb( 0x00, FIFO_CONTROL ); 
	outb( 0x80, LINE_CONTROL );
	outw( 0x0001, DIVISOR_LATCH );
	outb( 0x03, LINE_CONTROL );
	outb( 0x09, MODEM_CONTROL );
	outb( 0x01, INTR_ENABLE );
	outb( 0xC3, FIFO_CONTROL );

	// eliminate any stale register-values
	inb( INTR_IDENTIFY );
	inb( MODEM_STATUS );
	inb( LINE_STATUS );
	inb( RECEIVE_DATA );

	// for debugging: confirm the contents of UART registers
	printk( " Modem Control: %02X \n", inb( MODEM_CONTROL ) );
	printk( "  Modem Status: %02X \n", inb( MODEM_STATUS ) );
	printk( "  Line Control: %02X \n", inb( LINE_CONTROL ) );
	printk( "   Line Status: %02X \n", inb( LINE_STATUS ) );
	printk( "   Intr Enable: %02X \n", inb( INTR_ENABLE ) );
	printk( " Intr Identify: %02X \n", inb( INTR_IDENTIFY ) );

	// create our '/proc' pseudo-file and register this driver
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	register_chrdev( my_major, devname, &my_fops );
}


module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

