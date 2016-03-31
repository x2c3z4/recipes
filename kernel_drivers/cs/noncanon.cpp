//----------------------------------------------------------------
//	noncanon.cpp
//
//	Program shows how to enable non-canonical console-input.
//	
//	Reference: W. Richard Stevens, "Advanced Programming in
//	the UNIX Environment," Addison-Wesley (1993), page 354.
//
//	programmer: ALLAN CRUSE
//	written on: 20 FEB 2002
//	revised on: 21 APR 2005
//---------------------------------------------------------------

#include <stdio.h>	// for printf(), perror()
#include <stdlib.h>	// for exit()
#include <unistd.h>	// for STDIN_FILENO
#include <termios.h>	// for tcgetattr(), tcsetattr()

#define ESCAPE	0x1B

int main( void )
{
	printf( "\nPlacing console in non-canonical mode\n" );

	struct termios	save_termios;
	if ( tcgetattr( STDIN_FILENO, &save_termios ) < 0 )
		{ perror( "tcgetattr" ); exit(1); }

	struct termios	work_termios = save_termios;
	work_termios.c_lflag &= ~( ECHO | ICANON );
	work_termios.c_cc[ VMIN ] = 1;	// one character at a time
	work_termios.c_cc[ VTIME ] = 0;	// with no console timeout
	if ( tcsetattr( STDIN_FILENO, TCSAFLUSH, &work_termios ) < 0 )
		{ perror( "tcsetattr" ); exit(1); }

	int	done = 0;
	while ( !done )
		{
		int	inch = 0;
		read( STDIN_FILENO, &inch, sizeof( inch ) );
		if ( inch == ESCAPE ) done = 1;
		printf( "%08X ", inch );
		if (( inch < 0x20 )||( inch > 0x7E )) inch = '.';
		printf( "%c\n", inch );
		}

	if ( tcsetattr( STDIN_FILENO, TCSAFLUSH, &save_termios ) < 0 )
		{ perror( "setattr" ); exit(1); }

	printf( "Console is restored to canonical mode\n\n" );
}
