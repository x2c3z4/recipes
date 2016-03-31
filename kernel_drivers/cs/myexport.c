//-------------------------------------------------------------------
//	myexport.c
//
//	This module provides a convenient 'workaround' for the fact 
//	that newer versions of the official Linux kernel sources do 
//	not export the 'sys_call_table[]' array as a public symbol.  
//
//	Using an accompanying shell script ('myscript'), the kernel 
//	address for the 'sys_call_table[]' symbol is extracted from 
//	the uncompressed kernel ('vmlinux'), and then supplied as a
//	command-line argument during installation of this module.
//
//	NOTE: Written and tested using Linux kernel version 2.4.17.
//
//	programmer: ALLAN CRUSE
//	written on: 24 JUN 2004
//	revised on: 01 DEC 2007 -- for Linux kernel version 2.6.22.5
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/kernel.h>	// for sscanf()

char modname[] = "myexport";
unsigned long myval;

char *svctable = "blah";
module_param(svctable, charp, 0444 );

unsigned long *sys_call_table;
EXPORT_SYMBOL(sys_call_table);

static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "with svctable=%s \n", svctable );

	myval = simple_strtoul( svctable, 0, 16 );
	if ( myval == 0 ) return -EINVAL;
	sys_call_table = (unsigned long*)myval;

	printk( "<1>  sys_call_table[] at %p \n", sys_call_table );
	return	0;  // SUCCESS
}

static void __exit my_exit( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
