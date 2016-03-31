//-------------------------------------------------------------------
//	setbaud.cpp
//
//	This unprivileged utility-program provides users with a way
//	to modify the serial-UART's current baudrate by supplying a
//	command-line argument.  (The value of that argument will be
//	relayed to our device-driver via an 'ioctl()' system-call.)
//	If no argument is given, the existing baudrate is reported.         
//
//		compile using:  $ g++ setbaud.cpp -o setbaud
//		execute using:  $ ./setbaud <arg>
//
//	NOTE: Our 'baudrate.c' device-driver needs to be installed.
//
//	programmer: ALLAN CRUSE
//	written on: 18 OCT 2007
//	revised on: 21 OCT 2007 -- so case 1 falls through to case 0
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <stdlib.h>	// for atoi(), exit()   
#include <sys/ioctl.h>	// for ioctl()

#define GET_BAUDRATE	0
#define SET_BAUDRATE	1

char devname[] = "/dev/uart";

int main( int argc, char **argv )
{
	// open the device-file
	int	fd = open( devname, O_RDWR );
	if ( fd < 0 ) { perror( devname ); exit(1); }

	// perform an appropriate 'ioctl()' system-call
	int	baudrate = 0;
	switch ( argc )
		{
		case 2:	// one command-line argument was supplied
			baudrate = atoi( argv[1] );
			if ( ioctl( fd, SET_BAUDRATE, &baudrate ) < 0 )
				{ perror( "SET_BAUDRATE" ); exit(1); }

		case 1: // no command-line arguments were supplied 
			if ( ioctl( fd, GET_BAUDRATE, &baudrate ) < 0 )
				{ perror( "GET_BAUDRATE" ); exit(1); }
			break;

		default: // more than one argument was supplied
			fprintf( stderr, "Too many arguments\n" ); exit(1);
		}
	
	// report the current baudrate
	printf( " \n baudrate=%d \n", baudrate );
	printf( "\n" );
}

