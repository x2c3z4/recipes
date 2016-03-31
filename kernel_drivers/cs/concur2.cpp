//-------------------------------------------------------------------
//	concur2.cpp
//
//	This program revises our 'concur1.cpp' demo, incorporating a
//	spinlock that implements 'mutual exclusion' so that only one 
//	thread at a time accesses the shared 'counter' variable, but
//	at the cost of introducing considerable 'busy-waiting' (plus
//	bus-contention if executed on a multiprocessor platform).
//
//	programmer: ALLAN CRUSE
//	written on: 06 OCT 2004
//	revised on: 14 SEP 2007 -- omits unneeded 'status' variable
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <stdlib.h>	// for atoi(), malloc(), exit() 
#include <sched.h>	// for clone()
#include <sys/wait.h>	// for wait()

#define FLAGS 	( SIGCHLD | CLONE_VM )
#define STACKSIZE 4096
#define N_THREADS 4

int	mutex = 1;

void inline enter_critical( void )
{
	asm("spin:  lock  btr $0, mutex ");
	asm("       jnc   spin          ");
}

void inline leave_critical( void )
{
	asm(" bts $0, mutex ");
}


int	maximum = 2500000;	// default number of iterations
int	counter = 0;		// the shared counter is global


int my_thread( void *dummy )
{
	int	i, temp;

	for (i = 0; i < maximum; i++) 
		{
		enter_critical();
		temp = counter;
		temp += 1;
		counter = temp;
		leave_critical();
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
