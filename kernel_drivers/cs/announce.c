//-------------------------------------------------------------------
//	announce.c
//
//	This example shows how a Linux kernel module can write its 
//	output to the current console or graphical desktop window.
//
//		compile with: $ ./mmake announce.c
//		install with: $ /sbin/insmod announce.ko
//		remove using: $ /sbin/rmmod announce.ko
//
//	NOTE: Developed and tested with Linux kernel version 2.6.21.
//
//	programmer: ALLAN CRUSE
//	written on: 11 JUL 2007 
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/sched.h>	// for 'current'
#include <linux/tty.h>		// for 'tty_struct'

char modname[] = "announce";
char announce[ 80 ];

int my_init( void )
{
	int	len = 0;

	// setup pointer to the current tty 
	struct tty_struct *tty = current->signal->tty;

	// create our 'announce' message-string
	len += sprintf( announce+len, "\n\tKernel module \'%s.ko\' ", modname );
	len += sprintf( announce+len, "is being installed.\r\n\n" ); 

	// write our message to the current console
	tty->driver->write( tty, announce, len );

	return	0;  // SUCCESS;
}

void my_exit( void )
{
	struct tty_struct *tty = current->signal->tty;
	int	len = 0;

	len += sprintf( announce+len, "\n\tKernel module \'%s.ko\' ", modname );
	len += sprintf( announce+len, "is being removed.\r\n\n" );
	tty->driver->write( tty, announce, len );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL");

