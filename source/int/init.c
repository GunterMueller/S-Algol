#include "int.h"
#include <sys/types.h>
#include <signal.h>

/*
#define OUTLINE
#define REALS		"/usr/lib/S/realio.out"
*/

/*
#ifdef	RASTER
#define RASTERFUNCS	"/usr/local/lib/S/Sraster.out"
#define RASTERFNS	"SRASTERFUNCS"
#endif	RASTER
#define REALS		"/user/staff/dave/S/fastS/sun/release/source/int.alpha/realio.out"
#define REALIO  	"SREALIO"
*/
#ifdef	RASTER
#define RASTERFUNCS	"/usr/local/lib/S/Sraster.out"
#define RASTERFNS	"SRASTERFUNCS"
#endif	RASTER
#define REALS		"/usr/local/lib/S/realio.out"
#define REALIO  	"SREALIO"
#ifdef OUTLINE
#define OUTFUNCS	"/usr/local/lib/S/outline.out"
#define OUTFNS		"SOUTFUNCS"
#endif	OUTLINE
#define BLOCK_SIZE	-6
#define VERSION_NO	-8
#define START_ADDR	-5
#define STRUC_ADDR	-4
#define RIO_FLAGS	-6
#define FLOW_OFF	-5
#define VERS_OFF	-4
#ifdef PERQ
#define NO_S_FUNCS	57	/* 0 - NO_S_FUNCS standard functions */
#ifdef OUTLINE
#define NO_OUT_FUNCS	15	/* standard functions written in outline */
#endif	OUTLINE
#else
#ifdef	RASTER
#define NO_S_FUNCS	60	/* 0 - NO_S_FUNCS standard functions */
#define NO_RASTER_FUNCS	9	/* standard functions written for rasters */
#else
#define NO_S_FUNCS	43	/* 0 - NO_S_FUNCS standard functions */
#ifdef OUTLINE
#define NO_OUT_FUNCS	10	/* standard functions written in outline */
#endif
#endif	RASTER
#endif PERQ

#define S_CHAR		020000000001	/* bit 1 and 32 */

#define I_W		12
#define S_W		2
#define R_W		14

#define HEAP_MIN	BUFF_SIZE + 3 * OVERHEAD
#if	defined( M68000 ) || defined( ALPHA )
#define	DEF_HEAP_SIZE	1536*1024	/* 1x5 Mb */
#define	SVSHEAP		"SHEAP"		/* SHEEP */
#define	MIN_HEAP_SIZE	64*1024		/* 64K bytes */
#define	MAX_HEAP_SIZE	16*1024*1024	/* 16Mb bytes */
#endif	/* M68000 || ALPHA */
#define MAX_MEM		5		/* 5 x 64K chunks of memory */
#define BREAK_FOR	65536		/* 64K bytes */
#define INIT_HEAP_SIZ   51200 		/* initial heap size - 200 k bytes */
short int *cp ;				/* short for indexing into code */
int *cp_l ;			/* for extraction of 4 byte numbers */


/*
**	The layout of memory is like this :
**
**             <-- lo memory                             --> hi memory
**
**                                                     heap base        heap max
**                                                       |               |
**                                                       v               v
**   ---------------------------------------------------------------------
**   |         |       |      |           |              |               |
**   | interp- | user- | char | main      | pointer      |               |
**   |         |       |      |     <-----|        <-----|      heap     |
**   |   reter | code  | tab  | stack     | stack        |               |
**   |         |       |      |           |              |               |
**   ---------------------------------------------------------------------
**                                        ^              ^
**                                        |              |
**                                        sb             psb
**
*/

extern	off_t	lseek() ;
extern	psint	*get_store() ;
extern	psint	*init_screen() ;
extern	psint	*init_cursor() ;
#ifdef	WEB
char	tmpname[ 19 ] ;
int	tmpf ;
extern	int	daemonise() ;
extern	int	startup() ;
extern	int	mkstemp() ;
#endif	/* WEB */

