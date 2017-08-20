#include "int.h"
#include "graphics.h"
#include "../display/comms.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/filio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <pixrect/pixrect_hs.h>

extern int	displayConnected ;		/* is the display server still connected ? */
extern int	errno ;

#define	MAXLHEAPS	16			/* the maximum number of concurrently active local heaps */
#define	BUFFERSIZE	16384
#define	MINLHEAPSIZE	96			/* assume we need at least 1.5Meg (96*16K) if using graphics */

int lheap_fd ;
char lheap_fname[ MAXFNAME ] ;

void truncate_lheap( status,arg )
int status ; caddr_t arg ;
{
	if ( lheap_fd != -1 )
	{
		ftruncate( lheap_fd,MINLHEAPSIZE * BUFFERSIZE ) ;
		close( lheap_fd ) ;
	}
}

void zap_lheap( status,arg )
int status ; caddr_t arg ;
{
	if ( lheap_fd != -1 )
	{
		ftruncate( lheap_fd,0 ) ;
		close( lheap_fd ) ;
		unlink( lheap_fname ) ;	
	}
}

caddr_t alloc_shared_memory( size )
int size ;
{
	int i,start,attempts,created,mask ;
	char buffer[ BUFFERSIZE ] ;				/* big buffer for creating the target file */
	caddr_t addr ;

	lheap_fd = -1 ;
	attempts = 0 ;
	created = SFALSE ;
	while( lheap_fd == -1 && attempts < MAXLHEAPS )
	{
		mask = umask( 0 ) ;						/* so global rw access may be set */
		lheap_fd = open( lheap_fname,O_CREAT | O_EXCL | O_RDWR,0666 ) ;	/* create the file if it didnt exist - give rw for all */
		mask = umask( mask ) ;						/* restore the umask */
		if ( lheap_fd >= 0 ) created = STRUE ; else
		{
			lheap_fd = open( lheap_fname,O_RDWR ) ;
		}
		if ( lheap_fd == -1 )
		{			/* the open failed - if its a serious problem give up */
			if ( errno != EINTR ) return( ( caddr_t ) -1 ) ;
		} else
		{								/* we opened the file so lock it */
			if ( flock( lheap_fd,LOCK_EX | LOCK_NB ) )
			{							/* the file is locked by another process */
				close( lheap_fd ) ;
				lheap_fd = -1 ;
			} else
			{
				struct stat A,B ;

				if ( stat( lheap_fname,&A ) || fstat( lheap_fd,&B ) || A.st_ino != B.st_ino )
				{						/* the new file was removed between opening and being locked */
					close( lheap_fd ) ;
					lheap_fd = -1 ;
				} else
				{
										/* everything appears OK so plant an unlink on exit proc */
					if ( attempts < 2 )			/* keep a couple of files for reuse */
					{
						on_exit( truncate_lheap,( caddr_t ) 0 ) ;	/* truncate_lheap will give a 1meg file */
					} else
					{
						on_exit( zap_lheap,( caddr_t ) 0 ) ;	/* zap_lheap will unlink the lheap file */
					}
				}
			}
		}
		if ( lheap_fd == -1 )
		{
			strcat( lheap_fname,"." ) ;				/* extend the filename for the next loop */
			attempts++ ;						/* try another file */
			created = SFALSE ;					/* reset created flag just in case */
		}
	}
	if ( lheap_fd == -1 ) return( ( caddr_t ) -1 ) ;			/* give up if we cant make a local heap */

	size = ( size + BUFFERSIZE - ( psint ) 1 ) / BUFFERSIZE ;		/* a whole number of 16K pages - rounded up */
	if ( size < MINLHEAPSIZE ) size = MINLHEAPSIZE ;			/* at least 1.5Meg worth */
	start = 0 ;
	if ( !created )								/* if we are reusing a file the 1st 1.5Meg was written */
	{
		start += MINLHEAPSIZE * BUFFERSIZE ;
		if ( start != lseek( lheap_fd,start,0 ) )
		{
			close( lheap_fd ) ;
			lheap_fd = -1 ;
			return( ( caddr_t ) -1 ) ;				/* give up if we cant make a local heap */
		}
	}
	for ( i = start / BUFFERSIZE ; i < size ; i++ )
	{
		write( lheap_fd,buffer,BUFFERSIZE ) ;		/* writes to disk files cannot be interrupted! */
	}
	return( mmap( ( caddr_t ) 0x800000,size * BUFFERSIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_FIXED,lheap_fd,( off_t ) 0 ) ) ;
}

