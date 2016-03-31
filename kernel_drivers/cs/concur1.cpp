//-------------------------------------------------------------------
//	concur1.cpp
//
//	This program demonstrates the need for synchronization among
//	cooperating threads in a multithreaded application.  Several
//	threads here will separately increment a shared counter, but
//	without any synchronization mechanisms in this initial demo. 
//
//		compile using:  $ g++ concur1.cpp -o concur1
//		execute using:  $ ./concur1
//
//	NOTES: Users are urged to read the online manual's entry for
//	the 'clone' library-function used here: its behavior differs
//	from the more familiar UNIX 'fork' function.  In particular,
//	a child-process created using 'clone()' does not 'return' to
//	its parent-process, but automatically is terminated when its 
//	thread-function reaches a 'return' statement.          
//
//	programmer: ALLAN CRUSE
//	written on: 05 OCT 2004
//	revised on: 14 SEP 2007 -- omits unneeded 'status' variable  
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <stdlib.h>	// for atoi(), malloc(), exit() 
#include <sched.h>	// for clone()
#include <sys/wait.h>	// for wait()

#define FLAGS 	( SIGCHLD | CLONE_VM )
#define STACKSIZE 4096
#define N_THREADS 4


int	maximum = 2500000;	// default number of iterations
int	counter = 0;		// the shared counter is global


int my_thread( void *dummy )
{
	int	i, temp;

	for (i = 0; i < maximum; i++) 
		{
		temp = counter;
		temp += 1;
		counter = temp;
		}

	return	0;
}






int main( int argc, char **argv )
{
	//--------------------------------------------
	// accept an argument that modifies 'maximum' 
	//--------------------------------------------  
	if ( argc > 1 ) maximum = atoi( argv[1] );
	if ( maximum < 0 ) { fprintf( stderr, "Bad argument\n" ); exit(1); } 
	printf( "%d threads will increment a shared counter %d times\n", 
						N_THREADS, maximum );
	
	//-----------------------------------------
	// allocate memory for our threads' stacks
	//-----------------------------------------
	char 	*thread_stacks = (char*)malloc( STACKSIZE * N_THREADS );
	if ( thread_stacks == NULL ) { perror( "malloc" ); exit(1); }

	//-------------------------------------------------
	// parent-process activates several worker-threads
	//-------------------------------------------------
	char	*tos = thread_stacks;
	for (int i = 0; i < N_THREADS; i++)
		{
		tos += STACKSIZE;
		clone( my_thread, tos, FLAGS, NULL );
		}
	//-----------------------------------------------
	// parent waits for all worker-threads to finish
	//-----------------------------------------------
	for (int i = 0; i < N_THREADS; i++) wait( NULL );

	//----------------------------------------------
	// release memory allocated for threads' stacks
	//---------------------------------------------- 
	free( thread_stacks );

	//--------------------------------------------
	// display the resulting value of the counter
	//--------------------------------------------
	printf( "result: counter=%d\n", counter );
}
