//-------------------------------------------------------------------
//	defermsg.c
//
//	This module demonstrates the use of a workqueue to defer the
//	appearance of a 'hello' message on the screen for 10 secons.
//
//	NOTE: Developed and tested with Linux kernel version 2.6.10.
//
//	programmer: ALLAN CRUSE
//	written on: 22 MAR 2005
//	revised on: 24 OCT 2007 -- for workqueue changes in 2.6.22.5
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/sched.h>	// for 'init_work()'
#include <linux/tty.h>		// for 'struct tty_struct'

char modname[] = "defermsg";
struct workqueue_struct *myqueue;
struct delayed_work mywork;
char message[] = "\r\n\nHello!\r\n\n";

struct data	{
		struct tty_struct	*tty;
		} my_data;

void dowork( struct work_struct *work )
{
	struct data *foo = container_of( &my_data.tty, struct data, tty );
	struct tty_struct *tty = foo->tty;

	tty->driver->write( tty, message, strlen( message ) );
}

int init_module( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	myqueue = create_singlethread_workqueue( "mywork" );
	INIT_DELAYED_WORK( &mywork, dowork );
	my_data.tty = current->signal->tty;
	if ( !queue_delayed_work( myqueue, &mywork, HZ * 10 ) )
		return -EBUSY; 
	return	0;  // SUCCESS
}

void cleanup_module( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );

	if ( !cancel_delayed_work( &mywork ) )
		flush_workqueue( myqueue );
	destroy_workqueue( myqueue );
}

MODULE_LICENSE("GPL");

