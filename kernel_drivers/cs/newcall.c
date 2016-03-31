//-------------------------------------------------------------------
//	newcall.c
//
//	This module dynamically installs a new system-call, without
//	the need to modify and recompile the kernel's source files.
//	It is based on 'patching' an obsolete entry in the kernel's
//	'sys_call_table[]' array.  The address of that array can be
//	discovered, either in the 'System.map' file for the current
//	kernel (normally located in the '/boot' directory), or else
//	in the uncompressed kernel binary ELF file ('vmlinux') left
//	behind in the '/usr/src/linux' directory after compilation.
//	Some recent kernel-versions place the 'sys_call_table[]' in
//	a 'read-only' page-frame, so we employ a 'workaround' here.  
//
//	NOTE: Written and tested using Linux kernel version 2.4.17.
//
//	written on: 22 JUN 2004
//	revised on: 01 DEC 2007 -- for Linux kernel version 2.6.22.5
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <asm/uaccess.h>	// for copy_to/from_user()
#include <asm/unistd.h>		// for __NR_break
#include <asm/io.h>		// for phys_to_virt()

extern unsigned long *sys_call_table;

char modname[] = "newcall";
unsigned long save_old_syscall_entry;
unsigned long save_old_pgtable_entry;
unsigned int _cr4, _cr3;
unsigned int *pgdir, dindex;
unsigned int *pgtbl, pindex;


asmlinkage long my_syscall( int __user * num )
{
	int	val;

	if ( copy_from_user( &val, num, sizeof( int ) ) ) return -EFAULT;
	++val;
	if ( copy_to_user( num, &val, sizeof( int ) ) ) return -EFAULT; 

	return	0;  // SUCCESS
}	


static void __exit my_exit( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );
	sys_call_table[ __NR_break ] = save_old_syscall_entry;
	pgtbl[ pindex ] = save_old_pgtable_entry;
}



static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module ", modname );
	printk( "(sys_call_table[] at %08X) \n", (int)sys_call_table );

	// get current values from control-registers CR3 and CR4
	asm(" mov %%cr4, %%eax \n mov %%eax, _cr4 " ::: "ax" );
	asm(" mov %%cr3, %%eax \n mov %%eax, _cr3 " ::: "ax" );

	// confirm that processor is using the legacy paging mechanism
	if ( (_cr4 >> 5) & 1 ) 
		{ 
		printk( " processor is using Page-Address Extensions \n");
		return 	-ENOSYS;
		} 

	// extract paging-table indices from 'sys_call_table[]' address 
	dindex = ((int)sys_call_table >> 22) & 0x3FF;	// pgdir-index
	pindex = ((int)sys_call_table >> 12) & 0x3FF;	// pgtbl-index

	// setup pointers to the page-directory and page-table frames
	pgdir = phys_to_virt( _cr3 & ~0xFFF );
	pgtbl = phys_to_virt( pgdir[ dindex ] & ~0xFFF );	

	// preserve page-table entry for the 'sys_call_table[]' frame
	save_old_pgtable_entry = pgtbl[ pindex ];	

	printk( "\nInstalling new function for system-call %d\n", __NR_break );
	pgtbl[ pindex ] |= 2;	// make sure that page-frame is 'writable'
	save_old_syscall_entry = sys_call_table[ __NR_break ];
	sys_call_table[ __NR_break ] = (unsigned long)my_syscall;
	return	0;  // SUCCESS
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");

