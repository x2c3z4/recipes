//-------------------------------------------------------------------
//	mapwatch.cpp
//
//	This program presents a dynamic display of current entries 
//	defined in the kernel's volatile 'pkmap_page_table' array, 
//	read from a pseudo-file named '/proc/pkmaps', and also the
//	value of the kernel's volatile 'jiffies_64' variable, read
//	from a second pseudo-file named '/proc/jiffies'.  The user
//	terminates this program by hitting the <ESCAPE>-key.  
//	
//		compile using:  $ g++ mapwatch.cpp -o mapwatch
//		execute using:  $ ./mapwatch
//
//	NOTE: You must first install our 'pkmaps.c' kernel module.
//
//	programmer: ALLAN CRUSE
//	written on: 01 NOV 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <fcntl.h>	// for open() 
#include <stdlib.h>	// for exit() 
#include <unistd.h>	// for read() 
#include <termios.h>	// for tcgetattr(), tcsetattr()

char title[] = 
"A dynamic visualization for the Linux kernel's 'permanent' kernel-mappings";
char procname[] = "/proc/pkmaps";
char timename[] = "/proc/jiffies";

int main( int argc, char **argv )
{
	// open the two pseudo-files for reading
	int	fd = open( procname, O_RDONLY );
	if ( fd < 0 ) { perror( procname ); exit(1); }
	int	fj = open( timename, O_RDONLY );
	if ( fj < 0 ) { perror( timename ); exit(1); }

	// preserve a copy of the keyboard's 'termios' structure
	struct termios	tc_orig;
	tcgetattr( STDIN_FILENO, &tc_orig );

	// setup 'non-canonical' keyboard input	
	struct termios	tc_work = tc_orig;
	tc_work.c_lflag &= ~(ECHO | ICANON | ISIG);
	tc_work.c_cc[ VMIN ] = 0;
	tc_work.c_cc[ VTIME ] = 0;
	tcsetattr( STDIN_FILENO, TCSAFLUSH, &tc_work );

	// initialize the screen with explanatory title 
	printf( "\e[H\e[J" );		// erase the screen
	printf( "\e[3;4H %s ", title );	// show title message
	fflush( stdout );		// draw it immediatey 

	// allocate storage for the 'pkmap_page_table[]' array
	unsigned int	pgtbl[ 1024 ];	// for one page-frame

	// main loop continuously reads and displays kernel data
	int	done = 0;
	while ( !done )
		{
		lseek( fd, 0, SEEK_SET );
		if ( read( fd, pgtbl, 4096 ) < 0 ) break;
		printf( "\e[4;4H" );	// move the cursor
		printf( "\e[?25l" );	// hide the cursor

		// loop through page-table entries in backward order
		for (int i = 0; i < 1024; i++)
			{
			int	k = 1023 - i;
			int	attr = (pgtbl[k] & 0x7) | '0';
			int	locn = 0xFF800000 | (k << 12);
			if ( attr == '0' ) attr = '-';
			if ( ( i % 64 ) == 0 ) printf( "\n    " );
			printf( "%c", attr );
			if ( ( i % 64 ) == 63 ) printf( "  %08X ", locn );
			}

		// read 'jiffies_64' from '/proc/jiffies' pseudo-file
		char	jiffies[ 24 ] = {0};
		lseek( fj, 0, SEEK_SET );
		read( fj, jiffies, 21 );
		printf( "\e[22;1H" );	// move the cursor
		printf( "    %28s=%s ", "jiffies", jiffies );
		fflush( stdout );	// show value immediately

		// check for user's keypress
		int	inch = 0;
		if ( read( STDIN_FILENO, &inch, 4 ) < 0 ) break;
		if ( inch == 0x1B ) done = 1;
		}

	// restore the normal 'canonical' keyboard input
	tcsetattr( STDIN_FILENO, TCSAFLUSH, &tc_orig );
	printf( "\e[?25h" ); 	// show the cursor
	printf( "\e[23;1H\n" );	// move the cursor
}