initialise( argc,argv )
int argc ; char **argv ;
{
	int fi,tmp ;
	extern int *create_so(),*open_si(),set_signals() ;

#ifdef	WEB
        ( void ) strcpy( tmpname,"/tmp/salgol.XXXXXX" ) ;
        tmpf = mkstemp( tmpname );
	{
		int	len ;
		char	s[ 100 ] ;

		sprintf( s,"Running %s\n",argv[ 1 ] ) ;
		write( tmpf,s,strlen( s ) ) ;
	}
	close( tmpf ) ;
#endif	WEB
	gc_allowed = SFALSE ;
	set_signals() ;
	process_size = get_store() ;
	if ( argc < 2 ) Error( "No code file specified" ) ;
	fi = open( argv[ 1 ],0 ) ;
	get_code( fi ) ;
	if ( cp[ VERS_OFF ] != VERSION_NO ) Error( "Version number clash - re-compile using the lastest compiler" ) ;
	if ( cp[ FLOW_OFF ] ) fs_init() ;
	else flow_sum = SFALSE ;
	tmp = mkpsptr( storeBase )  ;
#ifdef PERQ
	s_base = ( short int * ) ( tmp + cp_l[ STRUC_ADDR ] / 2 ) ;
	pc = tmp * 2 + cp_l[START_ADDR ] ;
#else
	s_base = ( short int * ) ( ( char * )( storeBase ) + cp_l[ STRUC_ADDR ] ) ;
	pc = ( unsigned char * )( storeBase ) + cp_l[START_ADDR ] ;
#endif

/*
	if ( cp[ RIO_FLAGS ] ) get_reals() ;
*/
	get_reals() ;

	init_ct() ; 
#ifdef PERQ
	curs_init() ;
#endif PERQ
	init_stacks() ; init_heap() ;

	get_options( argc,argv ) ;

	/* initialise s.i and s.o */
	*--psp = mkpsptr( create_so() ) ; 
	*--psp = mkpsptr( open_si() ) ;
#ifndef	WEB
#ifdef	RASTER
/*
	make_screen() ;
*/
#ifdef	RASTOP
	*--psp = mkpsptr( init_screen() ) ;
	*--psp = mkpsptr( init_cursor() ) ;
#else
	*--psp = mkpsptr( NIL_PTR ) ;
	*--psp = mkpsptr( NIL_PTR ) ;
#endif	RASTOP
#endif	RASTER
#else
	*--psp = mkpsptr( storeBase ) ;
	*--psp = mkpsptr( storeBase ) ;
#endif	/*WEB*/
	sb[ PC ] = ( psint )( pc - ( unsigned char * ) storeBase ) ;
	sb[ SL ] = mkpsptr( sf ) ;
	sb[ DL ] = mkpsptr( sb ) ;
	sb[ PDL ] = mkpsptr( psb ) ;
	gc_allowed = STRUE ;
#ifdef	WEB
	daemonise() ;
        ( void ) strcpy( tmpname,"/tmp/salgol.XXXXXX" ) ;
        tmpf = mkstemp( tmpname );
	startup( argv[ 1 ] ) ;
#endif	/*WEB*/
}

init_ct()
{
	register int i,*ptr,val ;

	if ( ( int ) cp % 2 ) ptr = ( int * ) ( cp + 1 ) ;
	else ptr = ( int * ) ( cp + 2 ) ;
	char_tab = ptr ; ptr += 256 ;
	cp = ( short * ) ptr ; m_top = ( int * ) ptr ;
	i = 128 ; val = S_CHAR ;
#ifdef M68000
	while ( i-- ) { *--ptr = ( i << 24 ) ; *--ptr = val ; }
#else
#ifdef PERQ
	while ( i-- ) { *--ptr = ( i << 16 ) ; *--ptr = val ; }
#else
	while ( i-- ) { *--ptr = i ; *--ptr = val ; }
#endif PERQ
#endif M68000
	cp_l = ( int * ) cp ;
}

#ifdef PERQ
curs_init()		/* make space for the cursors */
{
	register int *ptr,i,fd ;

	ptr = ( int * ) cp ;	/* lets assume we're not over a page */
	if ( ( int ) ptr % 4 )	/* get some 64 bit aligned memory   */
		ptr = ( int * )( ( ( int ) ptr & ~ B_0_1 ) + 4 ) ;
	cur_table = ptr + 3 ;	/* table of bitmap vectors */
	for ( i = 0 ; i < 32 ; i++ )
	{
		*ptr = ( int )( ptr + 10 ) ; ptr++ ;
		*ptr++ = 0 ;
		*ptr++ = 0 ;
		*ptr++ = B_29 | B_31 ;	/* vec of ints */
		*ptr++ = -3 ;
		*ptr++ = 128 ;
		*ptr++ = 64 ;
		*ptr++ = 64 ;
		*ptr++ = 7 ;
		*ptr++ = 64 ;
		ptr += 128 ;
	}
	if ( ( fd = open( "/etc/wcurs/sel",0 ) ) == -1 )
		error( "cannot find default cursor file - /etc/wcurs/sel" ) ;
	if ( read( fd,( char * )( ptr ),516 ) != 516 )
		error( "cannot read default cursor file" ) ;
	( void ) close( fd ) ;	
	ptr += 129 ;
	cp = ( short * ) ptr ; m_top = ( int * ) ptr ;
	cp_l = ( int * ) cp ;
}
#endif PERQ

