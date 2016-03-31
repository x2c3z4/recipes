//----------------------------------------------------------------
//	newmod.cpp	
//
//	This utility creates the skeleton code for an installable 
//	Linux kernel module (LKM) having the module-name supplied
//	by the user as a command-line argument.     
//
//		compile using:	$ g++ newmod.cpp -o newmod 	
//		execute using:  $ ./newmod <modname>
//
//	NOTE: The resulting C source file may then be compiled in
//	one step using our 'mmake.cpp' utility-program.  
//	
//	programmer: ALLAN CRUSE
//	written on: 20 FEB 2002
//	revised on: 12 MAY 2002 -- to incorporate MODULE_LICENSE
//	revised on: 20 JAN 2005 -- for Linux kernel version 2.6.
//	revised on: 30 MAY 2007 -- to include the above comments
//	revised on: 17 JUN 2007 -- to use new-style init/cleanup
//----------------------------------------------------------------

#include <stdio.h>	// for fprintf(), fopen(), etc
#include <string.h>	// for strncpy(), strncat()
#include <time.h>	// for time(), localtime()

char authorname[] = "<YOUR NAME>";
char monthlist[] = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";

int main( int argc, char *argv[] )
{
	// check for module-name as command-line argument
	if ( argc == 1 ) 
		{
		fprintf( stderr, "Must specify module-name\n" );
		return	-1;
		}

	// prepare module name (without any suffic) 
	char	modname[33] = "";
	strncpy( modname, argv[1], 28 );
	strtok( modname, "." );

	// prepare code-file name (with ".c" suffix)
	char	srcname[33] = "";
	strncpy( srcname, modname, 28 );
	strncat( srcname, ".c", 2 );

	// announce this program's purpose 
	printf( "\nCreating skeleton for module " );
	printf( "named \'%s\' \n", srcname );

	// insure source-file doesn't already exist
	FILE	*fp = fopen( srcname, "rb" );
	if ( fp != NULL )
		{
		fclose( fp );
		fprintf( stderr, "File \'%s\' already exists\n", srcname );
		return	-1;
		}

	// create the new source-file
	fp = fopen( srcname, "wb" );
	if ( fp == NULL )
		{
		fprintf( stderr, "Cannot create source-file\n" );
		return	-1;
		}

	// obtain today's date (in DD MMM YYYY format)
	time_t		now = time( (time_t *)NULL );
	struct tm	*t = localtime( &now );
	char	month[4] = "";
	strncpy( month, monthlist+3*t->tm_mon, 3 );
	month[3] = '\0';

	char	when[16] = "";
	sprintf( when, "%02d %3s %04d", t->tm_mday, month, 1900+t->tm_year );	

	char	border[68] = "";
	memset( border, '-', 67 );

	fprintf( fp, "//%s\n", border );
	fprintf( fp, "//\t%s\n", srcname );
	fprintf( fp, "//\n" );
	fprintf( fp, "//\tprogrammer: %s\n", authorname );
	fprintf( fp, "//\tdate begun: %s\n", when );
	fprintf( fp, "//%s\n", border );

	fprintf( fp, "\n#include <linux/module.h>" );
	fprintf( fp, "\t// for init_module() " );
	fprintf( fp, "\n" );
	fprintf( fp, "\nchar modname[] = \"%s\";\n", modname );

	fprintf( fp, "\n" );
	fprintf( fp, "\nstatic int __init my_init( void )\n" );
	fprintf( fp, "{\n" );
	fprintf( fp, "\tprintk( \"<1>\\nInstalling \\\'%%s\\\' " );
	fprintf( fp, "module\\n\", modname );\n" );
	fprintf( fp, "\n\treturn\t0;  // SUCCESS\n" );
	fprintf( fp, "}\n" );
	
	fprintf( fp, "\n" );
	fprintf( fp, "\nstatic void __exit my_exit( void )\n" );
	fprintf( fp, "{\n" );
	fprintf( fp, "\tprintk( \"<1>Removing \\\'%%s\\\' " );
	fprintf( fp, "module\\n\", modname );\n" );
	fprintf( fp, "}\n" );

	fprintf( fp, "\n" );
	fprintf( fp, "module_init( my_init );\n" );
	fprintf( fp, "module_exit( my_exit );\n" );
	fprintf( fp, "MODULE_LICENSE(\"GPL\");\n" );
	fprintf( fp, "\n" );

	fclose( fp );
	printf( "\n" );
}
