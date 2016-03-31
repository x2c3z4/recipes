//-------------------------------------------------------------------
//	cmpxchg.cpp
//
//	This program investigates the setting of the ZF-bit by the
//	'cmpxchg' instruction, to see whether it would be feasible
//	to omit a subsequent 'test' instruction that appeared in a
//	function in the Linux kernel (version 2.6.22.5).   
//
//	Intel's pseudocode:
//		
//			cmpxchg	  source, destination
//
//		if ( accumulator == destination )
//			{ ZF <-- 1; destination <-- source; }
//		else	{ ZF <-- 0; accumulator <-- destination; }	
//
//	programmer: ALLAN CRUSE
//	written on: 22 SEP 2007
//-------------------------------------------------------------------

#include <stdio.h>	// for printf(), perror() 

int	cmos_lock, _eax, _ebx, _ecx, _edx = 0x12345678;

int perform_cmpxchg_test( int lock_value )
{
	cmos_lock = lock_value;
	_eax = _ebx = _ecx = 0;

	printf( "\n" );
	printf( " cmos_lock=%08X ", cmos_lock );
	printf( " EAX=%08X  EBX=%08X ", _eax, _ebx );
	printf( " ECX=%08X  EDX=%08X ", _ecx, _edx );
	printf( "\n" );
	asm(	"	mov	_edx, %edx		\n"\
		"	mov	cmos_lock, %eax		\n"\
		"	cmpxchg	%edx, cmos_lock		\n"\
		"	setz	_ebx			\n"\
		"	test	%eax, %eax		\n"\
		"	setz	_ecx			\n"\
		"	mov	%eax, _eax		\n");		
	printf( " cmos_lock=%08X ", cmos_lock );
	printf( " EAX=%08X  EBX=%08X ", _eax, _ebx );
	printf( " ECX=%08X  EDX=%08X ", _ecx, _edx );
	printf( "\n" );
	return	( _ebx == _ecx );
}

int main( int argc, char **argv )
{
	if ( perform_cmpxchg_test( 0 ) && perform_cmpxchg_test( ~0 ) )
		printf( "\nThe \'test\' instruction was unneeded \n\n" );
	else	printf( "\nThe \'test\' instruction is necessary \n\n" );
} 
