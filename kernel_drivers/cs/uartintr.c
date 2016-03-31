//-------------------------------------------------------------------
//	uartintr.c
//
//	This module installs an interrupt-handler for the UART, then
//	programs the UART to generate an interrupt whenever new data 
//	is received.  The baud-rate is programmed for 115200 bps and 
//	the 8-N-1 data-format is established.  The interrupt service
//	routine simply transmits each received character back to its
//	sender.  (The purpose here is to illustrate the Linux syntax
//	device-drivers must use for interrupt-management with kernel
//	version 2.6.22.5 -- too new to be covered by our textbooks.)  
//
//	You can test this module, by running our 'trycable.cpp' demo
//	on the adjacent workstation attached via a null-modem cable.
//
//	NOTE: Written and tested with Linux kernel version 2.6.22.5.
//
//	programmer: ALLAN CRUSE
//	written on: 11 OCT 2007
//-------------------------------------------------------------------

#include <linux/module.h>	// for init_module() 
#include <linux/interrupt.h>	// for request_irq()
#include <asm/io.h>		// for inb(), outb() 

#define UART_BASE	0x03F8
#define UART_IRQ	0x04

char modname[] = "uartintr";


irqreturn_t uart_isr( int irq, void *dev_id )
{
	// read the UART's Interrupt Identification register
	int	intr_ident = inb( UART_BASE + 2 ) & 0x0F;

	if ( intr_ident == 0x01 )   // no UART interrupts pending 
		return IRQ_NONE;

	if ( intr_ident == 0x04 )   // data-received interrupt
		outb( inb( UART_BASE ), UART_BASE );

	return	IRQ_HANDLED;
}













static int __init my_init( void )
{
	printk( "<1>\nInstalling \'%s\' module\n", modname );

	// install the UART interrupt-handler
	if ( request_irq( UART_IRQ, &uart_isr, IRQF_DISABLED,
		modname, &modname ) < 0 ) return -EBUSY;

	// initialize the UART for this demo's desired operations
	outb( 0x80, UART_BASE+3 );	// LINE CONTROL
	outw( 0x0001, UART_BASE );	// DIVISOR LATCH
	outb( 0x03, UART_BASE+3 );	// LINE CONTROL

	outb( 0x00, UART_BASE+2 );	// FIFO CONTROL
	outb( 0x0B, UART_BASE+4 );	// MODEM_CONTROL
	outb( 0x01, UART_BASE+1 );	// INTERRUPT ENABLE

	return	0;  // SUCCESS
}


static void __exit my_exit( void )
{
	// disable all UART interrupts
	outb( 0x00, UART_BASE+1 );	// INTERRUPT ENABLE
	outb( 0x00, UART_BASE+4 );	// MODEM CONTROL

	// remove our interrupt-handler
	free_irq( UART_IRQ, modname );

	printk( "<1>Removing \'%s\' module\n", modname );
}


module_init( my_init );
module_exit( my_exit );
MODULE_LICENSE("GPL");

