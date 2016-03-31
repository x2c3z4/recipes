//-------------------------------------------------------------------
//	tempcdev.c
//
//	This module executes some Linux system-calls which perform 
//	operations normally requiring 'root' privileges:
//
//		int sys_mknod( const char *, int, unsigned );
//		int sys_chmod( const char *, int );
//		int sys_unlink( const char * );
//
//	We need to have the '/dev/cmos' character-mode device-file 
//	available for our first in-class exercise on device-driver
//	programming for Linux.  
//
//	NOTE: Written and tested using Linux kernel version 2.6.22.
//
//	programmer: ALLAN CRUSE
//	written on: 30 AUG 2007
//	revised on: 31 AUG 2007 -- to close a security-vulnerability
//	revised on: 03 SEP 2007 -- corrected 'dev_t' parameter-value  
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <asm/uaccess.h>	// for get_ds(), set_fs()

char modname[] = "tempcdev";
char 	path[] = "/dev/cmos";
mode_t	mode = S_IFCHR | S_IRUSR | S_IRGRP | S_IROTH;
dev_t	dev  = (70 << 8);	// <--- modified on 9/3/07
int	retval;

static int __init my_init( void )
{
	mm_segment_t	savedFS = get_fs();	// preserve address-limit

	printk( "<1>\nInstalling \'%s\' module\n", modname );

	set_fs( get_ds() );	// lets our module do the system-calls
	asm("	pushal			");
	asm(" 	mov	$14, %eax 	");
	asm("	lea	path, %ebx	");
	asm("	mov	mode, %ecx	");
	asm("	mov	dev, %edx	");
	asm("	int	$0x80		");
	asm("	or	%eax, %eax	");
	asm("	jnz	fail		");
	asm("	mov	$15, %eax	");
	asm("	lea	path, %ebx	");
	asm("	mov	mode, %ecx	");
	asm("	int	$0x80		");
	asm("fail:			");
	asm("	mov	%eax, retval	");
	asm("	popal			");
	set_fs( savedFS );	// restore the former address-limit value

	if ( retval == 0 ) 
		printk( "successfully created \'%s\' device-node \n", path );
	else	printk( "could not create the \'%s\' device-node \n", path );

	return	retval;
}


static void __exit my_exit( void )
{
	mm_segment_t	savedFS = get_fs();	// preserve address-limit

	printk( "<1>Removing \'%s\' module\n", modname );

	set_fs( get_ds() );	// lets our module do the system-call
	asm("	pushal			");
	asm(" 	mov	$10, %eax 	");
	asm("	lea	path, %ebx	");
	asm("	int	$0x80		");
	asm("	mov	%eax, retval	");
	asm("	popal			");
	set_fs( savedFS );	// restore the former address-limit value

	if ( retval == 0 ) 
		printk( "successfully deleted \'%s\' device-node \n", path );
	else	printk( "could not delete the \'%s\' device-node \n", path );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL");

