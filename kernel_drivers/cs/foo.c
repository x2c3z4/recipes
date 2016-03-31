//-------------------------------------------------------------------
//	foo.c
//
//	This character-mode device-driver lets applications read
//	the current value of the kernel's 'jiffies_64' variable,
//	using the generic '/dev/foo' file setup by our SysAdmin.
//	(It will be used with our 'watchfoo.cpp' demonstration.)
//
//	NOTE: Written and tested for Linux kernel version 2.6.10
//
//	programmer: ALLAN CRUSE
//	written on: 20 MAR 2005
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/fs.h>		// for register_chrdev()
#include <asm/uaccess.h>	// for copy_to_user()
#include <linux/jiffies.h>	// for jiffies_64

char modname[] = "foo";
int mymajor = 96;

ssize_t myread( struct file *file, char *buf, size_t count, loff_t *pos )
{
	void	*from = (void*)&jiffies_64;

	if ( count > 8 ) count = 8;
	if ( copy_to_user( buf, from, count ) ) return -EFAULT;	
	return	count;
}

struct file_operations myfops = {
				owner:		THIS_MODULE,
				read:		myread,
				};


int init_module( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(major=%d)\n", mymajor );
	return	register_chrdev( mymajor, modname, &myfops );
}


void cleanup_module( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );
	unregister_chrdev( mymajor, modname );
}

MODULE_LICENSE("GPL");

