//-------------------------------------------------------------------
//	trytimer.c
//
//	This module demonstrates the use of a dynamic kernel timer.
//
//	NOTE: Written and tested using Linux kernel version 2.6.10.
//
//	programmer: ALLAN CRUSE
//	written on: 18 MAR 2005
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/timer.h>

#define	SECONDS_COUNT	10

char modname[] = "trytimer";	// module's name
struct timer_list timer;	// kernel object
unsigned int up_seconds;	// my timer data

void my_timer_function( unsigned long data )
{
	unsigned int	*data_ptr = (unsigned int*)data;

	*data_ptr += SECONDS_COUNT;
	printk( "  up for %u seconds \n", up_seconds );
	mod_timer( &timer, jiffies + HZ * SECONDS_COUNT );	
}

int init_module( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// initialize our kernel timer
	init_timer( &timer );

	// setup our timer's structure fields
	timer.expires = jiffies + HZ * SECONDS_COUNT;
	timer.data = (unsigned int)&up_seconds;
	timer.function = my_timer_function;

	// add our timer to the kernel's timer queue
	add_timer( &timer );
	return	0;  // SUCCESS
}


void cleanup_module( void )
{
	int	status = del_timer_sync( &timer );

	printk( "<1>Removing \'%s\' module\n", modname );
	printk( "  status=%08X \n", status );
}

MODULE_LICENSE("GPL");

