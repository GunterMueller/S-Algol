#ifdef	RASTOP
#include "errno.h"
#include "int.h"
#include "graphics.h"
#include "sys/time.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include <signal.h>

#ifndef	NULL
#define	NULL	0
#endif
#define	FLAGS		1
#define WP_FLAG	 020000
#define CLOSED_FLAG     010000
#define	S_O_OFF		-1

#define	MOUSE_STRC	0
#define	POINT_STRC	4

extern char *STRUCTERR ;

extern int *c_structure(),*c_v_p(),*c_v_ib(),*c_image() ;
extern int errno ;
char *wdm1 =		"unable to perform draw.line" ;
char *wdm2 =		"closed file passed to raster.op" ;
char *wdm3 =		"unable to perform raster.op" ;
char *wdm4 =		"unable to perform make.bitmap" ;
char *wdm5 =		"unable to perform colour.of" ;
char *wdm6 =		"cannot initialise graphics functions" ;
char *wdm7 =		"cannot read default cursor file" ;

				/* cursor to track mouse */
static psuint curs_init[] = {   0x00000000,0x7E000000,0x7C000000,0x78000000,
				0x7C000000,0x6E000000,0x47000000,0x03800000,
				0x01C00000,0x00E00000,0x00700000,0x00380000,
				0x00100000,0x00000000,0x00000000,0x00000000 } ;

					/* cursor for click to quit */
static psuint quit_curs[] = {	0xE9740000,0x89450000,0x89460000,0x89460000,
				0xED750000,0x00000000,0x0E700000,0x04500000,
				0x04500000,0x04700000,0x00000000,0xF7EE0000,
				0x82AA0000,0xF2AE0000,0x12A80000,0xF2E80000 } ;


static psuint appleColourMap[] = {
          16777215, 13434879, 10092543, 6750207, 3407871, 65535, 16764159,
13421823, 10079487, 6737151, 3394815, 52479,
          16751103, 13408767, 10066431, 6724095, 3381759, 39423, 16738047,
13395711, 10053375, 6711039, 3368703, 26367,
          16724991, 13382655, 10040319, 6697983, 3355647, 13311, 16711935,
13369599, 10027263, 6684927, 3342591, 255,
          16777164, 13434828, 10092492, 6750156, 3407820, 65484, 16764108,
13421772, 10079436, 6737100, 3394764, 52428,
          16751052, 13408716, 10066380, 6724044, 3381708, 39372, 16737996,
13395660, 10053324, 6710988, 3368652, 26316,
          16724940, 13382604, 10040268, 6697932, 3355596, 13260, 16711884,
13369548, 10027212, 6684876, 3342540, 204,
          16777113, 13434777, 10092441, 6750105, 3407769, 65433, 16764057,
13421721, 10079385, 6737049, 3394713, 52377,
          16751001, 13408665, 10066329, 6723993, 3381657, 39321, 16737945,
13395609, 10053273, 6710937, 3368601, 26265,
          16724889, 13382553, 10040217, 6697881, 3355545, 13209, 16711833,
13369497, 10027161, 6684825, 3342489, 153,
          16777062, 13434726, 10092390, 6750054, 3407718, 65382, 16764006,
13421670, 10079334, 6736998, 3394662, 52326,
          16750950, 13408614, 10066278, 6723942, 3381606, 39270, 16737894,
13395558, 10053222, 6710886, 3368550, 26214,
          16724838, 13382502, 10040166, 6697830, 3355494, 13158, 16711782,
13369446, 10027110, 6684774, 3342438, 102,
          16777011, 13434675, 10092339, 6750003, 3407667, 65331, 16763955,
13421619, 10079283, 6736947, 3394611, 52275,
          16750899, 13408563, 10066227, 6723891, 3381555, 39219, 16737843,
13395507, 10053171, 6710835, 3368499, 26163,
          16724787, 13382451, 10040115, 6697779, 3355443, 13107, 16711731,
13369395, 10027059, 6684723, 3342387, 51,
          16776960, 13434624, 10092288, 6749952, 3407616, 65280, 16763904,
13421568, 10079232, 6736896, 3394560, 52224,
          16750848, 13408512, 10066176, 6723840, 3381504, 39168, 16737792,
13395456, 10053120, 6710784, 3368448, 26112,
          16724736, 13382400, 10040064, 6697728, 3355392, 13056, 16711680,
13369344, 10027008, 6684672, 3342336, 238,
          221, 187, 170, 136, 119, 85, 68, 34, 17, 60928, 56576, 47872,
43520, 34816, 30464, 21760, 17408, 8704, 4352,
          15597568, 14483456, 12255232, 11141120, 8912896, 7798784,
5570560, 4456448, 2228224, 1114112, 15658734, 14540253,
          12303291, 11184810, 8947848, 7829367, 5592405, 4473924, 2236962,
1118481, 0} ;

psint Xscreen ;		  	/* window id for X screen -1 => no X window */
psint Xscr,Yscr,Zscr ;			/* X, Y and Z dims of the screen */
psint realZscr ;			/* the real Z dimension of the screen */
psint xpos,ypos,but1,but2,but3 ;        /* public state of mouse */
psint dateStamp ;			/* X datestamp */
psint curlive = STRUE ;                 /* is the cursor live - liable to move */
psint curvis = SFALSE ;                 /* is the cursor visible */
psint curx,cury ;                       /* where is the hotspot */

int cmapsize = 0 ;			/* size of the screen's colour map segment */
					/* the colour tables for setting the colour map */
unsigned char red[ 256 ],green[ 256 ],blue[ 256 ] ;
unsigned char svred[ 256 ],svgreen[ 256 ],svblue[ 256 ] ;
char	defaultWindowname[ 256 ] ;

static int convertToXrule( int ) ;
static psint Ninit_Cmap( int ) ;

static struct itimerval save_itime = { {0,33000},{0,33000} } ;	  /* interval timer - 30 Hz interval timer */

void start_itimer(void)
{
	setitimer( ITIMER_REAL,&save_itime,NULL ) ;
}
 
void stop_itimer(void)
{
	struct itimerval itime,ivtime ;
 
	itime.it_interval.tv_sec = 0 ;
	itime.it_interval.tv_usec = 0 ;
	itime.it_value.tv_sec = 0 ;
	itime.it_value.tv_usec = 0 ;
	setitimer( ITIMER_REAL,&itime,&save_itime ) ;
	if ( save_itime.it_value.tv_usec <= 0 ) save_itime.it_value.tv_usec = 33000 ;
}

psint scan( x,y )	       /* calculates scan line length for given x,y */
psint x,y ;
{
	psint scanl ;
 
			/* check that dimensions are in valid range */
	if ( ( psint ) 8192 < x || x < ( psint ) 1 || y < ( psint ) 1 || ( psint ) 8192 < y )
		error( "bitmap dimensions not in range 1-8192" ) ;
 
			/* calculate normal scan line length - return if OK */
	scanl = ( ( x + ( psint ) 31 ) / ( psint ) 32 ) * ( psint ) 4 ;
	return( scanl ) ;
}

psint get_defCmap()
{
	char *defcmap ;
	extern char *getenv() ;
 
	defcmap = getenv( SVSCMAP ) ;		  /* shell variable holding desired colour map */
	if ( defcmap != ( char * ) 0 )		  /* if one was specified use it! */
	{
		if ( strcmp( defcmap,"blackonwhite" ) == 0 ) return( DEF_CMAP ) ;
		if ( strcmp( defcmap,"whiteonblack" ) == 0 ) return( ALT_CMAP ) ;
	}
	return( DEF_CMAP ) ;
}