char *get_lheap( size )
int size ;
{
	caddr_t addr ;
	extern char *malloc() ;

	strcpy( lheap_fname,"/usr/lib/S/tmp/lheap" ) ;
	addr = alloc_shared_memory( size ) ;

	if ( ( int ) addr <= 0 )
	{
		close_Xscreen() ;
		return( malloc( size ) ) ;
	} else
	{
		return( ( char * ) addr ) ;
	}
}

psint		Xscreen = -1 ;				/* which screen is in use => -1 if none in use */
extern psint	Xscr,Yscr,Zscr ;			/* dimensions of window */
extern psint	cmapsize,realZscr ;
extern psint	xpos,ypos,but1,but2,but3 ;		/* locator info... */
extern psint	mouse_live ;				/* is the mouse alive? */
extern psint	curx,cury ;				/* the cursor's intended hot spot */
extern short	curs_init[] ;				/* cursor's bitmap */
static char	savedName[ MAXFNAME ] ;			/* save the windows name so it can be restored */
extern unsigned char red[],green[],blue[] ;		/* copy of the desired colour map */

#define	CLICKTOQUIT	"Click-quit"			/* click to quit message */
#define	RUNNING		"S-algol"			/* running message */
#define	XTERM		"xterm"

int reverse_bits( i,b )					/* reverse 1st b bits of i */
int i,b ;
{
	int j,k ;

	k = 0 ;
	for ( j = 0 ; j < b ; j++ )
	{
		if ( i & ( 1 << j ) ) k |= 1 << ( b - 1 - j ) ;
	}
	return( k ) ;
}

init_Xcmap( defCmap )
psint defCmap ;
{
	int i ;

	cmapsize = 1 ;
	for ( i = 0 ; i < Zscr ; i++ )
	{
		cmapsize *= 2 ;
	}

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
		D_refreshCmap( Xscreen,i,red[ i ],green[ i ],blue[ i ] ) ;
}

static int scaleDown( dim )		/* scale down the dimensions by 2 */
int dim ;
{
	if ( dim < 100 ) return( dim ) ;		/* unless its already small */
	if ( dim > 200 ) return( dim / 2 ) ;		/* if its bit enough OK */
	return( 100 ) ;					/* anything in between - just use 100 */
}

init_scr_X( maxZscr,cmap )
int maxZscr,cmap ;
{
	openReply reply ;

	cmap = !cmap ;
	D_initDisplayServer( "/usr/lib/S/exec/sun4_display" ) ;
	Xscreen = D_firstWindow( maxZscr,&reply ) ;
	if ( Xscreen == -1 )
	{
		char *displayName ;
		extern char *getenv() ;

		displayName = getenv( "DISPLAY" ) ;
		if ( displayName != ( char * ) 0 )
		{
			Xscreen = D_openWindow( displayName,maxZscr,&reply ) ;
			reply.maxX = scaleDown( reply.maxX ) ;
			reply.maxY = scaleDown( reply.maxY ) ;
		}
	}
	if ( Xscreen != -1 )
	{
		Xscr = reply.maxX ;
		Yscr = reply.maxY ;
		realZscr = reply.maxZ ;
		Zscr = realZscr > maxZscr ? maxZscr : realZscr ;

		init_Xcmap( cmap ) ;		/* setup the local version of whats supposed to be in the colour map - then fix it */
	}
}