init_stacks()
{
	register int i,*ptr ;
#ifdef OUTLINE
	int *optr ;
#endif
#ifdef RASTER
	int *optr ;
#endif RASTER

        ptr = m_top + MS_SIZE ;	/* make room for m-stack */
	p_top = ptr ;		/* save for setting up of p-stack */

	/* MAIN STACK */

	/* initialise the standard functions */
#ifdef OUTLINE
	ptr -= NO_OUT_FUNCS * 2 ;
	optr = ptr ;
#endif
#ifdef RASTER
	ptr -= NO_RASTER_FUNCS * 2 ;
	optr = ptr ;
#endif RASTER
	for ( i = NO_S_FUNCS ; i >= 0 ; ) { *--ptr = i-- ; *--ptr = 0 ; }

	sb = ptr ; sf = sb ;
	ptr = ptr - 6 ;	/* leave space for 1st stack frame */

	/* initialise the pre-declared main stack identifiers */
	*--ptr = I_W ;
	*--ptr = S_W ;
	*--ptr = R_W ;
	*--ptr = MAXINT ;
	*--ptr = EPSILON0 ;
	*--ptr = EPSILON1 ;
	*--ptr = PI0 ;
	*--ptr = PI1 ;
	*--ptr = MAXREAL0 ;
	*--ptr = MAXREAL1 ;

	init_rio( ptr ) ;
	ptr -= 10 ;
        sp = ptr ;

#ifdef OUTLINE
	get_outln() ;
	init_outln( optr ) ;
#endif
#ifdef RASTER
	get_raster() ;
	init_raster( optr ) ;
#endif RASTER

	/* POINTER STACK */

	p_top += 3 ;		/* make room for vector header see GC */
	psb = p_top + PS_SIZE ;	/* make room for p-stack */
	psp = psb ; psf = psb ;
}

init_heap()
{
	register psint *ptr ;

	ptr = ( psint * ) ( psb + 1  );
	heap_base = ptr ; heap_max = process_size - 1 ; /* the last word that can be used */

								/* check we have at leat the initial heap size of free space */
	if ( heap_base + INIT_HEAP_SIZ > process_size ) Error( "Program too large" ) ;
#ifdef	COMPACT
	heap_p = heap_base ;
#else
	heap_base[ 1 ] = ( psint ) mkpsptr( heap_base + 2 ) ; heap_base[ 3 ] = 0 ;
#ifdef	PERQ
	heap_base[ 2 ] = ( ( int ) heap_max - ( int ) heap_base ) / 2 - 1  ;
#else
	heap_base[ 2 ] = ( ( psint ) heap_max - ( psint ) heap_base ) / 4 - 1  ;
#endif	PERQ
#endif	COMPACT
}

get_code( fi )
register int fi ;
{
	register char *buff ; register int ans ;
	int size ;
	short block_no ;
	size = 0 ; block_no = 0 ;

	if ( fi == -1 ) Error( "Can't open code file" ) ;

	buff = ( char * ) storeBase ; 

	( void ) lseek( fi,( off_t )BLOCK_SIZE,2 ) ;
	( void ) read( fi,( char * )( &size ),4 ) ;
	( void ) read( fi,( char * )( &block_no ),2 ) ;
	if ( block_no != -1 ) Error( "Program not bound" ) ;
	( void ) lseek( fi,( off_t )0,0 ) ;
	if ( ( ans = read( fi,buff,size ) ) != size || ans == -1 )
	 Error( "Read error on code file" ) ;
	buff += ans ; ( void ) close( fi ) ;
	cp = ( short * ) buff ; cp_l = ( int * ) buff ; 
}