psint get_maxZscr()
{
	char *maxZscr ;
	extern char *getenv() ;
 
	maxZscr = getenv( SVSZDIM ) ;		  /* shell variable holding desired max Z dimension */
	if ( maxZscr != ( char * ) 0 )		  /* if one was specified use it! */
	{
		if ( *maxZscr < ( char )( '0' ) || *maxZscr > ( char )( '8' ) || maxZscr[ 1 ] != ( char ) 0 )
		{
			printf( "%s must be a decimal digit in the range 0 to 8 - using %d instead\n",SVSZDIM,DEF_ZDIM ) ;
			return( DEF_ZDIM ) ;
		}
		return( ( psint )( *maxZscr - '0' ) ) ; /* return a number in the range 0 - 8 */
	}
	return( DEF_ZDIM ) ;
}
 

make_screen()	   /* setup the system interface to the screen */
{
	char *myterm ;
	int maxZscr,defCmap ;
	extern char *ttyname() ;
 
	maxZscr = get_maxZscr() ;		       /* max depth of image the user wnats to know about */
	defCmap = get_defCmap() ;
	myterm = ttyname( 1 ) ;			 /* find name of my tty */

	/* only do graphics if requested and logged onto the console - and mouse available */
	Xscreen = ( psint ) -1 ;
/*
	if ( ( maxZscr > 0 ) && ( myterm != 0 ) )
*/
	if ( ( maxZscr > 0 ) )
	{
		init_scr_X( maxZscr,defCmap ) ;
		if ( Xscreen == ( psint ) -1 ) 	/* set up text output tty emulator */
		{		       /* if no graphics cut the screen down to 1by1by1 */
			init_pstty() ;
			Xscr = 1 ;
			Yscr = 1 ;
			Zscr = 1 ;
			cmapsize = 0 ;
		}
	}
}

psint *init_screen()	/* make space for the screen */
{
	psint vecsz,*vec,slinc ;
	int maxZscr,defCmap ;
	psint i,*scr,*scrbm ;
	char *offsets[ MAXZDIM ] ;
	psint status,xpos,ypos ;

							/* calculate the size of the screen copy */
	maxZscr = get_maxZscr() ;		       /* max depth of image the user wnats to know about */
	defCmap = get_defCmap() ;
	Xscreen = ( psint ) -1 ;

	D_initDisplay() ;

	stop_itimer() ;
	Xscreen = D_openWindow( "",maxZscr,&Xscr,&Yscr,&Zscr ) ;
	start_itimer() ;
	if ( Xscreen == ( psint ) -1 ) return ( storeBase ) ;
	Xscr -= 100 ; Yscr -= 100 ;
	Zscr = Zscr > MAXZDIM ? MAXZDIM : Zscr ;

	xpos = ( psint ) -1 ;
	ypos = ( psint ) -1  ;

	slinc = scan( Xscr,Yscr ) ;
	vecsz = ( Yscr * slinc ) / ( psint ) 4 ;	/* no. of 4 byte words to hold pixels */

	screen = c_image() ;		/* create an image descriptor */
	screen[ BITMAP ] = ( psint ) 0 ;
							/* if there is a screenmake it standard output */
	screen[ WINDOW ] = ( Xscreen != ( psint ) -1 ) ? ( psb[ S_O_OFF ] ) : ( psint ) 0  ;
	screen[ X_OFFSET ] = ( psint ) 0 ;
	screen[ Y_OFFSET ] = ( psint ) 0 ;
	screen[ X_DIM ] = Xscr ;
	screen[ Y_DIM ] = Yscr ;

	vec = c_v_p( 1,Zscr ) ;	/* create a bmap vector */
	for ( i = 1 ; i <= Zscr ; i++ )
		vec[ 2 + i ] = ( psint ) 0 ;

	screen[ BITMAP ] = mkpsptr( vec ) ;		/* fill in ptr to dummy vector */


	/* now create the bitmaps to hold a copy of the screen */

	for ( i = 1 ; i <= Zscr ; i++ )
	{
		psint	*ptr ;

		vec = c_v_ib( -3,vecsz ) ;		/* create bitmap */
		vec[ DIM_X ] = Xscr ;
		vec[ DIM_Y ] = Yscr ;
		vec[ OFFSET ] = ( psint ) 7 ;
		vec[ NO_LINES ] = Yscr ;

		ptr = mkptr( screen[ BITMAP ] ) ;
		ptr[ 2 + i ] = mkpsptr( vec ) ;
	}


	scr = mkptr( screen[ BITMAP ] ) ;
	for ( i = ( psint ) 0 ; i < Zscr ; i++ )
	{
		scrbm = mkptr( scr[ ( psint ) 3 + i ] ) ;
		offsets[ i ] = ( char * )( scrbm + scrbm[ OFFSET ] )  ;
	}

	if ( Xscreen != ( psint ) -1 )
	{
		status = D_setScreen( Xscreen,xpos,ypos,Xscr,Yscr,Zscr,bitchk( scrbm ),BITMAPSFORMAT,offsets ) ;
		if ( status != ( psint ) 0 )
		{
			D_closeWindow( Xscreen ) ;
			screen = storeBase ;
		}
/*
		start_itimer() ;
*/
		init_Xcmap( defCmap ) ;
/*
		stop_itimer() ;
*/
		status = D_readWindowName( Xscreen,defaultWindowname ) ;
		status = D_renameWindow( Xscreen,SGRAPHICS ) ;
	}
	return( screen ) ;
}

void  ShowCursor( x,y )
psint	x,y ;
{
	psint	xdim,ydim,slinc ;
	psint	*ptr,result ;

	xdim = cursor[ X_DIM ] ;
	ydim = cursor[ Y_DIM ] ;

	slinc = scan( xdim,ydim ) ;
	ptr = mkptr( cursor[ BITMAP ] ) ;

	ptr = mkptr( ptr[ 3 + 0 ] ) ;
	ptr += ptr[ OFFSET ] ;

	if ( Xscreen != -1 )
	{
		result = D_setCursor( Xscreen,&xdim,&ydim,slinc,x,y,( unsigned char * ) ptr ) ;
		result = D_showCursor( Xscreen ) ;
		D_pingDisplay() ;
	}
}

psint *init_cursor()	/* make space for the cursor */
{
	psint i,vecsz,*vec,*vec1,slinc ;
	psint csize,*initcbits ;

	slinc = scan( CURXDIM,CURYDIM ) ;
	vecsz = ( CURYDIM * slinc ) / ( psint ) 4 ;

	cursor = c_image() ;					/* create an image descriptor */
	cursor[ BITMAP ] = ( psint ) 0 ;			/* fill in later */
	cursor[ WINDOW ] = ( psint ) 0 ;
	cursor[ X_OFFSET ] = ( psint ) 0 ;
	cursor[ Y_OFFSET ] = ( psint ) 0 ;
	cursor[ X_DIM ] = CURXDIM ;
	cursor[ Y_DIM ] = CURYDIM ;

	vec = c_v_p( 1,1 ) ;					/* create vector of bitmaps */
	vec[ 3 ] = ( psint ) 0 ;				/* will point at default cursor */

	cursor[ BITMAP ] = mkpsptr( vec ) ;	/* fill in pointer to bitmap vector */

	/* now initialise the bitmap to the default cursor */

	vec1 = c_v_ib( -3,vecsz ) ;				/* create bitmap */
	vec1[ DIM_X ] = CURXDIM ;
	vec1[ DIM_Y ] = CURYDIM ;
	vec1[ OFFSET ] = ( psint ) 7 ;
	vec1[ NO_LINES ] = CURYDIM ;

	vec[ 3 ] = mkpsptr( vec1 )  ;
	/*( ( psint * )( cursor[ BITMAP ] ) )[ 3 ] = ( psint ) vec ;	/* fill in pointer to bitmap vector */

	initcbits = vec1 + BITS ;
	csize = vecsz ;
	while( csize-- > (psint) 0 )
	{
		initcbits[ csize ] = curs_init[ csize ] ;
	} 

/*
	SetCursor( 0,0 ) ;		/* set hot spot to SUN 0,0 *
*/
	ShowCursor( 1,1 ) ;			/* display the cursor if mouse inuse */

	return( cursor ) ;
}

