//-------------------------------------------------------------------
//	syswrite.cpp
//
//	This program shows how you can issue a system-call directly
//	to the Linux kernel (rather than calling a standard library
//	function), by using 'inline assembly language' statements.
//
//		compile using:  $ g++ syswrite.cpp -o syswrite
//		execute using:  $ ./syswrite
//
//	programmer: ALLAN CRUSE
//	written on: 09 MAY 2005
//	revised on: 01 DEC 2007 -- for Linux kernel version 2.6.22.5
//-------------------------------------------------------------------

#include <string.h>		// for strlen()
#include <unistd.h>		// for STDOUT_FILENO  
#include <asm/unistd.h>		// for __NR_write


int my_write( int fd, void *buf, int len )
{
	int	retval;

	asm(" movl %0, %%eax " : : "i" (__NR_write) );
	asm(" movl %0, %%ebx " : : "m" (fd) );
	asm(" movl %0, %%ecx " : : "m" (buf) );
	asm(" movl %0, %%edx " : : "m" (len) );
	asm(" int $0x80 " );
	asm(" movl %%eax, %0 " : "=m" (retval) );

	return	retval;
}


int main( void )
{
	char	message[] = "\nHello, world!\n\n";
	int	msgsize = strlen( message );

	my_write( STDOUT_FILENO, message, msgsize );
}


