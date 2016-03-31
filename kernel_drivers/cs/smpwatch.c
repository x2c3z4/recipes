//-------------------------------------------------------------------
//	smpwatch.c
//
//	This module creates a pseudo-file named '/proc/smpwatch'
//	that is both readable and writable by user applications.
//	The file consists of an array of counters (corresponding
//	to the 256 possible interrupts on each of two processors
//	(on a dual-CPU platform), each counter being incremented 
//	each time its associated interrupt-handler gets invoked. 
//	The entire array gets reinitialized by any attempt to do
//	either 'open' or 'write' operations on this pseudo-file.
//
//	NOTE1: Developed and tested using kernel version 2.4.18.
//
//	NOTE2: This module implicitly assumes that Linux has put
//	the same value in the IDTR register for both processors. 
//
//	NOTE3: Our 'page-fault' ISR should not use register EBX.
//
//	programmer: ALLAN CRUSE
//	written on: 23 MAR 2003
//	revised on: 15 SEP 2007 -- for Linux kernel version 2.6.22.5
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module()
#include <linux/proc_fs.h>	// for create_proc_entry() 
#include <asm/uaccess.h>	// for copy_to_user()
#include <asm/io.h>		// for ioremap(), iounmap()

#define LAPICBASE    0xFEE00000	// for Local-APIC physical-address
#define	PROC_DIR	NULL	// for file's default directory
#define	PROC_MODE	0666	// for file's access permissions
#define FILESIZE  (256 * 2 * sizeof(char)) 	// for file's size

char modname[] = "smpwatch";
unsigned short oldidtr[3], newidtr[3];
unsigned long long *oldidt, *newidt;
unsigned long 	kpage;
unsigned long	*isr_original;
unsigned char	*intr_counter;
void 		*apic;

ssize_t
my_write( struct file *file, const char *buf, size_t len, loff_t *pos ),
my_read( struct file *file, char *buf, size_t len, loff_t *pos );
int my_open( struct inode *inode, struct file *file );

struct file_operations 
my_fops =	{
		owner:		THIS_MODULE,
		write:		my_write,
		read:		my_read,
		open:		my_open,
		};


void load_IDTR( void *regimage )
{
	asm(" lidtl %0 " : : "m" (*(unsigned short*)regimage) );
}

//--------  INTERRUPT SERVICE ROUTINES  ---------//
void isr_entry( void );
asm("	.text					");
asm("	.type	isr_entry, @function		");
asm("	.align	16				");
asm("isr_entry:					");
asm("	i = 0					");
asm("	.rept	256				");
asm("	pushl	$i				");
asm("	jmp	ahead				");
asm("	i = i + 1				");
asm("	.align	16				");
asm("	.endr					");
asm("ahead:					");
asm("	push	%ebp				");
asm("	mov	%esp, %ebp			");
asm("	push	%eax				");
asm("	push	%ecx				");
asm("	push	%edx				");
//
asm("	mov	4(%ebp), %ecx			");	
asm("	movzx	%cl, %ecx			");
asm("	mov	%ss:isr_original, %edx		");
asm("	mov  	%ss:(%edx, %ecx, 4), %eax	"); 
asm("	mov	%eax, 4(%ebp)			");

asm("	mov	%ss:apic, %eax			");
asm("	mov	%ss:0x23(%eax), %ch		");
asm("	and	$1, %ch				");

asm("	mov	%ss:intr_counter, %edx		");
asm("	incb	%ss:(%edx, %ecx, 1)		");
//
asm("	pop	%edx				");
asm("	pop	%ecx				");
asm("	pop	%eax				");
asm("	pop	%ebp				");
asm("	ret					");
//-----------------------------------------------//


ssize_t
my_read( struct file *file, char *buf, size_t len, loff_t *pos )
{
	if ( *pos >= FILESIZE ) return -EINVAL;
	if ( *pos + len > FILESIZE ) len = FILESIZE - *pos;
	if ( copy_to_user( buf, intr_counter, len ) ) return -EFAULT;
	*pos += len;
	return	len;
}


ssize_t
my_write( struct file *file, const char *buf, size_t len, loff_t *pos )
{
	memset( intr_counter, 0, FILESIZE );
	return	len;	
}



int my_open( struct inode *inode, struct file *file )
{
	memset( intr_counter, 0, FILESIZE );
	return	0;  // SUCCESS	
}





static int __init my_init( void )
{
	struct proc_dir_entry	*entry;
	unsigned long		i, isrlocn;
	
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// allocate kernel memory for counters, pointers, and new IDT
	kpage = get_zeroed_page( GFP_KERNEL );
	if ( !kpage ) return -ENOMEM;

	// setup references to our arrays of pointers and counters
	isr_original = (long *)(kpage + 2048);	
	intr_counter = (char *)(kpage + 3072);

	// initialize our module's global variables
	asm(" sidtl oldidtr \n sidtl newidtr ");
	memcpy( newidtr+1, &kpage, sizeof( unsigned long ) );
	oldidt = (unsigned long long*)(*(unsigned long*)(oldidtr+1));
	newidt = (unsigned long long*)(*(unsigned long*)(newidtr+1));
	memcpy( newidt, oldidt, 256 * sizeof( unsigned long long ) );
	
	// map the local-APIC
	apic = ioremap_nocache( LAPICBASE, PAGE_SIZE );
	if ( !apic ) { free_page( kpage ); return -EIO; }
	
	// setup our array of indirect jump-addresses
	for (i = 0; i < 256; i++)
		{
		unsigned long long  gate = oldidt[ i ];
		gate &= 0xFFFF00000000FFFFLL;
		gate |= ( gate >> 32 );
		gate &= 0x00000000FFFFFFFFLL;
		isr_original[ i ] = gate;
		}

	// build our new table of interrupt descriptors
	isrlocn = (unsigned long)isr_entry;
	for (i = 0; i < 256; i++)
		{
		unsigned long long oldgate, newgate;
		oldgate = oldidt[ i ];
		oldgate &= 0x0000FF00FFFF0000LL;
		newgate = isrlocn;
		newgate &= 0x00000000FFFFFFFFLL;
		newgate |= ( newgate << 32 );
		newgate &= 0xFFFF00000000FFFFLL;
		newgate |= oldgate;
		newidt[ i ] = newgate;
		isrlocn += 16;
		}

	// activate the new Interrupt Descriptor Table
	load_IDTR( newidtr );
	smp_call_function( load_IDTR, newidtr, 1, 1 );
	
	// create proc file with read-and-write capabilities
	entry = create_proc_entry( modname, PROC_MODE, PROC_DIR );
	entry->proc_fops = &my_fops;
	return	0;  // SUCCESS
}


static void __exit my_exit( void )
{
	// destroy our module's proc file
	remove_proc_entry( modname, PROC_DIR );

	// reactivate original Interrupt Descriptor Table
	smp_call_function( load_IDTR, oldidtr, 1, 1 );
	load_IDTR( oldidtr );

	// unmapo the local APIC
	iounmap( apic );

	// release allocated kernel memory
	if ( kpage ) free_page( kpage );
	printk( "<1>Removing \'%s\' module\n", modname );
}

MODULE_LICENSE("GPL");
module_init(my_init);
module_exit(my_exit);