psint bitchk( bmap )	/* check bitmap vector is legal */
psint *bmap ;		/* return the scan line increment */
{
	psint size1,size2,slinc ;

	size1 = bmap[ UPB ] - bmap[ LWB ] - ( psint ) 3 ;	/* size of vector */

	slinc = scan( bmap[ DIM_X ],bmap[ DIM_Y ] ) ;
	if ( slinc == ( psint ) -1 ) error( "corrupt bitmap vector found( bad dims )" ) ;
	slinc /= ( psint ) 4 ;				/* slinc in 32 bit words */

	size2 = slinc * bmap[ DIM_Y ] ;		/* size of bitmap */
	if ( size1 < ( psint ) 0 || size1 < size2 )
		error( "bitmap vector is an inconsistent size" ) ;

	if ( bmap[ OFFSET ] != ( psint ) 7 )
		error( "corrupt bitmap vector found( bad offset)" ) ;

	return( slinc * ( psint ) 4 ) ;		/* scan line inc. for pixrect */
}
mk_image()
{
	psint pix,xdim,ydim,nplanes,vecsz,*vec,*res,slinc,col ;
	psint	*ptr,plane ;
	psint	slw,size ;

	pix = *sp++ ;				/* the pixel */
	nplanes = ( pix >> ( psint ) 24 ) & B_0_7 ;	/* depth of the pixel */
	ydim = *sp++ ;			/* y dimension of the image */
	xdim = *sp++ ;			/* x dimension of the image */
	if ( ydim < ( psint ) 1 || xdim < ( psint ) 1 ) error( "bitmap dimensions mustbe > 0" ) ;

	res = c_image() ;			/* create the image descriptor */
	res[ BITMAP ] = ( psint ) 0 ;
	res[ WINDOW ] = ( psint ) 0 ;
	res[ X_OFFSET ] = ( psint ) 0 ;
	res[ Y_OFFSET ] = ( psint ) 0 ;
	res[ X_DIM ] = xdim ;
	res[ Y_DIM ] = ydim ;

	*--psp = mkpsptr( res ) ;			/* save the image descr. in case of GC */

	vec = c_v_p( 1,nplanes ) ;			/* build intermediate vector */

						/* initialise ptr fields to 0s in case of GC */
	while( nplanes-- != ( psint ) 0 ) vec[ nplanes + 3 ] = ( psint ) 0 ;
	nplanes = vec[ UPB ] ;

						/* fill in ptr to vec of bmaps from image desc. */
	ptr = mkptr( *psp ) ;
	ptr[ BITMAP ] = mkpsptr( vec ) ;

/*
	( ( psint * )( *psp ) )[ BITMAP ] = ( psint ) vec ;
*/

	slinc = scan( xdim,ydim ) ;
	vecsz = ( ydim * slinc ) / ( psint ) 4 ;	/* size of a plane's bitmap in 32bit words */
	slw =  slinc / 4 ;


	plane = 0 ;
	while( plane < nplanes )			/* next create bitmap vectors and     */
	{					/* pointers to them from vec of bmaps */
		psint *nextBitmap,*new_bits ;

						/* create the next bitmap */
		nextBitmap = c_v_ib( -3,vecsz ) ;
		nextBitmap[ DIM_X ] = xdim ;
		nextBitmap[ DIM_Y ] = ydim ;
		nextBitmap[ OFFSET ] = ( psint ) 7 ;
		nextBitmap[ NO_LINES ] = ( psint ) 0 ;

						/* save ptr to new bmap in the vector of bitmaps */
		new_bits = nextBitmap + nextBitmap[ OFFSET ] ;               /* set new_bits to the first pixel */
		size = slw * ydim ;                    /* the size of a plane */
                                                                                /* the plane's pixels */
		col = ( pix & ( ( psint ) 1 << plane ) ) ? -1 : 0 ;
		while( size-- > 0 ) *new_bits++ = col ;                     /* set all the words in the next plane */
  
		vec = mkptr( ptr[ BITMAP ] ) ;
/*
		vec = ( psint * )( ( ( psint * )( *psp ) )[ BITMAP ] ) ;
*/
		vec[ plane + 3 ] = mkpsptr( nextBitmap ) ;
		plane++ ;
	}
}
	
lnend()
{
	psint *wst,bmapinc,*wdes_s,x,y,*vec,Ydim ;
	psint cy1,cy2,cx1,cx2,direct,col,col1,depth,ok,*scanl ;

/*
	badwin( ( psint ) 0 ) ;
*/
	wst = mkptr( *psp++ ) ;

	direct = *sp++ ;
	y = *sp++ ;
	x = *sp++ ;
	col = *sp++ ;
	if ( x < ( psint ) 0 || x > wst[ X_DIM ] - ( psint ) 1 || y < ( psint ) 0 || y > wst[ Y_DIM ] - ( psint ) 1 )
		error( "attempt to read pixels outside image" ) ;
	x += wst[ X_OFFSET ] ;
	y += wst[ Y_OFFSET ] ;

	wdes_s = mkptr( wst[ BITMAP ] ) ;
	vec = mkptr( wdes_s[ 3 ] ) ;
	bmapinc  = bitchk( vec ) / ( psint ) 4 ;
	Ydim = vec[ DIM_Y ] ;
	vec += vec[ OFFSET ] ;
	y = Ydim - y - ( psint ) 1 ;
	scanl = vec + y * bmapinc ;

	cy2 = Ydim - wst[ Y_OFFSET ] - ( psint ) 1 ;	/* clip y upb */
	cy1 = cy2 - wst[ Y_DIM ] + ( psint ) 1 ;		/* clip y lpb */
	cx1 = wst[ X_OFFSET ] ;			/* clip x lwb */
	cx2 = cx1 + wst[ X_DIM ] - ( psint ) 1  ;		/* clip x uwb */

						/* set up loop control and clip col depth */
	ok = STRUE ;
	col1 = col & ( psint ) 1 ;
	depth = ( col >> ( psint ) 24 ) & B_0_7 ;
	if ( depth > wdes_s[ 2 ] ) depth = wdes_s[ 2 ] ;
	switch( direct )
	{
	case 0:	while ( ok && x >= cx1 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) x-- ; else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) x-- ;
		}
		break ;
	case 1:	while ( ok && x > cx1 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) x-- ; else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) x-- ;
		}
		break ;
	case 2:	while ( ok && x <= cx2 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) x++ ; else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) x++ ;
		}
		break ;
	case 3:	while ( ok && x < cx2 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) x++ ; else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) x++ ;
		}
		break ;
	case 4:	while ( ok && y <= cy2 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) { scanl += bmapinc ; y++ ; } else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) { scanl += bmapinc ; y++ ; }
		}
		break ;
	case 5:	while ( ok && y < cy2 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) { scanl += bmapinc ; y++ ; } else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) { scanl += bmapinc ; y++ ; }
		}
		break ;
	case 6:	while ( ok && y >= cy1 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) { scanl -= bmapinc ; y-- ; } else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) { scanl -= bmapinc ; y-- ; }
		}
		break ;
	case 7:	while ( ok && y > cy1 )
		if ( ( ( scanl[ LWRD(x) ] >> BIT(x) ) & ( psint ) 1 ) != col1 ) { scanl -= bmapinc ; y-- ; } else
		{
			psint *line,i = ( psint ) 1 ;

			while ( ok && i < depth )
			{
				line = mkptr( wdes_s[ i + ( psint ) 3 ] ) ;
				line += line[ OFFSET ] + y * bmapinc ;
				ok = ( line[ LWRD(x) ] >> BIT(x) & ( psint ) 1 ) == ( col >> i & ( psint ) 1 ) ;
			}
			ok = !ok ;
			if ( ok ) { scanl -= bmapinc ; y-- ; }
		}
		break ;
	default: error( "line.end direction must be in range 0-7" ) ;
	}

	*--sp = ( direct > ( psint ) 3 ? Ydim - y - ( psint ) 1  - wst[ Y_OFFSET ]: x - wst[ X_OFFSET ] ) ;
}

