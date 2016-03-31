//-------------------------------------------------------------------
//	yielding.cpp
//
//	This program investigates the ability of the 'nanosleep()'
//	system-call to allow a Linux task to voluntarily yield its  
//	control of the processor to another task, as might well be
//	preferable to busy-waiting in a multithreaded application. 
//	NOTE: This use of 'nanosleep()' was suggested in an online 
//	article about thread-scheduling by Dr. Edward G. Bradford.
//
//                     HOW TO USE THIS DEMO PROGRAM                           
//
//	When executed without command-line arguments, each process
//	prints multiple messages, but yields the cpu after each is
//	displayed, allowing other tasks to be scheduled.  However,
//	if an argument is supplied, each process displays its full 
//	sequence of messages before voluntarily giving up control.    
//
//	programmer: ALLAN CRUSE
//	written on: 05 OCT 2004
//	revised on: 14 SEP 2007 -- omit unneeded 'status' variable 
//-------------------------------------------------------------------

#include <stdio.h>	// for printf() 
#include <stdlib.h>	// for exit() 
#include <unistd.h>	// for fork() 
#include <sys/wait.h>	// for wait()
#include <time.h>	// for nanosleep()

#define N_TASKS	4

int main( int argc, char **argv )
{
	// create a 1-nanosecond timespec structure
	struct timespec ts = { 0, 1 };

	// parent forks a series of child-processes
	int	taskid = 0;
	for (int i = 0; i < N_TASKS; i++)
		{
		++taskid;
		if ( fork() ) continue;
		for (int j = 0; j < 4; j++)
			{	
			printf( "task #%d \n", taskid );
			if ( argc == 1 ) nanosleep( &ts, NULL );
			}
		exit(0);
		}

	// parent waits until each child has terminated
	for (int i = 0; i < N_TASKS; i++) wait( NULL ); 
	printf( "finished\n" );	
}

