//-------------------------------------------------------------------
//	trymacro.c
//
//	This module demonstrates use of the 'sizeof' and 'offsetof'
//	operators, and the newly introduced 'container_of()' macro.	
//
//		install and execute:  $ cat /proc/trymacro
//
//	NOTE: Written and tested for Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 25 OCT 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <linux/stddef.h>	// for 'container_of()'

char modname[] = "trymacro";

struct mystruct	{
		char		w;
		short		x;
		long		y;
		long long	z;
		} my_instance = { 1, 2, 3, 4 };

const int nbytes = sizeof( my_instance );
const int off_z = offsetof( struct mystruct, z );

char structmsg[128] = "struct mystruct {\n"\
			"\t\tchar\t\tw;\n"\
			"\t\tshort\t\tx;\n"\
			"\t\tlong\t\ty;\n"\
			"\t\tlong long\tz;\n"\
			"\t\t} my_instance ="\
			" { 1, 2, 3, 4 }; \n";


int my_get_info( char *buf, char **start, off_t off, int count )
{
	int		len = 0;
	long		*ptr = &my_instance.y;
	struct mystruct	*p = container_of( ptr, struct mystruct, y ); 
	int		off_w = offsetof( struct mystruct, w );
	int		off_x = offsetof( struct mystruct, y );
	int		off_y = offsetof( struct mystruct, z );
	
	len += sprintf( buf+len, "\n\n\n%s\n", structmsg );	
	len += sprintf( buf+len, "sizeof(my_instance)=%d bytes \n", nbytes );
	len += sprintf( buf+len, "\n\n" );
	len += sprintf( buf+len, "offset of member w = %d bytes \n", off_w );
	len += sprintf( buf+len, "offset of member x = %d bytes \n", off_x );
	len += sprintf( buf+len, "offset of member y = %d bytes \n", off_y );
	len += sprintf( buf+len, "offset of member z = %d bytes \n", off_z );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "value of member w = %d   \n", p->w );
	len += sprintf( buf+len, "value of member x = %d   \n", p->x );
	len += sprintf( buf+len, "value of member y = %ld  \n", p->y );
	len += sprintf( buf+len, "value of member z = %lld \n", p->z );
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

