//----------------------------------------------------------------
//	setconsole.cpp
//
//	This utility allows a user who possesses root privileges 
//	to redirect 'printk()' messages to a designated console. 
//
//	compile using:
//		root# gcc setconsole.cpp -o setconsole
//		root# chmod a+s setconsole
//
//	execute using:
//		user$ ./setconsole 4	
//		
//	This code is an 'updated' version of an example appearing 
//	in "Linux Device Drivers (3rd Edition)" on page 76.
//	          
//	programmer: ALLAN CRUSE
//	written on: 26 JUN 2007
//----------------------------------------------------------------

#include <fcntl.h>		// for open()		 
#include <stdio.h>		// for fprintf()
#include <errno.h>		// for errno
#include <stdlib.h>		// for exit()
#include <string.h>		// for strerror()
#include <sys/ioctl.h>		// for ioctl()

#define TIOCL_SETKMSGREDIRECT	11 	// from <linux/tiocl.h>

int main( int argc, char **argv )
{	
	char	bytes[ 2 ] = { TIOCL_SETKMSGREDIRECT, 0 };	

	if ( argc == 2 ) bytes[1] = atoi( argv[1] );	// console id-number
	else	{
		fprintf( stderr, "%s: need a single argument\n", argv[0] );
		exit(1);
		}

	int	fd = open( "/dev/console", O_RDWR );		
	if ( fd < 0 ) { perror( "/dev/console" ); exit(1); }	
	
	if ( ioctl( fd, TIOCLINUX, bytes ) < 0 )	
		{	
		fprintf( stderr, "%s: ioctl( fd, TIOCLINUS, ...):", argv[0] );
		fprintf( stderr, " %s\n", strerror( errno ) );
		exit(1);
		}

	exit(0);
}
