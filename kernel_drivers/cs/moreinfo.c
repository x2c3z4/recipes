//-------------------------------------------------------------------
//	moreinfo.c
//
//	This module shows how you could create a dynamic file in the 
//	'/proc' directory that is able to show more information than 
//	would fit in the buffer the kernel provides to 'get_info()'.
//	It is based on some comments from <linux/fs/proc/generic.c>.
//	
//	NOTE: Written and tested with Linux kernel version 2.6.22.6.
//
//	programmer: ALLAN CRUSE
//	written on: 02 SEP 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 

#define N_BYTES	  	8192	// 8-KB exceeds kernel's buffer

char modname[] = "moreinfo";	// name for '/proc' pseudo-file
char mod_buf[ N_BYTES ];	// module's own storage-storage

int my_get_info( char *buf, char **start, off_t offset, int buf_len )
{
	int	n_bytes = 0;	// default value (for 'end-of-file')

	if ( offset < N_BYTES )	// some data remains to be absorbed 
		{
		*start = mod_buf + offset;
		n_bytes = N_BYTES - offset;
		if ( n_bytes > buf_len ) n_bytes = buf_len;
		}

	if ( offset == N_BYTES ) // module's buffer-data exhausted 
		{
		*start = buf;
		n_bytes = sprintf( buf, "\n" );
		}

	return	n_bytes;	// how far 'offset' will be advanced
}

static int __init my_init( void )
{
	int	i;

	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// initialize our 8-KB 'mod_buf[]' array with ascii data
	for (i = 0; i < 8; i++)
		memset( &mod_buf[ i * 1024 ], i + '0', 1024 );

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

