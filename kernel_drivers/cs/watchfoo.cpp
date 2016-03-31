//-------------------------------------------------------------------
//	watchfoo.cpp
//
//	This program continuously rereads a device-file ('/dev/foo')
//	in order to show the current value of a kernel variable that
//	increments rapidly (i.e., the 'jiffies_64' interrupt-count).
//
//	NOTE: Our 'foo.c' device-driver is needed with this program. 
//  
//	We observe two problems with this demo-program: it consumes 
//	an excessive amount of CPU time, and it can't be terminated
//	gracefully (i.e., without delivery of some external signal).
//
//	programmer: ALLAN CRUSE
//	written on: 20 MAR 2005
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <stdlib.h>	// for exit() 
#include <unistd.h>	// for read(), 

int main( int argc, char **argv )
{
	// open the '/dev/foo' device-file for reading
	int	fd = open( "/dev/foo", O_RDONLY );
	if ( fd < 0 ) { perror( "/dev/foo" ); exit(1); }

	// main program-loop
	while ( 1 )  // this loop could continue forever! 
		{
		// declare and initialize a 64-bit variable
		unsigned long long	jiffies = 0LL;
		// read current contents of the device-file
		int 	nbytes = read( fd, &jiffies, 8 );
		if ( nbytes < 0 ) break;
		// print the current device-file contents
		printf( "\r  jiffies=%llu  ", jiffies );
		fflush( stdout );
		}
}
