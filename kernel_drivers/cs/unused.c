//-------------------------------------------------------------------
//	unused.c
//
//	This module creates a pseudo-file named '/proc/unused' that
//	shows which entries in the 'sys_call_table[]' are obsolete.
//	You need to execute our 'myscript' file in order to install 
//	our 'myexport.ko' kernel-object BEFORE installing this LKM.
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 11 DEC 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/utsname.h>	// for utsname()
#include <asm/unistd.h>		// for NR_sys_calls

char modname[] = "unused";
char legend[] = "List of the obsolete sys_call_table[] entries: ";
extern unsigned long *sys_call_table;

int my_get_info( char *buf, char **start, off_t off, int count )
{
	struct new_utsname	*uts = utsname();
	int			i, nelts = 0, len = 0;

	len += sprintf( buf+len, "\n %s\n", legend );
	for (i = 0; i < NR_syscalls; i++)
		if ( sys_call_table[ i ] == sys_call_table[ __NR_break ] )
			{
			if ( ( nelts % 10 ) == 0 ) 
				len += sprintf( buf+len, "\n" );
			++nelts;
			len += sprintf( buf+len, "%5d ", i );
			}
	len += sprintf( buf+len, "\n" );

	len += sprintf( buf+len, "\n Number of obsolete entries on " );
	len += sprintf( buf+len, "\'%s\' = %d \n", uts->nodename, nelts );
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

