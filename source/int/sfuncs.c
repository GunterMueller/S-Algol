#include <sys/types.h>
#include <sys/times.h>
#include <sys/wait.h>
#include "int.h"
#include "math.h"
#include "stand.h"

unsigned char *go_sub( sfn )
int sfn ;
{
	int tmp1,n ;
	register int *ptr ; 
	register char *c ; 
	psreal f1,f2 ;
	unsigned char *pc ;
	extern char *i_to_s() ;
	extern int make_string(),trace(),*c_v_ib(),*c_v_p() ;

	switch ( sfn )
	{
	case SIN:
/*
		f = ( double * ) sp ;
		f = sin( *f ) ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		f2 = sin( ( double ) f1 ) ;
		ptr = ( psint * )( &f2 ) ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case COS:
/*
		f = ( double * ) sp ;
		*f = cos( *f ) ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		f2 = cos( ( double ) f1 ) ;
		ptr = ( psint * )( &f2 ) ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case EXP:
/*
		f = ( double * ) sp ;
		*f = exp( *f ) ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		f2 = exp( ( double ) f1 ) ;
		ptr = ( psint * )( &f2 ) ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case LN:
/*
		f = ( double * ) sp ;
		*f = log( *f ) ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		f2 = log( ( double ) f1 ) ;
		ptr = ( psint * )( &f2 ) ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case SQRT:
/*
		f = ( double * ) sp ;
		*f = sqrt( *f ) ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		f2 = sqrt( ( double ) f1 ) ;
		ptr = ( psint * )( &f2 ) ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case ATAN:
/*
		f = ( double * ) sp ;
		*f = atan( *f ) ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		f2 = atan( ( double ) f1 ) ;
		ptr = ( psint * )( &f2 ) ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case TRUNCATE:
/*
		f = ( double * ) sp ;
		*++sp = floor( *f ) ;
		goto ret_ib ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		*++sp = floor( ( double ) f1 ) ;
		goto ret_ib ;
	case ABS: 
		if ( *sp < 0 ) *sp = -( *sp ) ;
		goto ret_ib ;
	case RABS:
/*
		f = ( double * ) sp ;
		if ( *f < 0.0 ) *f = -*f ;
		goto ret_r ;
*/
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = *sp ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;
		if ( f1 < 0.0 ) f1 = -f1 ;
		sp[ 1 ] = ptr[ FLOAT0 ] ;
		*sp = ptr[ FLOAT1 ] ;
		goto ret_r ;
	case FIDDLE_R:
		ptr = c_v_ib( 1,2 ) ;
		ptr[ 3 ] = *sp++ ; 
		ptr[ 4 ] = *sp++ ;
		*--psp = mkpsptr( ptr ) ; 
		goto ret_p ;
	case OPEN: 
		sopen() ; 
		goto ret_p ;
	case CLOSE: 
		sclose() ; 
		goto ret_v ;
	case SEEK: 
		sseek() ; 
		goto ret_v ;
	case CREATE: 
		create() ; 
		goto ret_p ;
	case FLUSH: 
		sflush() ; 
		goto ret_v ;
	case OPTIONS: 
		*--psp = *psb ; 
		goto ret_p ;
	case CODE: 
		*--psp = mkpsptr( ( *sp++ & B_0_6 ) * 2 + char_tab ) ; 
		goto ret_p ;
	case DECODE: 
		c = ( char * ) ( mkptr( *psp++ ) ) ; 
		if ( !mkpsptr( ( int * ) c ) ) error( "Decode of empty string" ) ;
		*--sp = c[ 4 ] ; 
		goto ret_ib ;
	case LENGTH: 
		ptr = mkptr( *psp++ ) ;
		*--sp = ( !( mkpsptr( ptr ) ) ? 0 : *ptr & B_0_15 ) ; 
		goto ret_ib ;
	case IFORMAT: 
		c = i_to_s( &n,*sp++ ) ; 
		*--psp = make_string( n,c ) ; 
		goto ret_p ;
	case LETTER: 
		c = ( char * ) ( mkptr( *psp++ ) ) ;
		*--sp = c && ( c[ 4 ] >= 'a' && c[ 4 ] <= 'z' || c[ 4 ] >= 'A' && c[ 4 ] <= 'Z' ) ;
		goto ret_ib ;
	case DIGIT: 
		c = ( char * ) ( mkptr( *psp++ ) ) ;
		*--sp = c && ( c[ 4 ] >= '0' && c[ 4 ] <= '9' ) ;
		goto ret_ib ;
	case LINE_NUMBER: 
		*--sp = line_no ; 
		goto ret_ib ;
	case SHIFT_R: 
		tmp1 = *sp++ ; 
		*sp >>= tmp1 ;
		if ( *sp & B_31 ) *sp ^= ( B_31 >> tmp1 - 1 ) ;
		goto ret_ib ;
	case SHIFT_L: 
		tmp1 = *sp++ ; 
		*sp <<= tmp1 ;
		goto ret_ib ;
	case B_AND: 
		tmp1 = *sp++ ; 
		*sp &= tmp1 ; 
		goto ret_ib ;
	case B_OR: 
		tmp1 = *sp++ ; 
		*sp |= tmp1 ; 
		goto ret_ib ;
	case EXEC: 
		{
			register int *ptr,*vbase ; 
			register char *c1 ; 
			int no_elements ;

			ptr = mkptr( *psp++ ) ;
			no_elements = ptr[ 2 ] - ptr[ 1 ] + 1 ;
			vbase = ( int * ) work_space ;
			c1 = ( char * ) ( vbase + no_elements + 1 ) ;
			ptr += 3 ;
			while ( no_elements-- )
			{
				register int size,*p ; 
				register char *c2 ;

				*vbase++ = mkpsptr( ( int * ) c1 ) ;
				if ( p = mkptr( *ptr++ ) )
				{
					size = *p++ & B_0_15 ;
					c2 = ( char * ) p ;
					while ( size-- ) *c1++ = *c2++ ;
				}
				*c1++ = 0 ;
			}
			*vbase = 0 ; 
			ptr = ( int * ) work_space ;
			execv( mkptr( *ptr ),( char ** )( work_space + 4 ) ) ;
		}
		goto ret_v ;
	case FORK: 
		*--sp = fork() ; 
		goto ret_ib ;
	case WAIT: 
		{
			register int ans ; 
/*
			union wait pid ;
			pid.w_status = *sp++ ; 
*/
			int pid ;

			pid = *sp++ ;
			ans = wait( &pid ) ;
			ptr = c_v_ib( 0,1 ) ;
			*--psp = mkpsptr( ptr ) ;
			ptr[ 3 ] = ans ; 
/*
			ptr[ 4 ] = pid.w_status ;
*/
			ptr[ 4 ] = pid ;
			goto ret_p ; 
		}
	case  TRACE: 
		trace( STRUE ) ; 
		goto ret_v ; 
	case FIND_SUBSTR: 
		{
			register int *s1,*s2 ; 
			int size1,size2 ;

			s1 = mkptr( *psp++ ) ;
			if ( !mkpsptr( s1 ) )
			{ 
				*psp++ ; 
				*--sp = 0 ; 
			}
			else
			{
				s2 = mkptr( *psp++ ) ;
				if ( !mkpsptr( s2 ) ) *--sp = 0 ;  
				else if ( ( size1 = ( *s1++ & B_0_15 ) ) > ( size2 = ( *s2++ & B_0_15 ) ) ) *--sp = 0 ;
				else
				{
	
					register char *c1,*c2 ;
					int i,match = SFALSE ;
					c1 = ( char * ) s1 ;
					c2 = ( char * ) s2 ;
					for( i = 0 ; ( i <= size2 - size1 ) && !match ; i++ )
						if ( match = !( strncmp( c1,c2 + i,size1 ) ) )
							*--sp = i + 1 ; 
					if ( !( match ) ) *--sp = 0 ;
				}
			}
			goto ret_ib ; 
		}
	case ENVIRONMENT: 
		{
			register char *c ; 
			register int *base ; 
			int size,envc ;
			extern psptr make_string() ;

			envc = 0 ; 
			while ( envv[ envc ] ) envc++ ;
			base = c_v_p( 0,envc - 1 ) ;
			*--psp = mkpsptr( base ) ; 
			base += 3 ;
			for ( size = ( int ) envc ; size-- ; )
				*base++ = 0 ;
			while ( envc-- )
			{
				c = envv[ envc ] ; 
				size = 0 ;
				while ( *c++ ) size++ ;
				*--base = make_string( size,envv[ envc ] ) ;
			}
			goto ret_p ; 
		}
	case SYSTEM: 
		{
			int size,*p ; 
			register char *c1,*c2 ;

			p = mkptr( *psp++ ) ;
			size = *p++ & B_0_15 ;
			c1 = work_space ;
			c2 = ( char * ) p ; 
			while ( size-- ) *c1++ = *c2++ ; 
			*c1 = 0 ;
#ifdef	RASTOP
			stop_itimer() ;
#endif	RASTOP
			*--sp = system( work_space ) ; 
#ifdef	RASTOP
			start_itimer() ;
#endif	RASTOP
			goto ret_ib ;
		}
	case RANDOM:	/* random number generator - CACM vol 11,9 p642 */
		{
			tmp1 = ( *sp >> 13 ) ^ *sp ;
			*sp = ( ( tmp1 << 18 ) ^ tmp1 ) & B_0_30 ;
			goto ret_ib ;
		}
	case B_NOT:
		tmp1 = *sp++ ;
		*sp = ~tmp1 ;
		goto ret_ib ;
	case B_XOR:
		tmp1 = *sp++ ;
		*sp ^= tmp1 ;
		goto ret_ib ;
	case DATE:
		{
			int tim ;
			extern char *ctime() ;
			extern time_t time() ;
			extern psptr make_string() ;

			tim = ( int ) time( ( time_t * ) 0 ) ;
			*--psp = make_string( 24,ctime( &tim ) ) ;
			goto ret_p ;
		}
	case TIME:
		{

			struct	tms the_time ;
			( void ) times( &the_time ) ; 
			*--sp = the_time.tms_utime ;
			goto ret_ib ;
		}
	case INTRPT:
		*--sp = intrpt ;
		intrpt = SFALSE ;
		goto ret_ib ;
	case INTRPTON:
		intron() ;
		goto ret_v ;
	case INTRPTOFF:
		introff() ;
		goto ret_v ;
	case RDVEC:
		readvec() ;
		goto ret_p ;
	case WRTVEC:
		wrtvec() ;
		goto ret_v ;
#ifdef PERQ
	case CRTBMAP:
		mkbitmap() ;
		goto ret_p ;
	case CRTWIND:
		mkwind() ;
		goto ret_p ;
	case RASTEROP:
		rastop() ;
		goto ret_v ;
	case CDRAWLN:
		drwlne() ;
		goto ret_v ;
	case LOCATOR:
		locator() ;
		goto ret_p ;
	case COLOUROF:
		colourof() ;
		goto ret_ib ;
	case SETCURS:
		setcurs() ;
		goto ret_v ;
	case ISAWIND:
		isawindow() ;
		goto ret_ib ;
	case CCURS:
		curs() ;
		goto ret_p ;
	case LNEND:
		lnend() ;
		goto ret_ib ;
	case SETLOCAT:
		setlocator() ;
		goto ret_v ;
	case SIZEOF:
		areasize() ;
		goto ret_p ;
	case HIDEUPS:
		hide_updates() ;
		goto ret_v ;
	case SHOWUPS:
		show_updates() ;
		goto ret_v ;

	/* the functions below here are written in S-algol */

	case CURS:
		error( "Outline function cursor not defined" ) ;
	case MKBMAP:
		error( "Outline function make.bitmap not defined" ) ;
	case MKWIND:
		error( "Outline function make.window not defined" ) ;
	case DRAWPIC:
		error( "Outline function draw not defined" ) ;
	case LIMIT:
		error( "Outline function limit not defined" ) ;
	case ERASETO:
		error( "Outline function erase.to not defined" ) ;
	case CHKCOL:
		error( "Outline function check.colour not defined" ) ;
	case DRAWLIN:
		error( "Outline function draw.line not defined" ) ;

	case STDWIN:
		error( "Outline function standard.window not defined" ) ;
	case COLOF:
		error( "Outline function colour.of not defined" ) ;
	case FILL:
		error( "Outline function fill not defined" ) ;
	case MNKMEN:
		error( "Outline function make.menu not defined" ) ;
	case PICMEN:
		error( "Outline function pick.menu not defined" ) ;
	case FRST:
		error( "Outline function frst not defined" ) ;
	case SCND:
		error( "Outline function scnd not defined" ) ;
#endif PERQ
#ifdef	RASTER
        case XDMN:
#ifdef  RASTOP
                Xdim() ;
#else
                psp++ ; *--sp = ( psint ) 0 ;
#endif  RASTOP
                goto ret_ib ;
        case YDMN:
#ifdef  RASTOP
                Ydim() ;
#else
                psp++ ; *--sp = ( psint ) 0 ;
#endif  RASTOP
                goto ret_ib ;
        case LOCAT:
#ifdef  RASTOP
                locator() ;
#else
                *--psp = ( psint ) NIL_PTR ;
#endif  RASTOP
                goto ret_p ;
       case CURSTIP:
#ifdef  RASTOP
                curstip() ;
#endif  RASTOP
                goto ret_p ;
        case PIXEL:
#ifdef  RASTOP
                pixel() ;
#else
                psp++ ; sp++ ;
#endif  RASTOP
                goto ret_ib ;
        case CONST_IM:
#ifdef  RASTOP
                cimage() ;
#endif  RASTOP
                goto ret_p ;
        case VAR_IM:
#ifdef  RASTOP
                cimage() ;
#else
#endif  RASTOP
                goto ret_p ;
        case DEPTH:
#ifdef  RASTOP
                depth() ;
#else
                psp++ ; *--sp = ( psint ) 0 ;
#endif  RASTOP
                goto ret_ib ;
        case PDEPTH:
                *sp = ( *sp >> ( psint ) 24 ) & B_0_7 ;
                goto ret_ib ;
        case COL_MAP:
#ifdef  RASTOP
                colmap() ;
#else
                sp += 2 ;
#endif  RASTOP
                goto ret_v ;
        case COL_OF:
#ifdef  RASTOP
                colof() ;
#endif  RASTOP
                goto ret_ib ;
        case LNEND:
#ifdef  RASTOP
                lnend() ;
#else
                psp++ ; sp += 3 ;
#endif  RASTOP
                goto ret_ib ;
        case PLANE:
#ifdef  RASTOP
                plane() ;
#else
                sp++ ;
                ptr = c_v_ib( 0,0 ) ;
                ptr[ 3 ] = ( psint ) 0 ;
                *psp = mkpsptr( ptr ) ;
#endif  RASTOP
                goto ret_p ;
        case PNXLINE:
#ifdef  RASTOP
                pnxlne() ;
#else
                psp++ ; sp += 6 ;
#endif  RASTOP
                goto ret_v ;
        case CURSON:
#ifdef  RASTOP
                setcursor( 0 ) ;
#endif  RASTOP
                goto ret_v ;
        case CURSOFF:
#ifdef  RASTOP
                setcursor( 1 ) ;
#endif  RASTOP
                goto ret_v ;
        case INPPEND:
                inp_pend() ;
                break ;
#endif	RASTER
	default:
		error( "unknown standard function" ) ;
	}

ret_ib:
	tmp1 = *sp++ ; 
	psp = psf ; 
	sp = sf ; 
	line_no = sf[ LINE_NO ] ;
	pc = ( unsigned char * ) storeBase +  sf[ RA ] ; 
	sf = mkptr( sf[ DL ] ) ; 
	psf = mkptr( sf[ PDL ] ) ;
	*--sp = tmp1 ;
	return( pc ) ;

ret_r:
/*
	f = ( double * ) sp ; 
	psp = psf ; 
	sp = sf - 2 ; 
	line_no = sf[ LINE_NO ] ;
	pc = ( unsigned char * ) storeBase +  sf[ RA ] ; 
	sf = mkptr( sf[ DL ] ) ; 
	psf = mkptr( sf[ PDL ] ) ;
	f1 = ( double * ) sp ; 
	*f1 = *f ;
	return( pc ) ;

*/
	ptr = ( psint * )( &f1 ) ;
	ptr[ FLOAT0 ] = sp[ 1 ] ;
	ptr[ FLOAT1 ] = *sp ;
	psp = psf ; 
	sp = sf ; 
	line_no = sf[ LINE_NO ] ;
	pc = ( unsigned char * ) storeBase + sf[ RA ] ;
	sf = mkptr( sf[ DL ] ) ;
	psf = mkptr( sf[ PDL ] ) ;
	*--sp = ptr[ FLOAT0 ] ;
	*--sp = ptr[ FLOAT1 ] ;
	return( pc ) ;
ret_p:
	tmp1 = *psp++ ; 
	psp = psf ; 
	sp = sf ; 
	line_no = sf[ LINE_NO ] ;
	pc = ( unsigned char * ) storeBase +  sf[ RA ] ; 
	sf = mkptr( sf[ DL ] ) ; 
	psf = mkptr( sf[ PDL ] ) ;
	*--psp =  ( psptr ) tmp1 ;
	return( pc ) ;

ret_v:
	psp = psf ; 
	sp = sf ; 
	line_no = sf[ LINE_NO ] ;
	pc = ( unsigned char * ) storeBase +  sf[ RA ] ; 
	sf = mkptr( sf[ DL ] ) ; 
	psf = mkptr( sf[ PDL ] ) ;
	return( pc ) ; 
}

