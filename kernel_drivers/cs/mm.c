//-------------------------------------------------------------------
//	mm.c
//
//	This module creates a pseudo-file (named '/proc/mm') which
//	shows some info from a task's memory-management structure.
//
//	NOTE: Written and tested with Linux kernel version 2.6.10.
//
//	programmer: ALLAN CRUSE
//	written on: 14 FEB 2005
//	revised on: 04 OCT 2007 -- for Linux kernel version 2.6.22.5
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/sched.h>	// for 'struct task_struct'
#include <linux/mm.h>		// for 'struct mm_struct'

int my_get_info( char *buf, char **start, off_t off, int count )
{
	struct task_struct	*tsk = current;
	struct mm_struct	*mm = tsk->mm;
	unsigned long		stack_size = (mm->stack_vm << PAGE_SHIFT);
	unsigned long		down_to = mm->start_stack - stack_size;
	int			len;

	len = sprintf( buf, "\nInfo from the Memory Management structure " );
	len += sprintf( buf+len, "for task \'%s\' ", tsk->comm );
	len += sprintf( buf+len, "(pid=%d) \n", tsk->pid );
	len += sprintf( buf+len, "   pgd=%08lX  ", (unsigned long)mm->pgd );
	len += sprintf( buf+len, "mmap=%08lX  ", (unsigned long)mm->mmap );
	len += sprintf( buf+len, "map_count=%d  ", mm->map_count );
	len += sprintf( buf+len, "mm_users=%d  ", mm->mm_users.counter );
	len += sprintf( buf+len, "mm_count=%d  ", mm->mm_count.counter );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "    start_code=%08lX  ", mm->start_code );
	len += sprintf( buf+len, " end_code=%08lX\n", mm->end_code );
	len += sprintf( buf+len, "    start_data=%08lX  ", mm->start_data );
	len += sprintf( buf+len, " end_data=%08lX\n", mm->end_data );
	len += sprintf( buf+len, "     start_brk=%08lX  ", mm->start_brk );
	len += sprintf( buf+len, "      brk=%08lX\n", mm->brk );
	len += sprintf( buf+len, "     arg_start=%08lX  ", mm->arg_start );
	len += sprintf( buf+len, "  arg_end=%08lX\n", mm->arg_end );
	len += sprintf( buf+len, "     env_start=%08lX  ", mm->env_start );
	len += sprintf( buf+len, "  env_end=%08lX\n", mm->env_end );
	len += sprintf( buf+len, "   start_stack=%08lX  ", mm->start_stack );
	len += sprintf( buf+len, "  down_to=%08lX ", down_to );
	len += sprintf( buf+len, " <--- stack grows downward \n" );
	len += sprintf( buf+len, "\n" );
	return	len;
}





int init_module( void )
{
	create_proc_info_entry( "mm", 0, NULL, my_get_info );
	return	0;  // SUCCESS
}



void cleanup_module( void )
{
	remove_proc_entry( "mm", NULL );
}

MODULE_LICENSE("GPL");

