//-------------------------------------------------------------------
//	ioremap.c
//
//	This module illustrates use of the kernel's 'ioremap()' and 
//	'iounmap()' functions.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.5
//
//	programmer: ALLAN CRUSE
//	written on: 27 SEP 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/proc_fs.h>	// for create_proc_info_entry() 
#include <asm/io.h>		// for ioremap(), iounmap()

#define LOCAL_APIC_BASE	0xFEE00000

char modname[] = "ioremap";
void *lapic;
int _cr3;

int my_get_info( char *buf, char **start, off_t off, int count )
{
	int	i, pgdir, pgtbl, cpu, len;

	cpu = ioread32( lapic + 0x20 ) >> 24;

	asm(" push %eax \n mov %cr3, %eax \n mov %eax, _cr3 \n popl %eax ");

	len = 0;
	len += sprintf( buf+len, "\n LOCAL APIC registers " );
	len += sprintf( buf+len, "for processor %d \n", cpu );

	for (i = 0; i < 0x400; i+=16)
		{
		int	val = ioread32( lapic + i );
		if ( ( i % 128 ) == 0 ) 
			len += sprintf( buf+len, "\n %04X:", i );
		len += sprintf( buf+len, " %08X", val );
		}
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "\n" );

	pgdir = ((int)lapic >> 22)&0x3FF;
	pgtbl = ((int)lapic >> 12)&0x3FF;

	len += sprintf( buf+len, " APIC virtual-address = %08X ", (int)lapic );
	len += sprintf( buf+len, " CR3=%08X ", _cr3 );
	len += sprintf( buf+len, " pgdir+0x%03X ", pgdir << 2 );
	len += sprintf( buf+len, " pgtbl+0x%03X ", pgtbl << 2 );
	len += sprintf( buf+len, "\n" );
	len += sprintf( buf+len, "\n" );

	return	len;
}


static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	lapic = ioremap( LOCAL_APIC_BASE, PAGE_SIZE );
	if ( !lapic ) return -ENODEV;

	printk( " LOCAL APIC at physical address %08X", LOCAL_APIC_BASE );
	printk( " remapped to virtual address %08X \n", (int)lapic );
	
	create_proc_info_entry( modname, 0, NULL, my_get_info );
	return	0;  //SUCCESS
}


static void __exit my_exit(void )
{
	iounmap( lapic );

	remove_proc_entry( modname, NULL );

	printk( "<1>Removing \'%s\' module\n", modname );
}

module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL"); 

