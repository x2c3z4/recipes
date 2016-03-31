//-------------------------------------------------------------------
//	smpinfo.c
//
//	This module will create a pseudo-file named '/proc/smpinfo' 
//	which allows users to display the information in three data
//	structures that reside in your workstation's ROM-BIOS area. 
//	The format of these data-structures is documented online in 
//	the "Intel Multiprocessor Specification (version 1.4)"; you  
//	can find a 'link' on our class website (under 'Resources').
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.6
//
//	programmer: ALLAN CRUSE
//	written on: 25 SEP 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/io.h>		// for phys_to_virt(), virt_to_phys()

char modname[] = "smpinfo";	// name for our '/proc' file
unsigned char *mpfp;		// MP Floating Pointer address
unsigned char *mpct;		// MP Configuration Table address
unsigned short baselen;		// Base Configuration Table length

int my_get_info( char *buf, char **start, off_t off, int count )
{
	int	i, len = 0;
	
	len += sprintf( buf+len, "\n MP Floating Pointer structure\n" );
	for (i = 0; i < 16; i++) 
		len += sprintf( buf+len, " %02X", mpfp[i] );
	len += sprintf( buf+len, "\n" );


	len += sprintf( buf+len, "\n MP Configuration Table Header " );
	for (i = 0; i < 44; i++) 
		{
		if ( ( i % 16 ) == 0 ) len += sprintf( buf+len, "\n" );
		len += sprintf( buf+len, " %02X", mpct[i] );
		}
	len += sprintf( buf+len, "\n" );

	len += sprintf( buf+len, "\n Base Configuration Table entries \n" );
	for (i = 44; i < baselen; ) 
		{
		int	j, k = ( mpct[i] ) ? 8 : 20;
		for (j = 0; j < k; j++)
			len += sprintf( buf+len, " %02X", mpct[i+j] );
		len += sprintf( buf+len, "\n" );
		i += k;
		}
	len += sprintf( buf+len, "\n" );

	return	len;
}


static int __init my_init( void )
{
	unsigned long	where;

	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// search for the MP Floating Pointer structure
	for (where = 0xF0000; where < 0x100000; where += 16)
		{
		mpfp = (char*)phys_to_virt( where );
		if ( strncmp( mpfp, "_MP_", 4 ) == 0 ) break;
		}	
	if ( where == 0x100000 ) return -ENODEV;

	printk( "MP Floating Pointer structure found " );
	printk( "at physical-address %08lX \n", where ); 

	// initialize pointer to the MP Configuration Table
	where = (*(unsigned long*)(mpfp+4));
	mpct = (unsigned char*)phys_to_virt( where );
	printk( "MP Configuration Table Header at %08lX \n", where );

	// initialize location and size of the Base Configuration Table
	baselen = *(int*)(mpct+4);

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

