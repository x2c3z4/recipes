//-------------------------------------------------------------------
//	tasklist.c
//
//	This module creates a pseudo-file (named '/proc/tasklist')
//	that will display a list of all the system's active tasks.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.
//
//	programmer: ALLAN CRUSE
//	written on: 05 SEP 2004
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/sched.h>	// for init_task

char modname[] = "tasklist";
struct task_struct  *task;
int  n_elt;			// 'global' so value will be retained

int my_get_info( char *buf, char **start, off_t offset, int buflen )
{
	int	len = 0;	// reinitialized on each reentry 

	if ( offset == 0 )	// first time through this procedure
		{
		task = &init_task;	// starting list-element
		n_elt = 0;		// starting count-value
		}
	else if ( task == &init_task ) return 0;  // end-of-file
	
	len += sprintf( buf+len, "#%-3d ", ++n_elt );
	len += sprintf( buf+len, "%5d ", task->pid );
	len += sprintf( buf+len, "%lu ", task->state );
	len += sprintf( buf+len, "%-15s ", task->comm );
	len += sprintf( buf+len, "\n" );

	task = next_task( task );	// the 'next' list-element

	*start = buf;		// be sure to 'reenter' this procedure
	return	len;		// as long as return-value is non-zero
}

int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}

void __exit my_exit( void )
{
	remove_proc_entry( modname, NULL );
	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL"); 

