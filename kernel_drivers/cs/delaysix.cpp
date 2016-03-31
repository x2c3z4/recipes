//-------------------------------------------------------------------
//	delaysix.cpp
//
//	This program uses our 'cmosram.c' device-driver to access 
//	the Real-Time Clock, in order to create a 6-second delay.
//
//	     compile using:  $ g++ delaysix.cpp -o delaysix
//	     execute using:  $ ./delaysix
//
//	NOTE: This program requires our 'cmosram.ko' driver to be
//	installed in the kernel (and assumes '/dev/cmos' exists).   
//
//	programmer: ALLAN CRUSE 
//	written on: 02 OCT 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <unistd.h>	// for read() 

int status = 0;		// for most recent value of RTC's Status_A

int main( int argc, char **argv )
{
	// open the 'dev/cmos' device-file for reading
	int	fd = open( "/dev/cmos", O_RDONLY );
	if ( fd < 0 ) { perror( "/dev/cmos" ); return -1; }


	// this loop repeatedly reads Status_Register_A, until 
	// its 'UpdateInProgress' bit has 'flipped' six times
	for (int i = 0; i < 6; i++)
		{
		// do busy-wait until UpdateInProgress is 'true'	
		do	{
			lseek( fd, 10, SEEK_SET );
			read( fd, &status, 1 );
			}
		while ( ( status & 0x80 ) == 0x00 );

		// do busy-wait until UpdateInProgress is 'false'
		do	{
			lseek( fd, 10, SEEK_SET );
			read( fd, &status, 1 );
			}
		while ( ( status & 0x80 ) == 0x80 );

		printf( " %d \n", i+1 );
		}

	// print closing message and exit
	printf( "\nOK, six seconds have elapsed\n" );
}