#if	defined( M68000 ) || defined( ALPHA )
psint get_heap_size()
{
        char *hsz ;
        psint heap_size ;
	extern char *getenv() ;

        heap_size = DEF_HEAP_SIZE ;     /* default heap size */

        hsz = getenv( SVSHEAP ) ;      /* shell variable holding desired heap size */
        if ( hsz != ( char * ) 0 )      /* if one was specified use it! */
        {
                char *c ;
                psint slen,scale,req_size ;

                slen = ( psint) 0 ;                             /* check the length of the string */
                c = hsz ;                                       /* and that it only contains digits */
                while( *c != ( psint ) 0 )
                {                                               /* give up if SVSHEAP contains non digits */
                        if ( *c < ( char )( '0' ) || *c > ( char )( '9' ) )
                        {
                                ( void ) printf( "%s must only contain decimal digits - using %d instead\n",SVSHEAP,DEF_HEAP_SIZE ) ;
                                return( heap_size ) ;
                        }
                        *c++ -= ( char )( '0' ) ;
                        slen++ ;
                }
 
                if ( slen > ( psint ) 8 || slen == ( psint ) 0 ){
                        ( void ) printf( "%s must be in the range %d to %d - using %d instead\n",
                                SVSHEAP,MIN_HEAP_SIZE,MAX_HEAP_SIZE,DEF_HEAP_SIZE ) ;
                        return( heap_size ) ;
                }
 
                scale = ( psint ) 1 ;                           /* calculate the integer */
                req_size = ( psint ) 0 ;
                while( slen-- > ( psint ) 0 )
                {
                        req_size += ( ( psint ) *--c ) * scale ;
                        scale *= ( psint ) 10 ;
                }
 
                                                        /* check the integer is
in the correct range */
                if ( req_size < MIN_HEAP_SIZE || req_size > MAX_HEAP_SIZE )
                {
                        ( void ) printf( "%s must be in the range %d to %d - using %d instead\n",
                                SVSHEAP,MIN_HEAP_SIZE,MAX_HEAP_SIZE,DEF_HEAP_SIZE ) ;
                        return( heap_size ) ;
                }
                return( req_size ) ;
        }
        return( heap_size ) ;
}

psint	*get_store()
{
        char *bptr ;
        psint heap_size ;
        extern char *malloc() ;
 
        heap_size = get_heap_size() ;                   /* discover required heap size - or use default 1 Mb */
        heap_size &= ~( ( psint ) 3 ) ;                 /* round down to a multiple of 4 bytes */
 
        bptr = malloc( heap_size ) ;
        if ( ( psint ) bptr == ( psint ) 0 ) error( "Initial heap too large" ) ; 
        if ( ( psint ) bptr % ( psint ) 4 )                             /* test 32 bit alignment */
                bptr = ( char * )( ( ( ( psint ) bptr + ( psint ) 4 ) / ( psint ) 4 ) * ( psint ) 4 ) ;
	storeBase = ( psint * ) bptr ;
	bptr += heap_size ;
	return( ( int * ) bptr ) ;
}
#else
int get_store()
{
	extern char *sbrk() ;
	int i ;

	storeBase = ( int * )( ( int )( sbrk( BREAK_FOR ) ) + 3  & ~B_0_1 ) ;	/* try to get 64 k */
	if ( ( int ) storeBase == -1 ) Error( "Can't get store" ) ;
	for( i = 1 ; ( i < MAX_MEM ) && ( ( int ) sbrk( BREAK_FOR ) != -1 ) ; i++ ) ;
	return( ( int ) ( int * )( ( int )( sbrk( 0 ) ) & ~B_0_1 ) ) ;
}
#endif	/* M68000 || ALPHA */

get_options( argc,argv )
int argc ; char **argv ;
{
	register int *base,size ; register char *c ;
	extern psint *c_v_p() ;

	base = c_v_p( 1,argc ) ;
	*psb = mkpsptr( base ) ; base += 3 ;
	for ( size = ( int ) argc ; size-- ; ) *base++ = 0 ;
	while ( argc-- )
	{
		c = argv[ argc ] ; size = 0 ;
		while ( *c++ ) size++ ;
		*--base = make_string( size,argv[ argc ] ) ;
	}
}

fs_init()
{
	extern char *malloc() ;
	char *tst ;

	flow_sum = STRUE ;
	tst = malloc( cp[ FLOW_OFF ] * 4 + 8 ) ;
	if ( tst == ( char * ) 0 )
		Error( "Can't get space for flow summary" ) ;
	if ( ( int ) tst % 4 )
	{
		tst = ( char * ) ( ( ( int ) tst + 4 ) / 4 * 4 ) ;
	}
	flow_vec = ( int * ) tst ;
	*flow_vec = cp[ FLOW_OFF ] ;
}

