/**************************************************************************************/
/*  This module should enable and trap all the floating point exceptions if a f68881  */
/* chip is being used - OR an FPA. for soft floating point silly messages may appear. */
/**************************************************************************************/

#include <signal.h>
#include "int.h"

int fpmode_() ;
int fpstatus_() ;

#define MBIT( x )		( 1 << x )

sigFpe( sn,fpe_num )					/* floating point exception has been raised */
int sn,fpe_num ;					/* sn is the signal number - fpe_num is the fp error code */
{
	int stat,oldstat ;

	signal( SIGFPE,SIG_IGN ) ;			/* disable the fpe interrupt */
	stat = 0 ;
	oldstat = fpstatus_( &stat ) ;			/* look at the status flags & reset them */
	error( "System signal - floating point exception" ) ;
}

enablefpe()						/* enable fpe signals assuming f68881 is present */
{							/* if there isnt one present silly messages may appear!!! */
	int new,nnew ;
	int stat,oldstat ;

							/* the bits set the following traps:
							10: fp divide by zero,
							11: fp underflow,
							12: fp overflow,
							13: fp operand error,
							14: fp signalling NAN,
							15: fp FPA bus error?.
							the following bits were not used:
							8: fp inexact1
							9: fp inexact2 */
	new =  MBIT( 10 ) | MBIT( 11 ) | MBIT( 12 ) | MBIT( 13 ) | MBIT( 14 ) | MBIT( 15 ) ;
	nnew = fpmode_( &new ) ;			/* enable the relevant traps */

	stat = 0 ;
	oldstat = fpstatus_( &stat ) ;			/* reset the status flags */

	signal( SIGFPE,sigFpe ) ;
}