pixel()	/* find the colour of a given pixel using raster.op */
{
	psint *wst,*wdes_s,x,y,pixel,nplanes ;
	int *ptr,bmapinc ;

/*
	badwin( 0 ) ;
*/

	wst = mkptr( *psp++ ) ;

	y = *sp++ ;
	x = *sp++ ;
	if ( x < ( psint ) 0 || x >= wst[ X_DIM ] || y < ( psint ) 0 || y >= wst[ Y_DIM ] )
		error( "attempt to read pixel outside image" ) ;
	x += wst[ X_OFFSET ] ;
	y += wst[ Y_OFFSET ] ;

	wdes_s = mkptr( wst[ BITMAP ] ) ;				/* get the vector of bitmaps */
	nplanes = wdes_s[ 2 ] ;
	pixel = nplanes << ( psint ) 24 ;
	ptr = mkptr( wdes_s[ 3 ] ) ;			/* ptr points at bitmap */
	y = ptr[ DIM_Y ] - y - ( psint ) 1 ;					/* invert Y dimension */

	while( nplanes-- > ( psint ) 0 )						/* find pixel in each plane */
	{
		ptr = mkptr( wdes_s[ nplanes + ( psint ) 3 ] ) ;
		bmapinc = bitchk( ( psint * ) ptr ) ;
		ptr += 7 + y * bmapinc / 4  ;			/* NB 7 is offset from start of bitmap vector to the bitmap */
		if ( ( ptr[ LWRD(x) ] >> BIT(x) ) & 1 ) pixel |= ( ( psint ) 1 << nplanes ) ;
	}
	*--sp = pixel ;
}

Ydim()		/* Y dimension of an image */
{
	psint *wst ;

	wst = mkptr( *psp++ ) ;	/* get pointer to image desc */
	*--sp = wst[ Y_DIM ] ;	/* return size */
}

Xdim()		/* X dimension of an image */
{
	psint *wst ;

	wst = mkptr( *psp++ ) ;	/* get pointer to image desc */
	*--sp = wst[ X_DIM ] ;	/* return size */
}

plane()
{
	psint *wst,p ;

	wst = mkptr( *psp ) ;			/* get pointer to image's vector of bitmaps */
	wst = mkptr( wst[ BITMAP ] ) ;	/* either way wst points at vec of planes */

	p = *sp++ ;					/* index to required plane */
	if ( p < ( psint ) 0 || p >= wst[ UPB ] )
		error( "attempt to select non-existent plane of an image" ) ;
	wst = mkptr( wst[ p + ( psint ) 3 ] ) ;	/* wst is the required plane */
	*psp = mkpsptr( wst ) ;				/* put plane on pointer stck */
}

depth()
{
	psint *wst ;

					/* get pointer to image's vector of bitmaps */
	wst = mkptr( *psp++ ) ;
	wst = mkptr( wst[ BITMAP ] ) ;

	*--sp = wst[ 2 ] ; 		/* Return upb of vector */
}

cimage()
{
	psint *oimage,*ovec,*nimage,*nvec,i,vecsz,nplanes ;
	psint xdim,ydim,*bmap,sx,sy,slinc ;
	psint	*ptr ;

						/* make the new image descriptor */
	nimage = c_image() ;
	nimage[ BITMAP ] = ( psint ) 0 ;
	nimage[ WINDOW ] = ( psint ) 0 ;
	nimage[ X_OFFSET ] = ( psint ) 0 ;
	nimage[ Y_OFFSET ] = ( psint ) 0 ;

						/* how large is the new bitmap */
	oimage = mkptr( *psp ) ;
	xdim = oimage[ X_DIM ] ;
	ydim = oimage[ Y_DIM ] ;
	ovec = mkptr( oimage[ BITMAP ] ) ;
	nplanes = ovec[ 2 ] ;			/* upb is no. of planes */

	slinc = scan( xdim,ydim ) ;
	vecsz = ( ( slinc * ydim ) + ( psint ) 3 ) / ( psint ) 4 ;

						/* complete and save the new image descriptor */
	nimage[ X_DIM ] = xdim ;
	nimage[ Y_DIM ] = ydim ;
	*psp = mkpsptr( nimage ) ;

						/* create new vector of bitmaps */
	nvec = c_v_p( 1,nplanes ) ;
	nimage[ BITMAP ] = mkpsptr( nvec ) ;
						/* copy the old vector of bitmaps to initialise the new */
	for( i = ( psint ) 0 ; i < ( psint ) 3 + nplanes ; i++ ) nvec[ i ] = ovec[ i ] ;

						/* calculate the source rectangle */
	bmap = mkptr( ovec[ 3 ] ) ;
	sy = bmap[ DIM_Y ] - oimage[ Y_OFFSET ] - ydim ;
	sx = oimage[ X_OFFSET ] ;


					/* make new vectors for the planes - if they exist */
					/* the screen should have 1 plane - vec entry is 0 */
	for( i = ( psint ) 0 ; i < nplanes ; i++ )
	{
		psint *vec1,*vec2,*sbits,*dbits,slincD,slincS ;

		vec2 = c_v_ib( -3,vecsz ) ;	/* create the new bitmap */

					/* find the original plane */
		vec1 = mkptr( ovec[ i + 3 ] ) ;

					/* fill in ptr to new vec from ne vec of bmaps */
		nvec[ i + 3 ] = mkpsptr( vec2 ) ;
		vec2[ DIM_X ] = xdim ;
		vec2[ DIM_Y ] = ydim ;
		vec2[ OFFSET ] = ( psint ) 7 ;
		vec2[ NO_LINES ] = ydim ;

					/* complete source and dest bitmaps */
		slincS = bitchk( vec1 ) / 4 ;
		sbits = vec1 + vec1[ OFFSET ] ;

		slincD = bitchk( vec2 ) / 4 ;
		dbits = vec2 + vec2[ OFFSET ] ;

						/* mark destination plane as being updated */
		/* now do rasterop store to store */
		D_raster_op( dbits,slincD,0,0,xdim,ydim,( int ) SAMROP_SRC,sbits,slincS,sx,sy ) ;

	}
					/* pop closure off stack - new image will then be left as the top element */
}


colof()						/* given pixel what is the colour map index  */
{
	psint pix ;

	pix = *sp & B_0_23 ;		/* ignore the depth info */

						/* return -1 if pix isnt a legal index */
	if ( cmapsize < pix ) *sp = ( psint ) -1 ; else
	{
							/* reload the colourmap values */
		*sp = ( psint )( red[ pix ] << 16 | green[ pix ] << 8 | blue[ pix ] ) ;
	}
}


