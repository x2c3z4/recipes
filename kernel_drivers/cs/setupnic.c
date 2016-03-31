//-------------------------------------------------------------------
//	setupnic.c	     (A revision of our 'tempcdev.c' demo)
//
//	This module executes some Linux system-calls which perform 
//	operations normally requiring 'root' privileges:
//
//		int sys_mknod( const char *, int, unsigned );
//		int sys_chmod( const char *, int );
//		int sys_unlink( const char * );
//
//	Its purpose is to create the '/dev/nic' device-node in case
//	that device special file does not yet exist; but whether it
//	existed or not, the 'module_init()' function will return an
//	error-indicator, preventing the module from being installed
//	and therefore saving us the possible bother of removing it.  
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 13 NOV 2007  
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <asm/uaccess.h>	// for get_ds(), set_fs()

char    name[] = "/dev/nic";
mode_t	mode = S_IFCHR | S_IRUGO | S_IWUGO;
dev_t	dev  = (97 << 8);
int	retval;

static int __init my_init( void )
{
	mm_segment_t	savedFS = get_fs();	// preserve address-limit

	set_fs( get_ds() );	// lets our module do the system-calls
	asm("	pushal			");
	asm(" 	mov	$14, %eax 	");
	asm("	lea	name, %ebx	");
	asm("	mov	mode, %ecx	");
	asm("	mov	dev, %edx	");
	asm("	int	$0x80		");
	asm("	or	%eax, %eax	");
	asm("	jnz	fail		");
	asm("	mov	$15, %eax	");
	asm("	lea	name, %ebx	");
	asm("	mov	mode, %ecx	");
	asm("	int	$0x80		");
	asm("fail:			");
	asm("	mov	%eax, retval	");
	asm("	popal			");
	set_fs( savedFS );	// restore the former address-limit value
	return -1;		// Do NOT install this module 
}

static void __exit my_exit( void ) { /* no work to do here */ }

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL");

