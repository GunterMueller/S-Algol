/**************************************************************************/
/*  This module should enable and trap all the floating point exceptions  */
/*  on a sun4 or sun3 compiled with -f68881.                              */
/**************************************************************************/

#include "int.h"
#include "floatingpoint.h"

void sigFpe( sn,fpe_num )					/* floating point exception has been raised */
int sn,fpe_num ;					/* sn is the signal number - fpe_num is the fp error code */
{
#ifdef	EXCEPT
	sys_error( SEFPE ) ;
#else
	sigerror = "System signal - floating point exception" ;
	error( sigerror ) ;
#endif	EXCEPT
}

enablefpe()						/* enable fpe signals assuming fp h/ware is present */
{							/* if there isnt any present silly messages may appear!!! */
							/* enable all traps - then nobble inexact */
							/* enabled traps should be:
										fp divide by 0,
										fp underflow,
										fp overflow,
										invalid - operand error,
							*/
	if( ieee_handler( "set","all",sigFpe ) != 0 ||
	    ieee_handler( "set","inexact",SIGFPE_IGNORE ) != 0 )
		 error( "cannot enable the floating point exception handlers" ) ;

}

