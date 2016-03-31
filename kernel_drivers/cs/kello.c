//-----------------------------------------------------------------
//	kello.c
//
//	This kernel module outputs a brief message to the console.
//
//		compile using: $ mmake hello 
//		install using: $ /sbin/insmod hello.ko
//
//	programmer: ALLAN CRUSE
//	written on: 23 AUG 2007
//-----------------------------------------------------------------

#include <linux/module.h>		// for printk()

int init( void )
{
	printk( "\n   Kello, everybody! \n\n" );

	return	0;
}

void exit( void )
{
	printk( "\n   Goodbye now... \n\n" );
}

MODULE_LICENSE("GPL");
module_init(init);
module_exit(exit);

