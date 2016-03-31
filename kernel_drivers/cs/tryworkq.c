//-------------------------------------------------------------------
//	tryworkq.c
//
//	This module demonstrates the use of a kernel workqueue, as a
//	mechanism for scheduling work to be performed after a delay.
//
//	NOTE: Developed and tested with Linux kernel version 2.6.10.
//
//	programmer: ALLAN CRUSE
//	written on: 20 MAR 2005
//	revised on: 24 OCT 2007 -- for workqueue changes in 2.6.22.5
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/workqueue.h>	// for create_workqueue()

char modname[] = "tryworkq";

void dowork( struct work_struct * );
DECLARE_DELAYED_WORK( mywork, dowork );
struct workqueue_struct *myqueue;

void dowork( struct work_struct *dummy  )
{
	printk( "\n\n%15s: I am doing the delayed work now\n", modname );
}

int init_module( void )
{
	int	retval;

	printk( "<1>\nInstalling \'%s\' module\n", modname );
	myqueue = create_singlethread_workqueue( "mywork" );
	retval = queue_delayed_work( myqueue, &mywork, HZ*5 );
	printk( "retval=%d\n", retval );
	return	0;  // SUCCESS
}


void cleanup_module( void )
{
	printk( "<1>Removing \'%s\' module\n", modname );
	destroy_workqueue( myqueue );
}

MODULE_LICENSE("GPL");

