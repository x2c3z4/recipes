//-------------------------------------------------------------------
//	cmos.c
//
//	This Linux module creates a pseudo-file (named '/proc/cmos')
//	that lets a user see what current date and time is stored in 
//	this workstation's Real-Time Clock/Calendar chip, by typing:
//
//			     $ cat /proc/cmos
//
//	A privileged user may compile and install this module using:
//
//			user$ ./mmake cmos.c
//			root# /sbin/insmod cmos.ko
//
//	NOTE: Written and tested with Linux kernel version 2.6.17.6.
//
//	programmer: ALLAN CRUSE
//	written on: 24 JUN 2007
//	bug repair: 30 NOV 2007 -- converts BCD month-number to int
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/io.h>		// for inb(), outb()

char modname[] = "cmos";
unsigned char  cmos[10];
char *day[] = { "", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN" };
char *month[] = { "", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
			"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

int my_get_info( char *buf, char **start, off_t off, int count )
{
	int	i, len = 0;
	int	month_index;

	// input and store the first ten CMOS entries
	for (i = 0; i < 10; i++)
		{
		outb( i, 0x70 );
		cmos[i] = inb( 0x71 );
		}

	// show the current time and date
	len += sprintf( buf+len, "\n\t CMOS Real-Time Clock/Calendar:" );
	len += sprintf( buf+len, " %02X", cmos[4] );	// current hour
	len += sprintf( buf+len, ":%02X", cmos[2] );	// current minutes
	len += sprintf( buf+len, ":%02X", cmos[0] );	// current seconds
	len += sprintf( buf+len, " on" );
	len += sprintf( buf+len, " %s, ", day[ cmos[6] ] ); 	// day-name
	len += sprintf( buf+len, "%02X", cmos[7] );		// day-number

// BUG!	len += sprintf( buf+len, " %s", month[ cmos[8] ] );	// month-name

	// bug fix: convert 'cmos[ 8 ]' from BCD-format to integer-format
	month_index = ((cmos[ 8 ] & 0xF0)>>4)*10 + (cmos[ 8 ] & 0x0F);
	len += sprintf( buf+len, " %s", month[ month_index ] );	// month-name

	len += sprintf( buf+len, " 20%02X ", cmos[9] );		// year-number
	len += sprintf( buf+len, "\n" );

	len += sprintf( buf+len, "\n" );
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

//-----------------------------------------------------------------------
// Thanks to Brian Meagher for noticing that a conversion is required of
// the cmos-value for the month-number from BCD-format to integer-format
// if it is to be used as a lookup-index into the array month-names (not
// detected back during June, but evident in October/November/December).
//-----------------------------------------------------------------------