create_XImage( screen )
psint *screen ;
{
	psint i,*scr,*scrbm ;
	psint offsets[ MAXOFFSETS ] ;

	scr = ( psint * )( screen[ BITMAP ] ) ;
	for ( i = ( psint ) 0 ; i < Zscr ; i++ )
	{
		scrbm = ( psint * )( scr[ ( psint ) 3 + i ] ) ;
		offsets[ i ] = ( char * )( scrbm + scrbm[ OFFSET ] ) - ( char * ) store ;
	}

	D_setScreen( Xscreen,lheap_fname,Xscr,Yscr,Zscr,bitchk( scrbm ),BITMAPSFORMAT,offsets ) ;
	D_readWindowName( Xscreen,savedName ) ;		/* save the windows name for later */
	if ( !strcmp( savedName,RUNNING ) ||		/* did we have a disaster last time we made a window */
	     !strcmp( savedName,CLICKTOQUIT ) )	
	{
		strcpy( savedName,XTERM ) ;
	}
	D_renameWindow( Xscreen,RUNNING ) ;		/* indicate that the program is running */
}

freeze_XScreen()
{
	if ( Xscreen != ( psint ) -1 )
	{
		D_freeze( Xscreen ) ;
	}
}

moved_XImage( screen )
psint *screen ;
{
	psint i,*scr,*scrbm ;
	psint offsets[ MAXOFFSETS ] ;

	scr = ( psint * )( screen[ BITMAP ] ) ;
	for ( i = ( psint ) 0 ; i < Zscr ; i++ )
	{
		scrbm = ( psint * )( scr[ ( psint ) 3 + i ] ) ;
		offsets[ i ] = ( char * )( scrbm + scrbm[ OFFSET ] ) - ( char * ) store ;
	}

	D_changeOffsets( Xscreen,Xscr,Yscr,Zscr,bitchk( scrbm ),BITMAPSFORMAT,offsets ) ;
}

close_Xscreen()
{
	if ( Xscreen != -1 )
	{
		D_closeWindow( Xscreen ) ;
		Xscreen = -1 ;
		Xscr = 1 ;
		Yscr = 1 ;
		Zscr = 1 ;
		cmapsize = 0 ;
	}
}

quit_Xscreen()
{
	locatorReply reply ;
	flushWaitRq waitFor ;

	D_resetServer() ;
	D_renameWindow( Xscreen,CLICKTOQUIT ) ;
	D_flushWait( Xscreen,&waitFor ) ;
	D_locatorOne( Xscreen,&reply ) ;
	while( displayConnected && !reply.but1 && !reply.but2 && !reply.but3 && !reply.but4 && !reply.but5 )
		D_locatorOne( Xscreen,&reply ) ;
	D_renameWindow( Xscreen,savedName ) ;
	close_Xscreen() ;
}

set_Xcursor( x,y )
int x,y ;
{
	int i ;
	setCursorRq info ;

	if ( Xscreen != ( psint ) -1 )
	{
		info.Xhot = x ;
		info.Yhot = y ;
		for ( i = 0 ; i < sizeof( info.bits ) ; i++ )
		{
			info.bits[ i ] = ( ( char * ) curs_init )[ i ] ;
		}
		D_setCursor( Xscreen,&info ) ;
	}
}

change_Xcursor()
{
	int i ;
	setCursorRq info ;

	if ( Xscreen != ( psint ) -1 )
	{
		info.Xhot = curx ;
		info.Yhot = cury ;
		for ( i = 0 ; i < sizeof( info.bits ) ; i++ )
		{
			info.bits[ i ] = ( ( char * ) curs_init )[ i ] ;
		}
		D_setCursor( Xscreen,&info ) ;
	}
}

