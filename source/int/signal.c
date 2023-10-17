#include "int.h"
#include "signal.h"

extern int inited ;

void sighup()
{
	( void ) signal( SIGHUP,SIG_IGN ) ;
#ifdef	SUN
	{
	extern reset_input() ;

	reset_input() ;
	}
#endif	SUN
	if ( inited ) tidy_up() ;
	Exit( -1 ) ;
}

void sigint()
{
	( void ) signal( SIGINT,SIG_IGN ) ;
	intrpt = STRUE ;
	( void ) signal( SIGINT,sigint ) ;
}

intron()
{
        ( void ) signal( SIGINT,sigint ) ;
}

introff()
{
        ( void ) signal( SIGINT,SIG_DFL ) ;
}

void sigquit()
{
	( void ) signal( SIGQUIT,SIG_IGN ) ;
#ifdef	SUN
	{
	extern reset_input() ;

	reset_input() ;
	}
#endif	SUN
	if ( inited ) tidy_up() ;
	Exit( -1 ) ;
}

void sigill()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - illegal instruction" ) ;
}

void sigtrap()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - trace trap" ) ;
}

void sigiot()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - IOT ins" ) ;
}

void sigemt()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - EMT ins" ) ;
}

void sigbus()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - bus error" ) ;
}

void sigsegv()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - segmentation violation" ) ; 
}

void sigsys()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - bad system call" ) ;
}

void sigpipe()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - write to broken pipe" ) ;
}

static int ping ;

void sigalrm()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
	error( "System signal - alarm clock" ) ;
#endif	SUN
 
        signal( SIGALRM,SIG_IGN ) ;

#ifdef	RASTOP
        if ( ping )
        {
                D_pingDisplay() ;                               /* check the display is happy! */
        }
        ping = !ping ;                                          /* but only on every other signal */
#endif	RASTOP
        signal( SIGALRM,sigalrm ) ;
}

void sigterm()
{
#ifdef	SUN
	extern reset_input() ;

	reset_input() ;
#endif	SUN
	error( "System signal - software terminate" ) ;
}

/*
#ifdef  X11
sigchild()
{
        extern psint Xscreen ;

	FIX THIS
        if ( Xscreen != ( psint ) -1  )
        {
                extern reset_input() ;

		Xscreen = ( psint ) -1 ;
                reset_input() ;
                error( "System signal - display process terminated" ) ;
        }
}
#endif  X11
*/

#ifdef PERQ
suicide()
{
	extern psint rsscr_size ;

	( void ) signal( SIGWSTS,SIG_IGN ) ;	/* switch off the signal */
	logsig( SIGWSTS ) ;
	if ( rsscr_size )		/* ignore if screen size not yet relevant */
	{
#ifdef	EXCEPT
		sys_error( SEWSTS ) ;
		reset_scr() ;			/* put the screen back to its original size */
	}
	( void ) signal( SIGWSTS,suicide ) ;
#else
		sigerror = "System signal - screen has changed size" ;
		if ( compacting ) sig_pend = STRUE ; else error( sigerror ) ;
	} else ( void ) signal( SIGWSTS,suicide ) ;
#endif	EXCEPT
}
#endif PERQ

set_signals()
{
#ifdef	SUN
	extern void sigwinch_hndlr() ;
	extern void sigFpe() ;
#endif	SUN

	( void ) signal( SIGHUP,sighup );
	( void ) signal( SIGINT,sigint );
	( void ) signal( SIGQUIT,sigquit );
	( void ) signal( SIGILL,sigill );
	( void ) signal( SIGTRAP,sigtrap ) ;
	( void ) signal( SIGIOT,sigiot ) ;
/* GM	( void ) signal( SIGEMT,sigemt ) ;  */
#ifdef	SUN
	enablefpe() ;
	( void ) signal( SIGFPE,sigFpe ) ;
#endif	SUN
	( void ) signal( SIGBUS,sigbus ) ;
	( void ) signal( SIGSEGV,sigsegv ) ;
	( void ) signal( SIGSYS,sigsys ) ;
	( void ) signal( SIGPIPE,sigpipe ) ;
	( void ) signal( SIGALRM,sigalrm ) ;
	( void ) signal( SIGTERM,sigterm ) ;
/*
#ifdef  X11
        signal( SIGCHLD,sigchild ) ;
#endif  X11
*/
#ifdef PERQ
	( void ) signal( SIGWSTS,suicide ) ;
#endif PERQ
#ifdef	SUN
	( void ) signal( SIGWINCH,sigwinch_hndlr ) ;
#endif	SUN
	intrpt = SFALSE ;
}

