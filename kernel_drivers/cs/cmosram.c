//-------------------------------------------------------------------
//	cmosram.c
//
//	This Linux Kernel Module implements a simple character-mode 
//	device-driver for the Real-Time Clock's non-volatile memory 
//	(128 bytes).  Although it fully supports read() and lseek() 
//	access, its support for write() access is restricted to the
//	clock-and-calendar locations -- the non-configuration data.  
//	
//	NOTE: Written and tested using Linux kernel version 2.6.22.
//
//	programmer: ALLAN CRUSE
//	written on: 03 SEP 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for printk() 
#include <linux/fs.h>		// for register_chrdev() 
#include <asm/uaccess.h>	// for put_user(), get_user()
#include <asm/io.h>		// for inb(), outb()

char modname[] = "cmosram";	// name of this kernel module
char devname[] = "cmos";	// name for the device's file
int	my_major = 70;		// major ID-number for driver
int	cmos_size = 128;	// total bytes of cmos memory
int	write_max = 9;		// largest 'writable' address

ssize_t my_read( struct file *file, char *buf, size_t len, loff_t *pos )
{
	unsigned char	datum;

	if ( *pos >= cmos_size ) return 0;

	outb( *pos, 0x70 );  datum = inb( 0x71 );

	if ( put_user( datum, buf ) ) return -EFAULT;

	*pos += 1;
	return	1;
}

ssize_t my_write( struct file *file, const char *buf, size_t len, loff_t *pos )
{
	unsigned char	datum;

	if ( *pos >= cmos_size ) return 0;

	if ( *pos > write_max ) return -EPERM;

	if ( get_user( datum, buf ) ) return -EFAULT;

	outb( *pos, 0x70 );  outb( datum, 0x71 );

	*pos += 1;
	return	1;
}

loff_t my_llseek( struct file *file, loff_t pos, int whence )
{
	loff_t	newpos = -1;

	switch ( whence )
		{
		case 0:	newpos = pos; break;			// SEEK_SET
		case 1: newpos = file->f_pos + pos; break;	// SEEK_CUR
		case 2: newpos = cmos_size + pos; break;	// SEEK_END
		}

	if (( newpos < 0 )||( newpos > cmos_size )) return -EINVAL;
	
	file->f_pos = newpos;
	return	newpos;
}


struct file_operations my_fops = {
				owner:	THIS_MODULE,
				llseek:	my_llseek,
				write:	my_write,
				read:	my_read,
				};

static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", devname );
	printk( "(major=%d) \n", my_major );
	return	register_chrdev( my_major, devname, &my_fops );
}

static void __exit my_exit(void )
{
	unregister_chrdev( my_major, devname );
	printk( "<1>Removing \'%s\' module\n", devname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

