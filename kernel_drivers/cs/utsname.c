//-------------------------------------------------------------------
//	utsname.c
//
//	This example shows how your modules can display information
//	about your system which is stored within your Linux kernel.
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 27 OCT 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/utsname.h>	// for utsname();

char modname[] = "utsname";

int my_get_info( char *buf, char **start, off_t off, int count )
{
	struct new_utsname	*uts = utsname();
	int	len = 0;

	len += sprintf( buf+len, "\n\n Information from 'utsname' \n\n" );
	len += sprintf( buf+len, "    sysname: \'%s\' \n", uts->sysname );
	len += sprintf( buf+len, "    release: \'%s\' \n", uts->release );
	len += sprintf( buf+len, "   nodename: \'%s\' \n", uts->nodename );
	len += sprintf( buf+len, "    version: \'%s\' \n", uts->version );
	len += sprintf( buf+len, "    machine: \'%s\' \n", uts->machine );
	len += sprintf( buf+len, " domainname: \'%s\' \n", uts->domainname );
	len += sprintf( buf+len, "\n\n" );
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

