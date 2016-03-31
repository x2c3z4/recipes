//-------------------------------------------------------------------
//	rotation.cpp
//
//	This program uses some services which are implemented by our
//	'vmram.c' device-driver and which provide user-mode programs
//	with access to the ATI graphics controller's display memory.
//	Specifically, the 'read()', 'write()', and 'lseek()' methods
//	are employed to perform a 'rotation' of the (red,green,blue) 
//	color-components in every displayed pixel, assuming that the
//	program executes from within a 'truecolor' graphical window. 
//
//		compile using:  $ g++ rotation.cpp -o rotation
//		execute using:  $ ./rotation
//
//	NOTE: This program will require our 'vram.c' device-driver.
//
//	programmer: ALLAN CRUSE
//	written on: 16 OCT 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <stdlib.h>	// for exit() 
#include <unistd.h>	// for read(), write(), close() 
#include <sys/mman.h>	// for mmap() 

char devname[] = "/dev/vram";

int main( int argc, char **argv )
{
	// open the device-file for reading and writing
	int	fd = open( devname, O_RDWR );
	if ( fd < 0 ) { perror( devname ); exit(1); }

	// allocate a page-size storage-buffer
	unsigned int	*heap = (unsigned int*)malloc( 0x1000 );
	if ( !heap ) { perror( "malloc" ); exit(1); }
	
	// perform the color-rotations on successive pages	
	for (int p = 0; p < 4096; p++)
		{
		if ( read( fd, heap, 0x1000 ) < 0 ) break;
	
		for (int i = 0; i < 1024; i++) 
			{
			unsigned int	pel = heap[i];
			pel = (( pel >> 8 )&0x00FFFF)|(( pel << 16 )&0xFF0000);
			heap[i] = pel;
			}

		lseek( fd, -0x1000, SEEK_CUR );
		if ( write( fd, heap, 0x1000 ) < 0 ) break;
		}

	// release the allocated memory-page
	free( heap );
}