show_Xcursor()
{
	if ( Xscreen != ( psint ) -1 )
	{
		D_showCursor( Xscreen ) ;
	}
}

hide_Xcursor()
{
	if ( Xscreen != ( psint ) -1 )
	{
		D_hideCursor( Xscreen ) ;
	}
}

int convert_to_Xrule( rule )
int rule ;
{
	rule &= PIX_NOT( 0 ) ;				/* convert pixrect rule into just the op */
	switch( rule )
	{
	case PIX_CLR:	return( 0x0 ) ;			/* clear destination pixels */
	case PIX_SRC & PIX_DST:				/* and */
			return( 0x1 ) ;
	case PIX_SRC & PIX_NOT( PIX_DST ):		/* and not */
			return( 0x2 ) ;
	case PIX_SRC:	return( 0x3 ) ;			/* copy */
	case PIX_NOT( PIX_SRC ) & PIX_DST:		/* not and */
			return( 0x4 ) ;
	case PIX_DST:	return( 0x5 ) ;			/* no-op */
	case PIX_SRC ^ PIX_DST:				/* xor */
			return( 0x6 ) ;
	case PIX_SRC | PIX_DST:				/* or */
			return( 0x7 ) ;
	case PIX_NOT( PIX_SRC ) & PIX_NOT( PIX_DST ):	/* not and not */
			return( 0x8 ) ;
	case PIX_NOT( PIX_SRC ) ^ PIX_DST:		/* not xor */
			return( 0x9 ) ;
	case PIX_NOT( PIX_DST ):
			return( 0xa ) ;			/* invert destination pixels */
	case PIX_SRC | PIX_NOT( PIX_DST ):		/* or not */
			return( 0xb ) ;
	case PIX_NOT( PIX_SRC ):			/* not */
			return( 0xc ) ;
	case PIX_NOT( PIX_SRC ) | PIX_DST:		/* not or */
			return( 0xd ) ;
	case PIX_NOT( PIX_SRC ) | PIX_NOT( PIX_DST ):	/* not or not */
			return( 0xe ) ;
	case PIX_SET:	return( 0xf ) ;			/* set destination pixels */
	default:	return( 0x5 ) ;			/* no-op if things dont make sense */
	}
}

refresh_Xvector( x1,y1,x2,y2,col,sz,dd )				/* rules: 0 copy, 1 clear, 2 set, 3 invert */
int x1,y1,x2,y2,col,sz,dd ;
{
	D_refreshLine( Xscreen,x1,y1,x2,y2,sz,sz + dd - 1,convert_to_Xrule( PIX_SRC ),col ) ;
}

refresh_Xscreen( sx,sy,sz,dw,dh,dd,rule )			/* rules: 0 copy, 1 clear, 2 set, 3 invert */
int sx,sy,sz,dw,dh,dd,rule ;
{
	D_refreshRect( Xscreen,sx,sx + dw - 1,sy,sy + dh - 1,sz,sz + dd - 1,convert_to_Xrule( rule ) ) ;
}

refreshRop_Xscreen( sx,sy,sz,w,h,d,dx,dy,dz,rule )			/* rules: 0 copy, 1 clear, 2 set, 3 invert */
int sx,sy,sz,w,h,d,dx,dy,dz,rule ;
{
	D_refreshCopy( Xscreen,sx,sy,sz,w,h,d,dx,dy,dz,convert_to_Xrule( rule ) ) ;
}

put_Xcolor( pixel )
psint pixel ;
{
	if ( Xscreen != -1 )
		D_refreshCmap( Xscreen,pixel,red[ pixel ],green[ pixel ],blue[ pixel ] ) ;
}

void Xlocator()
{
	locatorReply reply ;


	D_locatorOne( Xscreen,&reply ) ;

	xpos = reply.xpos ;
	ypos = reply.ypos ;
	but1 = reply.but1 ;
	but2 = reply.but2 ;
	but3 = reply.but3 ;
}
