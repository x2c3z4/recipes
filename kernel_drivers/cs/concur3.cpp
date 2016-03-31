//-------------------------------------------------------------------
//	concur3.cpp
//
//	This program revises our 'concur2.cpp' demo to eliminate the
//	excessive 'busy-waiting' by invoking the Linux 'nanosleep()'
//	system-call in order to 'yield' the processor when the mutex
//	cannot immediately be acquired.  
//
//	programmer: ALLAN CRUSE
//	written on: 06 OCT 2004
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 
#include <stdlib.h>	// for atoi(), malloc(), exit() 
#include <sched.h>	// for clone()
#include <sys/wait.h>	// for wait()
#include <time.h>	// for nanosleep()

#define FLAGS 	( SIGCHLD | CLONE_VM )
#define STACKSIZE 4096
#define N_THREADS 4

int	mutex = 1;
struct timespec	ts = { 0, 1 };

void inline enter_critical( void )
{
	asm("spin:  lock  btr $0, mutex ");
	asm("       jc    pass          ");
	nanosleep( &ts, NULL );		// <-- system-call inserted here
	asm("       jmp   spin          ");
	asm("pass:			");
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
