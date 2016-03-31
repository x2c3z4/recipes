//-------------------------------------------------------------------
//	pkmaps.c	(An LKM supporting our 'mapwatch.cpp' tool)
//
//	This module provides kernel support for an application that
//	implements a 'dynamic visualization' of the current entries
//	defined in the 'pkmap_page_table[]' array.  These establish
//	so-called 'permanent' kernel-mappings for page-frames which
//	reside in the kernel's 'high' memory zone, and periodically
//	they are changed as some old page-frames are 'unmapped' and 
//	some new page-frames get 'mapped in'.  In order to allow an
//	application to dramatize the 'dynamic' aspect of its screen
//	display, the 'jiffies_64' kernel-variable is also exported.    
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 01 NOV 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/highmem.h>	// for 'jiffies_64'
#include <asm/uaccess.h>	// for copy_to_user()
#include <asm/io.h>		// for phys_to_virt()

#define PROC_MODE	0444	// access-attributes for pseudo-file 
#define PROC_DIR	NULL	// default directory for pseudo-file

char modname[] = "pkmaps";
char jifname[] = "jiffies";
void *pgtbl;

ssize_t my_read( struct file *file, char *buf, size_t len, loff_t *pos )
{
	if ( *pos > PAGE_SIZE ) return 0;
	if ( *pos + len > PAGE_SIZE ) len = PAGE_SIZE - *pos;
	if ( copy_to_user( buf, pgtbl + *pos, len ) ) return -EFAULT; 
	*pos += len;
	return	len;
}

loff_t my_llseek( struct file *file, loff_t where, int whence )
{
	loff_t	newpos = -1;

	switch ( whence )
		{
		case 0:	newpos = where; break;
		case 1: newpos = file->f_pos + where; break;
		case 2: newpos = PAGE_SIZE + where; break;
		}

	if (( newpos < 0 )||( newpos > PAGE_SIZE )) return -EINVAL;
	file->f_pos = newpos;
	return	newpos;
}

struct file_operations	my_fops = {
				  owner:	THIS_MODULE,
				  llseek:	my_llseek,
				  read:		my_read,
				  };


int my_get_info( char *buf, char **start, off_t off, int count )
{
	return	sprintf( buf, "%020llu = jiffies\n", jiffies_64 );
}


static int __init my_init( void )
{
	struct proc_dir_entry	*entry;
	unsigned long		ptdb, *pgdir;

	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// inline assembly-language is used here to access register CR3
	asm(" mov %%cr3, %%eax \n mov %%eax, %0 " : "=m" (ptdb) :: "ax" );
	pgdir = phys_to_virt( ptdb );
	pgtbl = phys_to_virt( pgdir[1022] & ~0xFFF );	

	create_proc_info_entry( jifname, PROC_MODE, PROC_DIR, my_get_info );
	entry = create_proc_entry( modname, PROC_MODE, PROC_DIR );
	entry->proc_fops = &my_fops;
	return	0;  //SUCCESS
}


static void __exit my_exit(void )
{
	remove_proc_entry( modname, PROC_DIR );
	remove_proc_entry( jifname, PROC_DIR );

	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

