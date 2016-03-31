//-------------------------------------------------------------------
//	inherit.cpp
//
//	This program investigates whether a mapping of video-memory
//	that a parent-process creates (by using 'mmap()') will also 
//	be inherited by a child-process which is created afterward.
//
//		compile using:  $ inherit.cpp -o inherit
//		execute using:  $ ./inherit
//
//	NOTE: This program will require our 'vram.c' device-driver.
//
//	programmer: ALLAN CRUSE
//	written on: 15 OCT 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <stdlib.h>	// for exit() 
#include <unistd.h>	// for lseek(), lseek()  
#include <sys/mman.h>	// for mmap() 
#include <sys/wait.h>	// for wait()

#define VRAM_BASE	0xA0000000

char devname[] = "/dev/vram";
unsigned int *vram = (unsigned int *)VRAM_BASE;

// These constants will depend on your system's screen-resolution
const int  hres = 1280, vres = 1024, pitch = 1280;

int main( int argc, char **argv )
{
	// open the device-file for reading and writing
	int	fd = open( devname, O_RDWR );
	if ( fd < 0 ) { perror( devname ); exit(1); }

	// map the video frame-buffer into user-space
	int	size = lseek( fd, 0, SEEK_END );
	int	prot = PROT_READ | PROT_WRITE;
	int	flag = MAP_FIXED | MAP_SHARED;
	if ( mmap( (void*)vram, size, prot, flag, fd, 0 ) == MAP_FAILED )
		{ perror( "mmap" ); exit(1); }

	// child-process draws a rectangular border around the screen
	if ( fork() ) wait( NULL ); // parent waits until child exits
	else for (int indent = 0; indent < 50; indent++)
		{
		int	cmin = indent, cmax = hres - indent -1;
		int	rmin = indent, rmax = vres - indent -1;
		int	r = rmin, c = cmin, color = 0x007F007F;
		do { vram[ r*pitch + c ] = color; ++c; } while ( c < cmax );	
		do { vram[ r*pitch + c ] = color; ++r; } while ( r < rmax );
		do { vram[ r*pitch + c ] = color; --c; } while ( c > cmin );	
		do { vram[ r*pitch + c ] = color; --r; } while ( r > rmin );
		}		
}

