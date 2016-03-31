//-------------------------------------------------------------------
//	devtfix.cpp
//
//	This program invokes the Linux 'makedev()' macro, in order 
//	to determine the value that should be passed to the kernel
//	from user-mode when the 'mknod()' system-call is employed.
//	
//		dev_t	dev = makedev( major, minor );
//
//	For our purposes we want to create a device-file which has 
//	major-number 70 and minor-number 0; (note that 70 = 0x46).
//
//	programmer: ALLAN CRUSE
//	written on: 03 SEP 2007
//-------------------------------------------------------------------

#include <fcntl.h>	// for makedev() 
#include <stdio.h>	// for printf()  

int main( int argc, char **argv )
{
	int	major = 70, minor = 0;

	dev_t	dev = makedev( major, minor );

	printf( " dev_t dev = makedev( major, minor ) \n" );
	printf( " sizeof( dev_t ) = %ld bytes \n", sizeof( dev_t ) );
	printf( " major=%d  minor=%d ", major, minor );
	printf( " dev = %lX \n", (long)dev );
}
