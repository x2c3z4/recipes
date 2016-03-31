//-------------------------------------------------------------------
//	loopback.cpp
//
//	This example demonstrates use of the UART's 'loopback' mode,
//	but it requires execution of our 'iopl3' command beforehand.
//
//		compile using:  $ g++ loopback.cpp -o loopback
//		execute using:  $ ./loopback
//
//	NOTE: Written and tested using i386 kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 08 OCT 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <stdlib.h>	// for exit() 
#include <unistd.h>	// for fork() 
#include <wait.h>	// for wait()
#include <sys/io.h>	// for outb(), inb()

#define UART	0x03F8	// base i/o-port for UART

int main( int argc, char **argv )
{
	// insure that IOPL will allow 'inb()' and 'outb()
	if ( iopl( 3 ) ) { perror( "iopl" ); return -1; }

	// disable all of the UART's interrupts 
	outb( 0x00, UART+1 );	// INTR_ENABLE

	// reset the UART's FIFOs and activate its FIFO mode
	outb( 0x00, UART+2 );	// FIFO_CONTROL
	outb( 0xC7, UART+2 );	// FIFO_CONTROL

	// select maximum baudrate and 8-N-1 data-format
	outb( 0x80, UART+3 );	// LINE_CONTROL
	outw( 0x0001, UART );	// DIVISOR_LATCH
	outb( 0x03, UART+3 );	// LINE_CONTROL

	// now initialize the UART for 'loopback' mode
	outb( 0x10, UART+4 ); 	// MODEM_CONTROL

	// display the readable control and status registers
	printf( "\n" );
	printf( "  MODEM_CONTROL=%02X \n", inb( UART+4 ) );
	printf( "   MODEM_STATUS=%02X \n", inb( UART+6 ) );
	printf( "   LINE_CONTROL=%02X \n", inb( UART+3 ) );
	printf( "    LINE_STATUS=%02X \n", inb( UART+5 ) );
	printf( "    INTR_ENABLE=%02X \n", inb( UART+1 ) );
	printf( "  INTR_IDENTIFY=%02X \n", inb( UART+2 ) );		
	printf( "\n" );	

	// prompt the user to type in a short message
	printf( "\nPlease type a short message \n" );

	// OK, now we execute a pair of tasks concurrently
	if ( fork() )
		{
		// parent-process will 'transmit' user's keystrokes  
		char	input[ 80 ] = {0};
		int	n = read( STDIN_FILENO, input, 80 );
		for (int i = 0; i < n; i++)
			{
			while ( ( inb( UART+5)&0x20 ) == 0 );
			outb( input[i], UART );
			}
		wait( NULL );
		printf( "parent is quitting\n" );
		}
	else	{
		// child-process prints each character it receives
		int	datum = 0;
		do	{
			while ( ( inb( UART+5 )&1 ) == 0 );		
			datum = inb( UART );
			printf( "%c", datum ); fflush( stdout );
			}
		while ( datum != '\n' );
		printf( "\nchild is quitting\n" );
		exit(1);
		}
}
