//----------------------------------------------------------------
//	iopl3.cpp	 (Lets user programs perform direct I/O)
//
//	This utility for Linux systems allows a user to launch a
//	new command shell which possesses system privileges that
//	each subsequenty executed application will then inherit,
//	and thereafter be able execute the 'iopl()' system-call.
//
//	NOTE: This utility already was compiled and installed on
//	our classroom and laboratory workstations.  So this code 
//	is only needed if you want to understand how the 'iopl3' 
//	commpand works, or if you want to install it yourself on
//	your computer at home, where you have 'root' privileges:
//
//	  compile using:  root# make iopl3
//	     		  root# chmod a+s iopl3  
//	  install using:  root# mv iopl3 /usr/local/bin/.
//
//	  execute using:  user$ iopl3
//
//	programmer: Alex Fedosov
//	commentary: Allan Cruse
//	completion: 26 AUG 2005
//	revised on: 13 OCT 2007 -- added new note to commentary
//----------------------------------------------------------------

#include <stdio.h>	// for printf()
#include <unistd.h>	// for getuid(), setuid()
#include <sys/io.h>	// for iopl()
#include <stdlib.h>	// for system()

int main(int argc, char *argv[])
{
	// preserve this task's user-identity
        int 	uid = getuid();
        printf( "%d\n", getuid() );

	// change this task's effective user-identity to 'root'
        setuid( 0 );
        printf( "%d\n", getuid() );

	// now 'root' can change this task's I/O Privilege-Level: IOPL=3
        iopl( 3 );

	// restore this task's former user-identity
        setuid( uid );
        printf( "%d\n", getuid()) ;

	// launch a new command-shell which will inherit IOPL=3
        system( "bash --login" );
        return 0;
}

