//-------------------------------------------------------------------
//	newstash.c
//
//	This is a revision of our 'stash.c' module.  It uses a pair
//	of Linux kernel semaphores to enforce exclusive access by a
//	single writer and a single reader, so as to eliminate 'race
//	conditions' that were observed with the original 'stash.c'.    
//
//	NOTE: Written and tested using Linux kernel version 2.6.10.  
//
//	programmer: ALLAN CRUSE
//	written on: 30 MAR 2005
//	revised on: 14 JUL 2007 -- for Linux kernel version 2.6.21.
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/fs.h>		// for register_chrdev()
#include <linux/sched.h>	// for wait_event_interruptible() 
#include <asm/uaccess.h>	// for get_user(), put_user()

#define RINGSIZE 512

char modname[] = "stash";	// keeps former pseudo-file name
int my_major = 40;		// and same device-file major ID
unsigned char ring[ RINGSIZE ];
volatile int head = 0, tail = 0;
wait_queue_head_t  wq_rd, wq_wr;

// here two semaphores are added
struct semaphore sem_wr, sem_rd;  


int my_open( struct inode *inode, struct file *file )
{
	// if task wants to write, obtain 'write' semaphore (or sleep)
	if ( file->f_mode & FMODE_WRITE ) down_interruptible( &sem_wr );

	// if task wants to read, obtain 'read' semaphore (or sleep)
	if ( file->f_mode & FMODE_READ ) down_interruptible( &sem_rd );	

	return	0;  // SUCCESS
}


int my_release( struct inode *inode, struct file *file )
{
	// if task was a writer, then release the 'write' semaphore
	if ( file->f_mode & FMODE_WRITE ) up( &sem_wr );

	// if task was a reader, then release the 'read' semaphore
	if ( file->f_mode & FMODE_READ ) up( &sem_rd );	

	return	0;  // SUCCESS
}


ssize_t
my_read( struct file *file, char *buf, size_t count, loff_t *pos )
{
	// sleep if necessary until the ringbuffer has some data 
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
	// sleep if necessary until the ringbuffer has room for more data
	int	next = (1 + tail) % RINGSIZE;
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
		open:		my_open,
		release:	my_release,
		};

int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d) \n", my_major );

	// initialize our semaphores and wait-queue structures
	init_MUTEX( &sem_wr );
	init_MUTEX( &sem_rd );
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