get_reals()
{
	register int j ; register char *buff ;
	short int vno ;
	int *wp,size,f,offset ;
	unsigned char *pr ;
	char *get_res ;
	extern char *getenv() ;

	if ( ( get_res = getenv( REALIO ) ) == ( char * ) 0 ) get_res = REALS ;
	if ( ( f = open( get_res,0 ) ) == -1 ) Error( "Can't open real I/O file" ) ;
	size = 0 ;
	buff = ( char * ) cp ;
	( void ) lseek( f,( off_t )( VERS_OFF * 2 ),2 ) ; ( void ) read( f,( char * )( &vno ),2 ) ;
	if ( vno != VERSION_NO ) Error( "Real I/O routines out of date" ) ;
	( void ) read( f,( char * )( &size ),4 ) ;
	( void ) lseek( f,( off_t )0,0 ) ;
	if ( ( j = read( f,buff,size ) ) != size || j == -1 ) Error( "Read error on real I/O file" ) ;
	( void ) close( f ) ;
	cp = ( short * ) ( buff + j ) ; cp_l = ( int * ) cp ;
	pr = ( unsigned char * ) ( buff + cp_l[ STRUC_ADDR ] ) ;
	while( !( pr[-1] ) ) pr-- ; /* skip padd to byte after finish.op */
	pr -= 36 ; /* 1st store closure parameter */

	wp = ( int * ) work_space ; j = 5 ;
	while ( j-- )
	{ 
#ifdef PERQ
		offset = ( int )( *pr++ ) << 16 ; 
		offset |= ( int )( *pr++ ) << 24 ;
		offset |= ( int )( *pr++ ) ; 
		offset |= ( int )( *pr++ ) << 8 ;
#else
#ifdef M68000
		offset = ( int )( *pr++ ) << 24 ;
		offset |= ( int )( *pr++ ) << 16 ; 
		offset |= ( int )( *pr++ ) << 8 ;
		offset |= ( int )( *pr++ ) ; 
#else
		offset = ( int )( *pr++ ) ; 
		offset |= ( int )( *pr++ ) << 8 ; 
		offset |= ( int )( *pr++ ) << 16 ;
		offset |= ( int )( *pr++ ) << 24 ;
#endif M68000
#endif PERQ
 		buff = ( char * ) pr ;
		pr += 3 ;
	  	buff += offset ;
		*wp++ = ( psint )( buff - ( char * ) storeBase ) ;
	}
}

init_rio( ptr )
register int *ptr ;
{
	register int *wp,i ;

	wp = ( int * ) work_space ;
	i = 5 ;
	while ( i-- ) { *--ptr = *wp++ ; *--ptr = mkpsptr( sf ) ; }
}

#ifdef OUTLINE
get_outln()
{
	register int j ; register char *buff ;
	short int vno ;
	int *wp,size,f,offset ;
	unsigned char *pr ;
	char *get_res ;
	extern char *getenv() ;

	if ( ( get_res = getenv( OUTFNS ) ) == ( char * ) 0 ) get_res = OUTFUNCS ;
	if ( ( f = open( get_res,0 ) ) == -1 ) Error( "Can't open outline file" ) ;
	size = 0 ;
	buff = ( char * ) p_top ;					/* otherwise main stack will be overwritten */
	( void ) lseek( f,( off_t )( VERS_OFF * 2 ),2 ) ; ( void ) read( f,( char * )( &vno ),2 ) ;
	if ( vno != VERSION_NO ) Error( "Outline routines out of date" ) ;
	( void ) read( f,( char * )( &size ),4 ) ;
	( void ) lseek( f,( off_t )0,0 ) ;
	if ( ( j = read( f,buff,size ) ) != size || j == -1 ) Error( "Read error on Outline functions file" ) ;
	( void ) close( f ) ;
	p_top = ( int * ) buff ; cp = ( short * ) ( buff + j ) ; cp_l = ( int * ) cp ;
	pr = ( unsigned char * ) ( buff + cp_l[ STRUC_ADDR ] ) ;
	while( !( pr[-1] ) ) pr-- ; /* skip padd to byte after finish.op */
	pr -= NO_OUT_FUNCS * 7 + 1 ; /* 1st store closure parameter */

	
	wp = ( int * ) work_space ; j = NO_OUT_FUNCS ;
	while ( j-- )
	{ 
#ifdef PERQ
		offset = ( int )( *pr++ ) << 16 ; 
		offset |= ( int )( *pr++ ) << 24 ;
		offset |= ( int )( *pr++ ) ; 
		offset |= ( int )( *pr++ ) << 8 ;
#else
#ifdef M68000
		offset = ( int )( *pr++ ) << 24 ;
		offset |= ( int )( *pr++ ) << 16 ;
		offset |= ( int )( *pr++ ) << 8 ; 
		offset |= ( int )( *pr++ ) ; 
#else
		offset = ( int )( *pr++ ) ; 
		offset |= ( int )( *pr++ ) << 8 ; 
		offset |= ( int )( *pr++ ) << 16 ;
		offset |= ( int )( *pr++ ) << 24 ;
#endif M68000
#endif PERQ
 		buff = ( char * ) pr ;
		pr += 3 ;
	  	buff += offset ;
		*wp++ = mkpsptr( ( psint * ) buff ) ;
	}
}