void colmap()
{
	psint entry,pixel,slw,Ydim,Zdim ;
	unsigned char rpix,gpix,bpix ;

	entry = *sp++ ;
	pixel = *sp++ ;

	pixel &= ( cmapsize - ( psint ) 1 ) ;

	bpix = ( unsigned char )( entry & B_0_7 ) ;
	entry >>= ( psint ) 8 ;
	gpix = ( unsigned char )( entry & B_0_7 ) ;
	entry >>= ( psint ) 8 ;
	rpix = ( unsigned char )( entry & B_0_7 ) ;

	if ( cmapsize == ( psint ) 2 )			/* monochrome case is special !!! */
	{
		if ( pixel == 0 )			/* setup the two alternate values */
		{
			gpix = rpix ? -1 : 0 ;
			bpix = rpix ? -1 : 0 ;
			rpix = rpix ? -1 : 0 ;
		} else
		{
			gpix = rpix ? 0 : -1 ;
			bpix = rpix ? 0 : -1 ;
			rpix = rpix ? 0 : -1 ;
		}
		if ( *BYTEORDER(red) != rpix )		/* if red[0] has changed - do the change */
		{
			*BYTEORDER(red) = rpix ; *BYTEORDER(red+1) = ~rpix ;
			*BYTEORDER(green) = gpix ; *BYTEORDER(green+1) = ~gpix ;
			*BYTEORDER(blue) = bpix ; *BYTEORDER(blue+1) = ~bpix ;

			if ( D_refreshCmap( Xscreen,pixel,*BYTEORDER(red+pixel),*BYTEORDER(green+pixel),*BYTEORDER(blue+pixel) ) != 0 )
			{
				D_closeWindow( Xscreen ) ;
				return ;
			}
		}
	} else
	{
		if ( *BYTEORDER(red+pixel) != rpix || *BYTEORDER(green+pixel) != gpix || *BYTEORDER(blue+pixel) != bpix )
		{
			*BYTEORDER(red+pixel) = rpix ;
			*BYTEORDER(green+pixel) = gpix;
			*BYTEORDER(blue+pixel) = bpix ;

			if ( D_refreshCmap( Xscreen,pixel,*BYTEORDER(red+pixel),*BYTEORDER(green+pixel),*BYTEORDER(blue+pixel) ) != 0 )
			{
				D_closeWindow( Xscreen ) ;
				return ;
			}
		}
	}
}

reset_cmap()	    /* will restore the colour map on program exit .... */
{
/*
	if ( scr_pr != ( struct pixrect * ) 0 ) pr_putcolormap( scr_pr,0,cmapsize,svred,svgreen,svblue ) ;
*/
}


locator()	/* read mouse position & buttons ( file -> pntr ) */
{
	psint *res,lbut1,lbut2,lbut3,xp,yp,status ;

	res = c_structure( MOUSE_STRC ) ;	/* global loc struct */
	*--psp = mkpsptr( res ) ;		/* since stand func skip closure */

	curlive = SFALSE ;			/* disable cursor */
	if ( Xscreen != ( psint ) -1 )
	{
        	D_locatorReply reply ;


/*
        	D_locatorOne( Xscreen,&reply ) ;
*/
        	status = D_locatorI( Xscreen,&reply ) ;

		if ( status != ( psint ) 0 )
		{
			D_closeWindow( Xscreen ) ;
			screen = storeBase ;
			return ;
		}
		
        	while ( reply.dateStamp == dateStamp )
        	{
			status = D_locatorI( Xscreen,&reply ) ;

			if ( status != ( psint ) 0 )
			{
				D_closeWindow( Xscreen ) ;
				screen = storeBase ;
				return ;
			}
		}

		xp = reply.xpos ;
        	yp = reply.ypos ;
        	lbut1 = reply.but1 ;
        	lbut2 = reply.but2 ;
        	lbut3 = reply.but3 ;
		dateStamp = reply.dateStamp ;


		if ( reply.dateStamp != -1 )
		{
			/* Only update if a change ? */
			but1 = lbut1 ;				/* state of mouse buttons */
			but2 = lbut2 ;
			but3 = lbut3 ;
			curlive = STRUE ;			/* kick cursor again */

			xpos = xp ;                             /* x pos of mouse */
			ypos = Yscr - yp - ( psint ) 1 ;        /* inverted y pos */
		}

		res[ 1 ] = ( psint ) 0 ;	/* vec of button values */
		res[ 2 ] = xpos ;			/* x - window relative position */
		res[ 3 ] = ypos ;			/* inverted y pos */
		res[ 4 ] = STRUE ;		/* window selected ? - dummy value */

		res = c_v_ib( 1,3 ) ;				/* create vec of bools for the buttons */
		res[ 3 ] = but1 ;
		res[ 4 ] = but2 ;
		res[ 5 ] = but3 ;

	}
	( mkptr( *psp ) )[ 1 ] = mkpsptr( res ) ;		/* fill in ptr to vec of bools from mouse strc */
}

/* lcode and lclip is the clipping algorithm found in Newman & Sproull */

psint lcode( x,y,cx1,cx2,cy1,cy2 )	/* code up clipping area */
psint x,y,cx1,cx2,cy1,cy2 ;
{
	psint res = ( psint ) 0 ;

	if ( x < cx1 ) res |= ( psint ) 8 ; else
	if ( x > cx2 ) res |= ( psint ) 4 ;
	if ( y < cy1 ) res |= ( psint ) 2 ; else
	if ( y > cy2 ) res |= ( psint ) 1 ;
	return( res ) ;
}

psint lclip( x1,y1,x2,y2,cx1,cx2,cy1,cy2 )	/* do the clipping */
psint *x1,*y1,*x2,*y2,cx1,cx2,cy1,cy2 ;
{
	psint c1,c2 ;

	c1 = lcode( *x1,*y1,cx1,cx2,cy1,cy2 ) ;
	c2 = lcode( *x2,*y2,cx1,cx2,cy1,cy2 ) ;
	while ( ( c1 || c2 ) && !( c1 & c2 ) )
	{
		psint c,x,y ;

		c = ( c1 ? c1 : c2 ) ;
		if ( c & ( psint ) 8 )
		{
			x = cx1 ;
			y = *y1 + ( *y2 - *y1 ) * ( cx1 - *x1 ) / ( *x2 - *x1 ) ;
		} else
		if ( c & ( psint ) 4 )
		{
			x = cx2 ;
			y = *y1 + ( *y2 - *y1 ) * ( cx2 - *x1 ) / ( *x2 - *x1 ) ;
		} else
		if ( c & ( psint ) 2 )
		{
			y = cy1 ;
			x = *x1 + ( *x2 - *x1 ) * ( cy1 - *y1 ) / ( *y2 - *y1 ) ;
		} else
		{
			y = cy2 ;
			x = *x1 + ( *x2 - *x1 ) * ( cy2 - *y1 ) / ( *y2 - *y1 ) ;
		}
		if ( c1 )
		{
			*x1 = x ;
			*y1 = y ;
			c1 = lcode( x,y,cx1,cx2,cy1,cy2 ) ;
		} else
		{
			*x2 = x ;
			*y2 = y ;
			c2 = lcode( x,y,cx1,cx2,cy1,cy2 ) ;
		}
	}
	return( c1 & c2 ) ;
}

