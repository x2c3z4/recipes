//-------------------------------------------------------------------
//	try17.cpp
//
//	This program makes a direct call to the Linux kernel with a
//	request to execute the obsolete system-call number 17.  The
//	kernel no longer implements that particular kernel service,
//	so we will see no changes to our function's argument-value.
//
//	We will use this program to demonstrate that we can replace
//	that unimplemented system-call with our own kernel routine. 
//
//		compile using:  $ g++ try17.cpp -o try17
//		execute using:  $ ./try17
//
//	programmer: ALLAN CRUSE
//	written on: 24 JUN 2004
//-------------------------------------------------------------------

#include <stdio.h>	// for printf() 


int exec_syscall_17( int *num )
{
	int	retval = -1;
	
	asm(" 	movl	$17, %eax	");
	asm("	movl	%0, %%ebx " : : "m" (num) );
	asm("	int	$0x80		");
	asm("	movl	%%eax, %0 " : "=m" (retval) );
	
	return	retval;
}	
	

int main( int argc, char **argv )
{
	int	number = 15;

	printf( "\nDemonstrating system-call 17 ... \n" );

	for (int i = 0; i <= 5; i++)
		{	
		printf( "\n#%d:  number=%d ", i, number );
		exec_syscall_17( &number );
		}
	printf( "\n\n" );
}