init_outln( ptr )
register int *ptr ;
{
	register int *wp,i ;

	wp = ( int * ) work_space ;
	i = NO_OUT_FUNCS ;
	while ( i-- ) { *ptr++ = mkpsptr( sf ) ; *ptr++ = *wp++ ; }
}

#endif

#ifdef RASTER
get_raster()
{
	register int j ; register char *buff ;
	short int vno ;
	int *wp,size,f,offset ;
	unsigned char *pr ;
	char *get_res ;
	extern char *getenv() ;

	if ( ( get_res = getenv( RASTERFNS ) ) == ( char * ) 0 ) get_res = RASTERFUNCS ;
	if ( ( f = open( get_res,0 ) ) == -1 ) Error( "Can't open outline file" ) ;
	size = 0 ;
	buff = ( char * ) p_top ;
	( void ) lseek( f,( off_t )( VERS_OFF * 2 ),2 ) ; ( void ) read( f,( char * )( &vno ),2 ) ;
	if ( vno != VERSION_NO ) Error( "Raster routines out of date" ) ;
	( void ) read( f,( char * )( &size ),4 ) ;
	( void ) lseek( f,( off_t )0,0 ) ;
	if ( ( j = read( f,buff,size ) ) != size || j == -1 ) Error( "Read error on Raster functions file" ) ;
	( void ) close( f ) ;
	p_top = ( int * ) buff ; cp = ( short * ) ( buff + j ) ; cp_l = ( int * ) cp ;
	pr = ( unsigned char * ) ( buff + cp_l[ STRUC_ADDR ] ) ;
	while( !( pr[-1] ) ) pr-- ; /* skip padd to byte after finish.op */
	pr -= NO_RASTER_FUNCS * 7 + 1 ; /* 1st store closure parameter */

	
	wp = ( int * ) work_space ; j = NO_RASTER_FUNCS ;
	while ( j-- )
	{ 
#ifdef PERQ
		offset = ( int )( *pr++ ) << 16 ; 
		offset |= ( int )( *pr++ ) << 24 ;
		offset |= ( int )( *pr++ ) ; 
		offset |= ( int )( *pr++ ) << 8 ;
#else
#ifdef M68000
		offset = ( int )( *pr++ ) << 24 ;
		offset |= ( int )( *pr++ ) << 16 ;
		offset |= ( int )( *pr++ ) << 8 ; 
		offset |= ( int )( *pr++ ) ; 
#else
		offset = ( int )( *pr++ ) ; 
		offset |= ( int )( *pr++ ) << 8 ; 
		offset |= ( int )( *pr++ ) << 16 ;
		offset |= ( int )( *pr++ ) << 24 ;
#endif M68000
#endif PERQ
 		buff = ( char * ) pr ;
		pr += 3 ;
	  	buff += offset ;
		*wp++ = ( psint )( buff - ( char * ) storeBase ) ;
	}
}

init_raster( ptr )
register int *ptr ;
{
	register int *wp,i ;

	wp = ( int * ) work_space ;
	i = NO_RASTER_FUNCS ;
	while ( i-- ) { *ptr++ = mkpsptr( sf ) ; *ptr++ = *wp++ ; }
}

#endif RASTER