pnxlne() /* standard PNX drawline on bitmap or window, no last point */
{
	psint x1,x2,y1,y2,col,Xdim,Ydim,a_cursor,nplanes,pixZdim ;
	psint *wdes_d,*wst,*ptr ;
        extern void check_startstop() ;
	psint rrule ;

	wst = mkptr( *psp++ ) ;
	a_cursor = SFALSE ;
	if ( wst[ WINDOW ] )			/* a window therefore only 1 plane - mustbe the screen */
	{
		wdes_d = mkptr( wst[ WINDOW ] ) ;
		if ( !( wdes_d[ FLAGS ] & CLOSED_FLAG ) && ( wdes_d[ FLAGS ] & WP_FLAG ) ) f_lush( wdes_d ) ;
                check_startstop() ;             /* do this first or the display may get out of step */
	}

	wdes_d = mkptr( wst[ BITMAP ] ) ;
	a_cursor = ( wdes_d == mkptr( cursor[ BITMAP ] ) ) ;	/* is it the cursor */
	nplanes = wdes_d[ UPB ] ;						/* how many planes */
	pixZdim = nplanes ;
	ptr = mkptr( wdes_d[ 3 ] ) ;
	Xdim = ptr[ DIM_X ] ;
	Ydim = ptr[ DIM_Y ] ;

	rrule = *sp++ ; 		/* The style of drawing */
	col = *sp++ & B_0_23 ; 		/* The pixel colour */
	y2  = *sp++ + wst[ Y_OFFSET ] ;	/* normalise image -	*/
	x2  = *sp++ + wst[ X_OFFSET ] ;	/* back to 0,0		*/
	y1  = *sp++ + wst[ Y_OFFSET ] ;
	x1  = *sp++ + wst[ X_OFFSET ] ;

		/* line clipping algorithm to keep coords on area */
		/* give up if definitely nothing to draw */
	/*printf( "col is %d ; x1,x2,y1,y2 are .. %d %d %d %d\n",col,x1,x2,y1,y2 ) ; */
	if ( lclip( &x1,&y1,&x2,&y2,wst[ X_OFFSET ],
		    wst[ X_DIM ] + wst[ X_OFFSET ] - ( psint ) 1,
		    wst[ Y_OFFSET ],wst[ Y_DIM ] + wst[ Y_OFFSET ] - ( psint ) 1 ) )
		return ;

	y1 = Ydim - y1 - ( psint ) 1 ;
	y2 = Ydim - y2 - ( psint ) 1 ;

        switch( rrule )
        {
        	case 0:         rrule = SAMROP_SET ;
                        	break ;
        	case 1:         rrule = SAMROP_CLR ;
                        	break ;
        	case 2:
        	default:        rrule = SAMROP_NOT( SAMROP_DST ) ;
        }

	while ( nplanes-- > ( psint ) 0 )			/* do draw.line on every plane */
	{
		psint *ptr,tmp,slinc ;
		psint *dst_bits ;

		ptr = mkptr( wdes_d[ pixZdim - nplanes - 1 + 3 ] ) ;

		slinc = bitchk( ptr ) / 4  ;
		dst_bits = ptr + ptr[ OFFSET ] ;
/*
		tmp = col & ( 1 << ( int )( nplanes ) ) ? -1 : 0 ;
*/
		tmp = col & ( 1 << ( int )( pixZdim - nplanes - (psint) 1 ) ) ? -1 : 0 ;
		D_raster_vector( dst_bits,slinc,x1,y1,x2,y2,rrule,
				 tmp ) ;
/*
				 col & ( 1 << ( int )( pixZdim - nplanes - (psint) 1 ) ) ? -1 : 0 ) ;
*/
	}

	if ( wst[ WINDOW ] )				/* if its a draw the vector on the screen */
	{
		psint start,*screenbits,ptr ;

		nplanes = wdes_d[ UPB ] ;			/* how many planes were there ? */
		ptr = wdes_d[ 3 ] ;

		start = 0 ;
		screenbits = mkptr( screen[ BITMAP ] ) ;
		while( start < Zscr && ptr != screenbits[ start + ( psint ) 3 ] ) start++ ;

			if ( D_refreshLine( Xscreen,x1,y1,x2,y2,start,Zscr + start - 1,
/*
		if ( D_refreshLine( Xscreen,x1,y1,x2,y2,2,nplanes - 3,
*/
				    convertToXrule( rrule ),col ) != 0 )
		{
			D_closeWindow( Xscreen ) ;
			Xscreen = ( psint ) -1 ;
    		} 
	}
	if ( a_cursor )		/* if the destination is the cursor */
	{			/*   must tell o.s it has changed   */
/*
		psint *ptr ;

				/* find ptr to image and check alignment *
		ptr = mkptr( wdes_d[ 3 ] ) ;
		bitchk( ptr ) ;
		ChangeCursor( ptr ) ;
*/
		ShowCursor( 1,1 ) ;
	}
}

curstip()
{
}

setcursor()
{
}

