//-------------------------------------------------------------------
//	elfcheck.cpp
//
//	This application reads in the first four bytes from a file 
//	whose name was entered as a command-line argument, then it
//	reports whether or not the ELF file-signature was present. 
//	(It illustrates the role in application programs played by 
//	standard library functions such as 'open()' and 'read()'.) 
//
//	      compile using:  $ g++ elfcheck.cpp -o elfcheck
//	      execute using:  $ ./elfcheck <filename>
//
//	programmer: ALLAN CRUSE
//	written on: 04 SEP 2007
//-------------------------------------------------------------------

#include <fcntl.h>	// for open() 
#include <stdio.h>	// for printf(), perror() 
#include <unistd.h>	// for read(), close() 
#include <string.h>	// for strncmp()

char buf[4];

int main( int argc, char **argv )
{
	if ( argc == 1 ) return -1;

	int	fd = open( argv[1], O_RDONLY );
	if ( fd < 0 ) { perror( argv[1] ); return -1; }

	int nbytes = read( fd, buf, 4 );
	if ( nbytes < 0 ) { perror( argv[1] ); return -1; }

	if ( strncmp( buf, "\177ELF", 4 ) == 0 )
		printf( "File \'%s\' has an ELF signature \n", argv[1] );
	else	printf( "File \'%s\' is not an ELF file \n", argv[1] );
	close( fd );
}
