//-------------------------------------------------------------------
//	cr3.c
//
//	This kernel module creates a pseudo-file named '/proc/cr3'
//	which allows an application program to discover the values 
//	that currently reside in the processor's control registers 
//	CR3 and CR4, and it explicitly interprets the state of the
//	PAE-bit (bit #5: Page Address Extensions), and the PSE-bit
//	(bit #4: Page Size Extensions), from control register CR4.
//	Together, these register-values determine the scheme being
//	used by the CPU to locate and interpret its 'page-mapping' 
//	tables for virtual-to-physical memory-address translation.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.
//
//	programmer: ALLAN CRUSE
//	written on: 26 AUG 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 

char modname[] = "cr3";

int my_get_info( char *buf, char **start, off_t off, int count )
{
	unsigned int	_cr3, _cr4;
	int		len = 0;

	asm( " mov %%cr3, %%eax \n mov %%eax, %0 " : "=m" (_cr3) :: "ax" );
	asm( " mov %%cr4, %%eax \n mov %%eax, %0 " : "=m" (_cr4) :: "ax" );

	len += sprintf( buf+len, "    CR3=%08X", _cr3 );
	len += sprintf( buf+len, "    CR4=%08X", _cr4 );
	len += sprintf( buf+len, "    PAE=%X", (_cr4 >> 5)&1 );
	len += sprintf( buf+len, "    PSE=%X", (_cr4 >> 4)&1 );
	len += sprintf( buf+len, "\n" );
	return	len;
}

static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}

static void __exit my_exit(void )
{
	remove_proc_entry( modname, NULL );
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