rastop( ropfn )	/* rasterop for bitmaps and/or windows */
psint ropfn ;
{
	psint *wdes_s,*wst_s,*wdes_d,*wst_d,a_cursor ;
	psint *ptr,next_bm,nplanes,overlap ;
	psint SXdim,SYdim,DXdim,DYdim,wd_s,wd_d ;
	psint width,height,sx,sy,dx,dy,src_is_dst,src_eq_dst ;
	psint *plane1,*plane2 ;
	psint *sbits,*dbits ;
	psint slincS,slincD ;
	extern void check_startstop() ;

	wst_d = mkptr( *psp ) ;				/* destination image desc */
	wst_s = mkptr( psp[ 1 ] ) ;			/* source image desc */
	psp += 2 ;					/* pop params from stack */


	/* first find out how big source is */

	if ( wst_s[ WINDOW ] )					/* if a file */
	{
		wdes_s = mkptr( wst_s[ WINDOW ] ) ;		/* get the window - ps file */
		wd_s = wdes_s[ FLAGS ] ;			/* window descriptor src - unix file */
								/* if write pending */
		if ( !( wd_s & CLOSED_FLAG ) && ( wd_s & WP_FLAG ) ) f_lush( wdes_s ) ;
	}
	wdes_s = mkptr( wst_s[ BITMAP ] ) ;		/* get the vector of bitmaps */
	ptr = mkptr( wdes_s[ 3 ] ) ;		/* get the first plane */
			/* find start of src bitmap - the actual bits  */
	SXdim = ptr[ DIM_X ] ;				/* src X dimension */
	SYdim = ptr[ DIM_Y ] ;				/* src Y dimension */

	/* next find out how big destination is */

	if ( wst_d[ WINDOW ] )					/* if a window file */
	{
		wdes_d = mkptr( wst_d[ WINDOW ] ) ;	/* get the actual file */
		wd_d = wdes_d[ FLAGS ] ;		/* next few lines as above */
		if ( !( wd_d & CLOSED_FLAG ) && ( wd_d & WP_FLAG ) ) f_lush( wdes_d ) ;
		check_startstop() ;			     /* do this first so the display keeps in step */
	}
	wdes_d = mkptr( wst_d[ BITMAP ] ) ;		/* get the actual bitmap */
	ptr = mkptr( wdes_d[ 3 ] ) ;		/* get 1st plane */

	DXdim = ptr[ DIM_X ] ;				/* destination X dimension */
	DYdim = ptr[ DIM_Y ] ;				/* destination Y dimension */
	a_cursor = ( wdes_d == mkptr( cursor[ BITMAP ] ) ) ;

	/* next get narrower of the two bitmaps */

	width = ( wst_s[ X_DIM ] < wst_d[ X_DIM ] ) ? wst_s[ X_DIM ] : wst_d[ X_DIM ] ;

	/* next get the bitmap with the smallest height */

	height = ( wst_s[ Y_DIM ] < wst_d[ Y_DIM ] ) ? wst_s[ Y_DIM ] : wst_d[ Y_DIM ] ;

	/* offsets for source and destination */

	sx = wst_s[ X_OFFSET ] ;
	sy = SYdim - wst_s[ Y_OFFSET ] - height ;
	dx = wst_d[ X_OFFSET ] ;
	dy = DYdim - wst_d[ Y_OFFSET ] - height ;

	/* check if source and destination are the same */
	/* do by comparing the first bit plane */
	wdes_s = mkptr( wst_s[ BITMAP ] ) ;
	wdes_d = mkptr( wst_d[ BITMAP ] ) ;

	src_eq_dst = ( wdes_s[ 3 ] == wdes_d[ 3 ] ) ? STRUE : SFALSE ;
	src_is_dst = ( src_eq_dst && dx == sx && dy == sy ) ? STRUE : SFALSE ;
	if ( src_is_dst )
	{
		switch( ropfn )
		{
		case 0: return ;
		case 1:	ropfn = SAMROP_NOT( SAMROP_DST ) ;
			break ;
		case 2:	return ;
		case 3:	ropfn = SAMROP_NOT( SAMROP_DST ) ;
			break ;
		case 4:	return ;
		case 5:	ropfn = SAMROP_NOT( SAMROP_DST ) ;
			break ;
		case 6:	ropfn = SAMROP_DST & SAMROP_NOT( SAMROP_DST ) ;
			break ;
		case 7:	ropfn = SAMROP_DST | SAMROP_NOT( SAMROP_DST ) ;
			break ;
		default:
			return ;
		}
	} else
	{
		switch( ropfn )
		{
		case 0:	ropfn = SAMROP_SRC ;
				break ;
		case 1:	ropfn = SAMROP_NOT( SAMROP_SRC ) ;
				break ;
		case 2:	ropfn = SAMROP_SRC & SAMROP_DST ;
				break ;
		case 3:	ropfn = SAMROP_NOT( SAMROP_SRC & SAMROP_DST ) ;
				break ;
		case 4:	ropfn = SAMROP_SRC | SAMROP_DST ;
				break ;
		case 5:	ropfn = SAMROP_NOT( SAMROP_SRC | SAMROP_DST ) ;
				break ;
		case 6:	ropfn = SAMROP_SRC ^ SAMROP_DST ;
				break ;
		case 7:	ropfn = SAMROP_NOT( SAMROP_SRC ^ SAMROP_DST ) ;
				break ;
		default:
			return ;
		}
	}

						/* index into vector of planes */
						/* test for over lapping planes and do in correct order */
						/* overlap set if rop must be done in order from 1st to last */
	nplanes = wdes_s[ UPB ] < wdes_d[ UPB ] ? wdes_s[ UPB ] : wdes_d[ UPB ] ;
	next_bm = ( psint ) 3 ;		 /* next bmap to look at */
	ptr = mkptr( wdes_s[ 3 ] ) ;      /* the first source plane */
	overlap = SFALSE ;		     /* flag to detect overlap */
	while( nplanes-- > ( psint ) 0 && !overlap )
	{				       /* search for the 1st source plane in the destination */
		if ( ptr == mkptr( wdes_d[ next_bm ] ) ) overlap = STRUE ;
		next_bm++ ;
	}

						/* do the rop allowing for ovelapping planes */
	nplanes = wdes_s[ UPB ] < wdes_d[ UPB ] ? wdes_s[ UPB ] : wdes_d[ UPB ] ;
						/* if planes overlap start at beginning */
	next_bm = overlap ? ( psint ) 3 : ( nplanes + ( psint ) 2 ) ;
	next_bm = 3 ;
	while( nplanes-- > ( psint ) 0 )
	{

						/* get address and scanl inc for src and dst planes */
		plane1 = mkptr( wdes_s[ next_bm ] ) ;
		slincS = bitchk( plane1 ) / 4 ;
		sbits = plane1 + plane1[ OFFSET ] ;

		plane2 = mkptr( wdes_d[ next_bm ] ) ;
		slincD = bitchk( plane2 ) / 4 ;
		dbits = plane2 + plane2[ OFFSET ] ;

						/* mark destination plane as being updated */
		/* now do rasterop store to store */
		if ( src_is_dst )
		D_raster_op( dbits,slincD,dx,dy,width,height,( int ) ropfn,sbits,slincS,sx,sy ) ;
		else D_raster_op( dbits,slincD,dx,dy,width,height,( int ) ropfn,sbits,slincS,sx,sy ) ;

		if ( overlap )
		{
			next_bm++ ;	     /* inc next_bm */
		} else
		{
			next_bm++ ;	     /* dec next_bm */
		}
	}

	if ( wst_d[ WINDOW ] )
	{					/* refresh the altered part of the screen */
		psint start,*screenbits ;
		extern void check_startstop() ;

		nplanes = wdes_s[ UPB ] < wdes_d[ UPB ] ? wdes_s[ UPB ] : wdes_d[ UPB ] ;
		ptr = mkptr( wdes_d[ 3 ] ) ;

		start = 0 ;
		screenbits = mkptr( screen[ BITMAP ] ) ;
		while( start < Zscr &&  ptr != mkptr( screenbits[ start + 3 ] ) ) start++ ;

		if ( start + nplanes <= Zscr )
		{
			if ( Xscreen != -1 && src_eq_dst )
			{
				if ( D_refreshCopy( Xscreen,sx,sy,start,width,height,nplanes,dx,dy,start,convertToXrule( ropfn ) ) != 0 )
				{
					D_closeWindow( Xscreen ) ;
					Xscreen = ( psint ) -1 ;
				}

			} else
			{
				switch( ropfn )
				{
				case SAMROP_SET:
				case SAMROP_CLR:
				case SAMROP_NOT( SAMROP_DST ):
				case SAMROP_DST:
						break ;
				default:	ropfn = SAMROP_SRC ;
				}

				if ( D_refreshRect( Xscreen,dx,dx + width - 1,dy,dy + height - 1,start,start + nplanes - 1,convertToXrule( ropfn ) ) != 0 )
				{
					D_closeWindow( Xscreen ) ;
					Xscreen = ( psint ) -1 ;
				}
			}
		}
	}

	if ( a_cursor )						/* if the destination is the cursor */
	{							/*   must tell o.s it has changed   */
		int *ptr ;

/*
		ptr = mkptr( wdes_d[ 3 ] ) ;
		bitchk( ptr ) ;
		ChangeCursor( ptr ) ;
*/
		ShowCursor( 1,1 ) ;
	}
}

static int convertToXrule( int rule )
{
	rule &= SAMROP_NOT( 0 ) ;			       /* convert pixrect rule into just the op */
	switch( rule )
	{
	case SAMROP_CLR:	return( 0x0 ) ;		 /* clear destination pixels */
	case SAMROP_SRC & SAMROP_DST:			   /* and */
				return( 0x1 ) ;
	case SAMROP_SRC & SAMROP_NOT( SAMROP_DST ):	     /* and not */
				return( 0x2 ) ;
	case SAMROP_SRC:	return( 0x3 ) ;		 /* copy */
	case SAMROP_NOT( SAMROP_SRC ) & SAMROP_DST:	     /* not and */
				return( 0x4 ) ;
	case SAMROP_DST:	return( 0x5 ) ;		 /* no-op */
	case SAMROP_SRC ^ SAMROP_DST:			   /* xor */
				return( 0x6 ) ;
	case SAMROP_SRC | SAMROP_DST:			   /* or */
				return( 0x7 ) ;
	case SAMROP_NOT( SAMROP_SRC ) & SAMROP_NOT( SAMROP_DST ):       /* not and not */
				return( 0x8 ) ;
	case SAMROP_NOT( SAMROP_SRC ) ^ SAMROP_DST:	     /* not xor */
				return( 0x9 ) ;
	case SAMROP_NOT( SAMROP_DST ):
				return( 0xa ) ;		 /* invert destination pixels */
	case SAMROP_SRC | SAMROP_NOT( SAMROP_DST ):	     /* or not */
				return( 0xb ) ;
	case SAMROP_NOT( SAMROP_SRC ):		  /* not */
				return( 0xc ) ;
	case SAMROP_NOT( SAMROP_SRC ) | SAMROP_DST:	     /* not or */
				return( 0xd ) ;
	case SAMROP_NOT( SAMROP_SRC ) | SAMROP_NOT( SAMROP_DST ):       /* not or not */
				return( 0xe ) ;
	case SAMROP_SET:	return( 0xf ) ;		 /* set destination pixels */
	default:		return( 0x5 ) ;		 /* no-op if things dont make sense */
	}
}

