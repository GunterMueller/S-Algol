#include "int.h"
#define VM1	"Vector bounds\nlwb = "
#define VM2	"\nupb = "

extern int *c_v_ib(),*c_v_p(),*c_v_r() ;

flow_info()
{
	int fd,nchs ;

	nchs = *flow_vec * 4 + 4 ;
	if ( ( fd = creat( "count.SS", 420 ) ) == -1 )
		Error( "Cannot create file count.SS" ) ;
	if ( write( fd,( char * ) flow_vec,nchs ) != nchs )
		Error( "Cannot write file count.SS" ) ;
	( void ) close( fd ) ;
}

int *chain_down( level )
register int level ;
{
	register int *ans ;

	ans = sf ;
	while ( level-- ) ans = mkptr( ans[ SL ] ) ;
	return( ans ) ;
}

trace( user_call ) 
int user_call ;
{
	register int *tmpp,line ; 
	extern char *i_to_s() ;
	psint	more = STRUE ;
	unsigned	char *XX ;

	tmpp = sf ;
	line = line_no ;
	if ( user_call ) f_lush( mkptr( psb[ -1 ] ) ) ;
	( void ) write_error( 2,"\nTrace of procedure calls in reverse order\n",43 ) ;
	do
	    {
		unsigned char *ptr ; 
		int size ; 
		char *str ;

		if ( tmpp == sb ) more = SFALSE ;
		if ( tmpp[ SL ] ) 
		{
			( void ) write_error( 2,"\nline ",6 ) ;
			str = i_to_s( &size,line ) ;
			( void ) write_error( 2,str,size ) ;
			( void ) write_error( 2," in ",4 ) ;
			XX = ( unsigned char * ) storeBase + tmpp[ PC ] ; 
			if ( XX[ 1 ] & B_15 ) XX += 7 ; /* if a structure offset */
			else XX += 5 ;
			XX = ( unsigned char * )( ( ( long ) XX + 3 ) & ~B_0_1 ) ;	/* skip perq padding */
			size = *( ( int * )( XX ) ) ;
			XX += 4 ;
			( void ) write_error( 2,( char * ) XX,size ) ;
		}
		line = tmpp[ LINE_NO ] ;
		tmpp = mkptr( tmpp[ DL ] ) ;
	}
	while ( more ) ;
	( void ) write_error( 2,"\n",1 ) ;
}

int eq_string( s1,s2 )
int *s1,*s2 ;
{
	register int e1 ; 
	register char *c1,*c2 ;

	if ( s1 == s2 ) return( STRUE ) ;
	if ( !mkpsptr( s1 ) || !mkpsptr( s2 ) ) return( SFALSE ) ;
	e1 = *s1++ & B_0_15 ;
	if ( e1 != ( *s2++ & B_0_15 ) ) return( SFALSE ) ;
	c1 = ( char * ) s1 ; 
	c2 = ( char * ) s2 ;
	while ( e1-- ) if ( *c1++ != *c2++ ) return( SFALSE ) ;
	return( STRUE ) ;
}

int lt_string( s1,s2 )
int *s1,*s2 ;
{
	register int count ; 
	register char *c1,*c2 ;
	int e1,e2 ;

	if ( s1 == s2 || !mkpsptr( s2 ) ) return( SFALSE ) ;
	if ( !mkpsptr( s1 ) ) return( STRUE ) ;
	e1 = *s1++ & B_0_15 ; 
	e2 = *s2++ & B_0_15 ;
	count = ( e1 > e2 ? e2 : e1 ) ;
	c1 = ( char * ) s1 ; 
	c2 = ( char * ) s2 ;
	while ( count-- ) if ( *c1 < *c2 ) return( STRUE ) ; 
	else if ( *c1++ > *c2++ ) return( SFALSE ) ;
	return( e1 < e2 ) ;
}

overfl()
{
	error( "Integer overflow" ) ;
}

int obj_size( ptr )
int *ptr ;
{
	int tmp ;

	tmp = ( *ptr >> 28 ) & 15 ;
	switch ( tmp ) {
	case 2  :	/* vector of pointers */
	case 10 :	/* vector of ints or bools */
		return( ptr[ 2 ] - ptr[ 1 ] + 4 ) ;
		break ;
	case 4  :	/* file descriptor */
		return( *ptr & B_0_15 ) ;
		break ;	
	case 6  :	/* vector of reals */
		return( ( ptr[ 2 ] - ptr[ 1 ] ) * 2 + 5 ) ;
		break ;	
	case 8  :	/* a string */
		return( ( ( *ptr & B_0_15 ) + 3 ) / 4 + 1 ) ;
		break ;
	case 12 :	/* a structure */
		return( s_base[ ( *ptr & B_0_15 ) / 2 ] ) ;
		break ;
#ifdef	RASTER
	case 14 :	/* an image */
		return( 7 ) ;
		break ;
#endif	RASTER
	default :	
		return( *ptr ) ;
	}

/*
	This is put in to keep lint happy
*/
	return( *ptr ) ;
}

bounds_error( m1,m2,i1,i2 )
char *m1,*m2 ; 
psint i1,i2 ;
{
	register char *c1,*c2 ; 
	psint start,size ;
	extern char *i_to_s() ;

	c1 = work_space + 30 ; 
	start = ( psint ) c1 ;
	c2 = m1 ;
	while ( *c1++ = *c2++ ) ;
	c2 = i_to_s( &size,i1 ) ; 
	c1-- ;
	while ( size-- ) *c1++ = *c2++ ;
	c2 = m2 ;
	while ( *c1++ = *c2++ ) ;
	c2 = i_to_s( &size,i2 ) ; 
	c1-- ;
	while ( size-- ) *c1++ = *c2++ ;
	*c1 = ( psint ) 0 ;

	error( ( char * ) start ) ;
}

int *iliffe( bounds )
int *bounds ;
{
	int lwb,upb,size,*p1,*p2 ;
	int Res1 = res1,Res2 = res2 ;

	lwb = bounds[ 1 ] ; 
	upb = *bounds ; 
	if ( lwb > upb ) bounds_error( VM1,VM2,lwb,upb ) ;
	size = upb - lwb + 1 ;

	if ( bounds != sp )
	{
		p1 = c_v_p( lwb,upb ) ;
		*--psp = mkpsptr( p1 ) ; 
		p1 += 2 ; 
		p2 = p1 + size ;
		while ( p2 != p1 ) *p2-- = 0 ;
		while ( size-- ) 
		{
			p2 = iliffe( bounds - 2 ) ;
			( mkptr( *psp ) )[ size + 3 ] = mkpsptr( p2 ) ;
		}
		return( mkptr( *psp++ ) ) ;
	}
	if ( type == REAL )
	{
		p1 = c_v_r( lwb,upb ) ; 
		p1 += 2 ;
		p2 = p1 + size * 2 ;
		while ( p2 != p1 ) { 
			*p2-- = Res2 ; 
			*p2-- = Res1 ; 
		}
		p1 -= 2 ;
		return( p1 ) ;
	}
	if ( type == PNTR )
	{
		p1 = c_v_p( lwb,upb ) ;
		Res1 = psb[ -res1 ] ;
	}
	else p1 = c_v_ib( lwb,upb ) ;
	p1 += 2 ; 
	p2 = p1 + size ;
	while ( p2 != p1 ) *p2-- = Res1 ;
	p1 -= 2 ;
	return( p1 ) ;
}
