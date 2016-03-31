//-------------------------------------------------------------------
//	sendto.cpp
//
//	This application will transmit a short test-message to the
//	computer whose node-name is specified on the command-line,
//	provided the 'ethers' file in the current directory has an
//	entry for that node and provided our device-driver 'nic.c'
//	for the Pro/1000 gigabit ethernet controller is installed. 
//
//		compile using:  $ g++ sendto.cpp -o sendto
//		execute using:  $ ./sendto <nodename>
//
//	programmer: ALLAN CRUSE
//	written on: 28 NOV 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror(), fopen(), fgets() 
#include <fcntl.h>	// for open() 
#include <stdlib.h>	// for strtol(), exit() 
#include <unistd.h>	// for write()  
#include <string.h>	// for strlen()
#include <sys/ioctl.h>	// for ioctl()

char filname[] = "ethers";
char devname[] = "/dev/nic";
char message[] = " CS635: Advanced Systems Programming \n"; 
unsigned char dstn[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int main( int argc, char **argv )
{
	if ( argc == 1 ) { fprintf( stderr, "nodename missing\n" ); exit(1); }
	
	// lookup the destination-node's hardware MAC-address	
	FILE	*fd = fopen( "ethers", "ro" );
	if ( !fd ) { perror( "ethers" ); exit(1); }
	char	line[ 80 ] = {0};
	while ( fgets( line, 80, fd ) )
		if ( strstr( line, argv[1] ) ) 
			{
			printf( "\nline from \'%s\': %s \n", filname, line );
			for (int i = 0; i < 6; i++)
				dstn[i] = strtol( line+i*3, NULL, 16 );
			break;
			}

	// open the device-file
	int	fp = open( devname, O_RDWR );
	if ( fp < 0 ) { perror( devname ); exit(1); }

	// set the device-driver packets' destination-address
	if ( ioctl( fp, 0, dstn ) ) { perror( "ioctl" ); exit(1); }

	// write our test-mesage to the device-file
	int	nbytes = write( fp, message, strlen( message ) );
	printf( "Wrote %d bytes to \'%s\' \n\n", nbytes, argv[1] );
}
