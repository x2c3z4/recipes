//----------------------------------------------------------------
//	dump.cpp
//
//	Shows a file's contents in hexadecimal and ascii formats.
//
//	        compile using:  $ g++ dump.cpp -o dump
//		execute using:  $ ./dump <filename>
//
//	programmer: ALLAN CRUSE
//	written on: 03 SEP 2007 
//----------------------------------------------------------------

#include <stdio.h>	// for printf(), fprintf(), perror()
#include <fcntl.h>	// for open(), O_RDONLY
#include <unistd.h>	// for lseek(), read(), close()
#include <stdlib.h>	// for exit()

int main( int argc, char *argv[] )
{
	// check for mandatory command-line argument
	if ( argc == 1 ) 
		{
		fprintf( stderr, "You must specify an input-file\n" );
		exit(1);
		}
	
	// open the specified input-file and obtain its size (in bytes)
	int	fd = open( argv[1], O_RDONLY );
	if ( fd < 0 ) { perror( argv[1] ); exit(1); }
	int	filesize = lseek( fd, 0, SEEK_END );
	
	// display title for the screen output (file's name and size)
	printf( "\nFILE DUMP: %s (%d bytes)\n", argv[1], filesize );
	
	// main loop to display file contents in hex and ascii formats
	lseek( fd, 0, SEEK_SET );
	for (int i = 0; i < filesize; i += 16)
		{
		printf( "\n  %08X: ", i );
		unsigned char	buf[ 16 ];
		for (int j = 0; j < 16; j++) 
			if ( read( fd, buf+j, 1 ) < 0 ) break;
		for (int j = 0; j < 16; j++) 
			{
			if ( i+j < filesize ) printf( "%02X ", buf[j] );
			else	printf( "   " );
			}
		printf( " " );
		for (int j = 0; j < 16; j++)
			{
			unsigned char	ch = (i+j < filesize) ? buf[j] : ' ';
			if (( ch < 0x20 )||( ch > 0x7E )) ch = '.';	
			printf( "%c", ch );
			}	
		}
	close( fd );
	printf( "\n\n" );
}