void	check_startstop()
{
}

init_pstty()
{
}

reset_input()
{
}

init_scr_X( maxZscr,defColourMap )
{
	char	*dname ;
	int	status	;

	D_initDisplay() ;

	stop_itimer() ;
	dname = ( char * ) 0 ;
	Xscreen = D_openWindow( "",maxZscr,&Xscr,&Yscr,&Zscr ) ;
	start_itimer() ;
/*
	Xscr -= 100 ; Yscr -= 100 ;
*/
	Xscr = ( Xscr / 3 ) * 2 ;
	Yscr = ( Yscr / 3 ) * 2 ;
/*
	start_itimer() ;
*/
	if ( Xscreen != ( psint ) -1 ) init_Xcmap( defColourMap ) ;
/*
	stop_itimer() ;
*/
}

init_Xcmap( defCmap )
psint defCmap ;
{
        int i,status ;

        cmapsize = 1 ;
        for ( i = 0 ; i < Zscr ; i++ )
        {
                cmapsize *= 2 ;
        }

/*
        for ( i = 0 ; i < cmapsize ; i++ )
        {
                int Cnt ;

                if ( defCmap )
                {
                        Cnt = i & 1 ? 0 : 255 ;
                } else
                {
                        Cnt = i & 1 ? 255 : 0 ;
                }
                red[ i ] = ( unsigned char )( Cnt ) ;
                green[ i ] = ( unsigned char )( Cnt ) ;
                blue[ i ] = ( unsigned char )( Cnt ) ;
        }
        for ( i = 0 ; i < cmapsize ; i++ )
	{
                status = D_refreshCmap( Xscreen,i,red[ i ],green[ i ],blue[ i ] ) ;
	}
*/

	Ninit_Cmap( cmapsize ) ;
}


static setAppleColourMap( pixel )
psint	pixel ;
{
	psuint entry ;
	unsigned char rpix,gpix,bpix ;

	pixel &= ( cmapsize - ( psint ) 1 ) ;
	entry = appleColourMap[ pixel ] ; 

/*
	The old version !!
	bpix = ( unsigned char )( entry & B_0_7 ) ;
	entry >>= ( psint ) 8 ;
	gpix = ( unsigned char )( entry & B_0_7 ) ;
	entry >>= ( psint ) 8 ;
	rpix = ( unsigned char )( entry & B_0_7 ) ;
*/
	rpix = ( unsigned char )( entry & B_0_7 ) ;
	entry >>= ( psint ) 8 ;
	gpix = ( unsigned char )( entry & B_0_7 ) ;
	entry >>= ( psint ) 8 ;
	bpix = ( unsigned char )( entry & B_0_7 ) ;
	if ( cmapsize == ( psint ) 2 )			/* monochrome case is special !!! */
	{
		if ( pixel == 0 )			/* setup the two alternate values */
		{
			gpix = rpix ? -1 : 0 ;
			bpix = rpix ? -1 : 0 ;
			rpix = rpix ? -1 : 0 ;
		} else
		{
			gpix = rpix ? 0 : -1 ;
			bpix = rpix ? 0 : -1 ;
			rpix = rpix ? 0 : -1 ;
		}
		if ( *BYTEORDER(red) != rpix )		/* if red[0] has changed - do the change */
		{
			*BYTEORDER(red) = rpix ; *BYTEORDER(red+1) = ~rpix ;
			*BYTEORDER(green) = gpix ; *BYTEORDER(green+1) = ~gpix ;
			*BYTEORDER(blue) = bpix ; *BYTEORDER(blue+1) = ~bpix ;

			if ( D_refreshCmap( Xscreen,pixel,*BYTEORDER(red+pixel),*BYTEORDER(green+pixel),*BYTEORDER(blue+pixel) ) != 0 )
			{
				D_closeWindow( Xscreen ) ;
				return ;
			}
		}
	} else
	{
		if ( *BYTEORDER(red+pixel) != rpix || *BYTEORDER(green+pixel) != gpix || *BYTEORDER(blue+pixel) != bpix )
		{
			*BYTEORDER(red+pixel) = rpix ;
			*BYTEORDER(green+pixel) = gpix;
			*BYTEORDER(blue+pixel) = bpix ;

			if ( D_refreshCmap( Xscreen,pixel,*BYTEORDER(red+pixel),*BYTEORDER(green+pixel),*BYTEORDER(blue+pixel) ) != 0 )
			{
				D_closeWindow( Xscreen ) ;
				return ;
			}
		}
	}
}

static psint Ninit_Cmap( cmapsize )
{
	int i,stop ;					/* init all cells by query of display uninited display */
							/* cells set by us except foreground & background */

	*BYTEORDER(red) = *BYTEORDER(green) = *BYTEORDER(blue) = ( unsigned char ) 255 ;
	*BYTEORDER(red+1) = *BYTEORDER(green+1) = *BYTEORDER(blue+1) = ( unsigned char ) 0 ;
	if ( D_refreshCmap( Xscreen,0,*BYTEORDER(red),*BYTEORDER(green),*BYTEORDER(blue) ) != 0 ||
		D_refreshCmap( Xscreen,1,*BYTEORDER(red+1),*BYTEORDER(green+1),*BYTEORDER(blue+1) ) != 0 )
	{
		return( (psint) -1 ) ;
	}
	stop = cmapsize - 1 ;
	for ( i = 2 ; i < stop ; i++ )
	{
		setAppleColourMap( i ) ;
/*
	Try Dave's Apple pie map 
		if ( D_queryCmap( Xscreen,i,BYTEORDER(red+i),BYTEORDER(green+i),BYTEORDER(blue+i) ) != 0 )
		{
			return( (psint) -1 ) ;
		}
		if ( !*BYTEORDER(red+i) && !*BYTEORDER(green+i) && !*BYTEORDER(blue+i) )
		{
			switch( i % 8 )
			{
			case 0:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			case 1:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
				break ;
			case 2:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
				break ;
			case 3:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			case 4:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
				break ;
			case 5:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			case 6:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			default:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
			}
			if ( D_refreshCmap( Xscreen,i,*BYTEORDER(red+i),*BYTEORDER(green+i),*BYTEORDER(blue+i) ) != 0 )
			{
				return( (psint) -1 ) ;
			}
		}
*/
	}
	return( 0 ) ;
}


put_Xcolor( pixel )
psint pixel ;
{
	if ( Xscreen != -1 )
	D_refreshCmap( Xscreen,pixel,red[ pixel ],green[ pixel ],blue[ pixel ] ) ;
}

click_to_quit()
{
	D_locatorReply reply ;
	psint status,more ;

	D_renameWindow( Xscreen,CLICKTOQUIT ) ;
	D_flushWait( Xscreen ) ;

	more = STRUE ;
	while ( more )
	{
		status = D_locatorI( Xscreen,&reply ) ;
		if ( status != ( psint ) 0 ) more = SFALSE ; else
		if ( reply.dateStamp != (  psint ) -1 )
		more = !reply.but1 && !reply.but2 && !reply.but3 && !reply.but4 && !reply.but5 ;
	}
	status = D_renameWindow( Xscreen,defaultWindowname ) ;

}
#endif	RASTOP
