//-------------------------------------------------------------------
//	stash.c
//
//	This module implements a public clipboard (named 'stash').
//	Anything written to this device by one process can then be 
//	read back by any other process.  (A ring-buffer is used by
//	this driver for its data storage.)  If no data is present,
//	the reader sleeps (until awakened by a writer); if no more
//	space is available, the writer sleeps (until awakened by a
//	reader).  You may notice 'race conditions' when using this
//	driver with multiple readers or with multiple writers.  To
//	use this driver, it is necessary to have a device-node:
//
//		     root#  mknod /dev/stash c 40 0 
//		     root#  chmod a+rw /dev/stash
//
//	NOTE: Written and tested with Linux kernel version 2.4.18.  
//
//	programmer: ALLAN CRUSE
//	date begun: 31 JAN 2003
//	completion: 02 FEB 2003
//	revised on: 13 JUN 2004 -- for Linux kernel version 2.4.23
//	revised on: 25 OCT 2004 -- for Linux kernel version 2.4.26
//	revised on: 28 MAR 2005 -- for Linux kernel version 2.6.10 
//	revised on: 14 JUL 2007 -- for Linux kernel version 2.6.21
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/fs.h>		// for register_chrdev()
#include <linux/sched.h>	// for TASK_INTERRUPTIBLE
#include <asm/uaccess.h>	// for get_user(), put_user()

#define RINGSIZE 512

char modname[] = "stash";
int my_major = 40;
unsigned char ring[ RINGSIZE ];
volatile int head = 0, tail = 0;
wait_queue_head_t  wq_rd, wq_wr;


ssize_t
my_read( struct file *file, char *buf, size_t count, loff_t *pos )
{
	// sleep if necessary until our ringbuffer has some data 
	if ( wait_event_interruptible( wq_rd, head != tail ) )
		return -ERESTARTSYS;

	// remove a byte of data from our ringbuffer
	if ( put_user( ring[ head ], buf ) ) return -EFAULT;
	head = ( 1 + head ) % RINGSIZE;

	// now awaken any sleeping writers
	wake_up_interruptible( &wq_wr );
	return	1;
}

ssize_t
my_write( struct file *file, const char *buf, size_t count, loff_t *pos )
{
	int	next = (1 + tail) % RINGSIZE;

	// sleep if necessary until our ringbuffer has room for more data
	if ( wait_event_interruptible( wq_wr, next != head ) ) 
		return -ERESTARTSYS; 

	// insert a new byte of data into our ringbuffer
	if ( get_user( ring[ tail ], buf ) ) return -EFAULT; 
	tail = (1 + tail) % RINGSIZE;

	// now awaken any sleeping readers 
	wake_up_interruptible( &wq_rd );
	return	1;
}


static struct file_operations 
my_fops =	{
		owner:		THIS_MODULE,
		write:		my_write,
		read:		my_read,
		};


int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d) \n", my_major );

	// initialize our wait-queue structures
	init_waitqueue_head( &wq_rd );
	init_waitqueue_head( &wq_wr );
	
	// register this device-driver with the kernel
	return	register_chrdev( my_major, modname, &my_fops );
}


void __exit my_exit( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );

	// unregister this device-driver 
	unregister_chrdev( my_major, modname );
}


module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");

