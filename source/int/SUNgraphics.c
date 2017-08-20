#include "errno.h"
#include "int.h"
#include "graphics.h"
#include <pixrect/pixrect_hs.h>
#include <sunwindow/window_hs.h>
#include <sunwindow/win_ioctl.h>
#include "sys/time.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include <signal.h>

#define	FLAGS		1
#define WP_FLAG         020000
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

#ifdef  X11
extern psint Xscreen ;                  /* window id for X screen -1 => no X window */
#endif  X11
psint Xscr,Yscr,Zscr ;			/* X, Y and Z dims of the screen */
psint realZscr ;                        /* the real Z dimension of the screen */
psint zapEnablePlane = SFALSE ;        /* is the screen being overwritten? by console output */
psint zapCounter = ( psint ) 0 ;        /* every 1200 interrupts - 30secs fix overlay plane */
struct pixrect *scr_pr = 0 ;		/* pixrect for graphics output */
struct pixwin *scr_pw = 0 ;		/* pixwin for graphics output */
psint ScrMask,lastmask ;                /* write enable masks for the screen */
int pw_fd ;				/* UNIX fd for the output window */
static int ppw_fd ;			/* UNIX fd for the current window */
static int Ppw_fd ;			/* UNIX fd for the current window */
psint repaint_pixwin = SFALSE ;		/* global flag set when pixwin corrupted */
static Rect pwin,mywin ;		/* rect's for resizing scr_pw */
psint sigwinch_ok = SFALSE ;		/* global flag set when its safe to repair pixwin damage */

mpr_static( Scrbm,1,1,1,0 ) ;		/* pixrect for screen copy */
mpr_static( srcbm,1,1,1,0 ) ;		/* source pixrect for rasterops */
mpr_static( dstbm,1,1,1,0 ) ;		/* destination pixrect for rasterops */

int cmapsize = 0 ;			/* size of the screen's colour map segment */
					/* the colour tables for setting the colour map */
unsigned char red[ 256 ],green[ 256 ],blue[ 256 ] ;
unsigned char svred[ 256 ],svgreen[ 256 ],svblue[ 256 ] ;

psint mxpos,mypos,mbut1,mbut2,mbut3 ;	/* state of SUN mouse */
psint mouse_live = SFALSE ;		/* is the mouse alive */
psint xpos,ypos,but1,but2,but3 ;	/* public state of SUN mouse */
					/* cursor to track mouse */
short curs_init[] = { 0xFC00,0xF800,0xF000,0xF800,0xDC00,0x8E00,0x0700,0x0380,
		      0x01C0,0x00E0,0x0070,0x0020,0x0000,0x0000,0x0000,0x0000 } ;
					/* cursor for click to quit */
short quit_curs[] = { 0xE974,0x8945,0x8946,0x8946,0xED75,0x0000,0x0E70,0x0450,
		      0x0450,0x0470,0x0000,0xF7EE,0x82AA,0xF2AE,0x12A8,0xF2E8 } ;
mpr_static( pscurs,16,16,1,curs_init ) ;/* startup cursor bitmap for mouse */
Cursor my_curs ;			/* pointer to cursor for suntools */
psint curlive = STRUE ;			/* is the cursor live - liable to move */
psint curvis = SFALSE ;			/* is the cursor visible */
psint curx,cury ;			/* where is the hotspot */
psint mousefd ;				/* fd for use accessing the mouse */
struct itimerval mtime ;		/* mouse interval timer */

psint scan( x,y )		/* calculates scan line length for given x,y */
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

void read_mouse()
{
	char Min[ 3 ] ;
	int rmask,wmask,xmask ;
	struct timeval tim0 ;

	( void ) signal( SIGALRM,SIG_IGN ) ;
	if ( scr_pw != ( struct pixwin * ) 0 ) read_events() ; else
#ifdef  X11
        if ( Xscreen == -1 )
#endif  X11
	{
		rmask = 1 << mousefd ; wmask = 0 ; xmask = 0 ; tim0.tv_sec = 0 ; tim0.tv_usec = 0 ;
		while ( select( mousefd + 1,&rmask,&wmask,&xmask,&tim0 ) == 1 )
		{
			if ( read( mousefd,Min,3 ) != 3 ) error( "error while reading mouse device" ) ;
			mbut1 = *Min & 4 ? SFALSE : STRUE ;
			mbut2 = *Min & 2 ? SFALSE : STRUE ;
			mbut3 = *Min & 1 ? SFALSE : STRUE ;
			mxpos += ( int )( Min[ 1 ] ) ;
			if ( mxpos < -curx ) mxpos = -curx ; else
			if ( mxpos - Xscr >= curx ) mxpos = Xscr - ( psint ) 1 ;
			mypos -= ( int )( Min[ 2 ] ) ;
			if ( mypos < -cury ) mypos = -cury ; else
			if ( mypos - Yscr >= cury ) mypos = Yscr - ( psint ) 1 ;

			if ( curlive )	/* cursor is live so update public mouse status + move cursor */
			{
				if ( curvis && ( xpos != mxpos || ypos != mypos ) )
				{
                                        int newmask ;
 
                                        newmask = 1 ;
                                        if ( newmask != ScrMask )
                                                pr_putattributes( scr_pr,&newmask ) ;
 
                                        pr_rop( scr_pr,mxpos,mypos,16,16,PIX_SRC ^ PIX_DST,&pscurs,0,0 ) ;
                                        pr_rop( scr_pr,xpos,ypos,16,16,PIX_SRC ^ PIX_DST,&pscurs,0,0 ) ;
 
                                        if ( newmask != ScrMask )
                                                pr_putattributes( scr_pr,&ScrMask ) ;
				}
				xpos = mxpos ; ypos = mypos ;
				but1 = mbut1 ;
				but2 = mbut2 ;
				but3 = mbut3 ;
			}
			rmask = 1 << mousefd ; wmask = 0 ; xmask = 0 ; tim0.tv_sec = 0 ; tim0.tv_usec = 0 ;
		}
                zapCounter++ ;
                if ( zapEnablePlane && ( zapCounter > ( psint ) 1200 ) )
                {
                        int thisPG ;                    /* current plane group */
 
                        zapCounter = ( psint ) 0 ;      /* reset counter */
                        thisPG = pr_get_plane_group( scr_pr ) ;
                        if ( thisPG != PIXPG_CURRENT )
                        {
                                pr_set_plane_group( scr_pr,PIXPG_OVERLAY_ENABLE ) ;
                                if ( PIXPG_OVERLAY_ENABLE == pr_get_plane_group( scr_pr ) )
                                {
                                        pr_rop( scr_pr,0,0,Xscr,Yscr,PIX_CLR,0,0,0 ) ;
                                        pr_set_plane_group( scr_pr,thisPG ) ;
                                }
                        }
                }
	}
	( void ) signal( SIGALRM,read_mouse ) ;
}

start_mtimer()
{
	mtime.it_interval.tv_sec = 0 ;
	mtime.it_interval.tv_usec = 25000 ;
	mtime.it_value.tv_sec = 0 ;
	mtime.it_value.tv_usec = 25000 ;
	( void ) setitimer( ITIMER_REAL,&mtime,( struct itimerval * ) 0 ) ;
}

stop_mtimer()
{
	mtime.it_interval.tv_sec = 0 ;
	mtime.it_interval.tv_usec = 0 ;
	mtime.it_value.tv_sec = 0 ;
	mtime.it_value.tv_usec = 0 ;
	( void ) setitimer( ITIMER_REAL,&mtime,( struct itimerval * ) 0 ) ;
}

start_mouse()
{
	if ( scr_pw == ( struct pixwin * ) 0 )
	{
		mousefd = open( "/dev/mouse",0 ) ;
		if ( mousefd == -1 ) return( -1 ) ;
	}

	xpos = 0 ; ypos = 0 ;		/* start for xpos and ypos */

	( void ) signal( SIGALRM,read_mouse ) ;
	start_mtimer() ;
	return( 0 ) ;

}

start_events()		/* used with pixwins */
{
	if ( scr_pw != ( struct pixwin * ) 0 )
	{
		Inputmask im ;				/* mask struct for controlling events handled */
		int pw_flgs = 0 ;

		win_get_kbd_mask( pw_fd,&im ) ;		/* leave all key board input to read(2) */
		input_imnull( &im ) ;
		win_set_kbd_mask( pw_fd,&im ) ;

		win_get_pick_mask( pw_fd,&im ) ;	/* look for mouse movement and button up/down */
		input_imnull( &im ) ;
		im.im_flags = IM_NEGEVENT ;
		win_setinputcodebit( &im,BUT(1) ) ;
		win_setinputcodebit( &im,BUT(2) ) ;
		win_setinputcodebit( &im,BUT(3) ) ;
		win_setinputcodebit( &im,LOC_MOVE ) ;
		win_set_pick_mask( pw_fd,&im ) ;

							/* ensure that reads of event info don't block */
		pw_flgs = fcntl( pw_fd,F_GETFL,pw_flgs ) ;
		pw_flgs |= FNDELAY ;
		( void ) fcntl( pw_fd,F_SETFL,pw_flgs ) ;

		xpos = 0 ; ypos = 0 ;			/* start assumed mouse location at 0,0 */
		but1 = SFALSE ;
		but2 = SFALSE ;
		but3 = SFALSE ;

		( void ) start_mouse() ;				/* start timed reads */
	}
}

read_events()		/* used with pixwins */
{
	Event ev ;

	while( input_readevent( pw_fd,&ev ) != -1 )
	{
		switch( event_id( &ev ) )
		{
		case BUT(1):
			but1 = event_is_up( &ev ) ? SFALSE : STRUE ;
			break ;
		case BUT(2):
			but2 = event_is_up( &ev ) ? SFALSE : STRUE ;
			break ;
		case BUT(3):
			but3 = event_is_up( &ev ) ? SFALSE : STRUE ;
			break ;
		default: ;
		}
		xpos = ( psint )( event_x( &ev ) ) ;
		ypos = ( psint )( event_y( &ev ) ) ;
	}
	if ( errno != EWOULDBLOCK )
	{
		mouse_live = SFALSE ;
		error( "cannot read mouse" ) ;
	}
}

static char mycmsname[ CMS_NAMESIZE ] ;
#define SCMSNAME "SALGOL"

char *mkcmsname()			/* make a unique name for the colour segment - SALGOL ++ process id */
{
	int i,num ;
	char xtn[ CMS_NAMESIZE - sizeof( SCMSNAME ) - 1 ] ;
	extern int getpid() ;

	num = getpid() ;
	for( i = 0 ; i < sizeof( xtn ) - 1 ; i++ )
	{
		xtn[ i ] = ( char )( num % 10 ) + '0' ;
		num /= 10 ;
	}
	xtn[ sizeof( xtn ) - 1 ] = ( char ) 0 ;

	( void ) strcpy( mycmsname,( char * )SCMSNAME ) ;
	( void ) strcat( mycmsname,( char * )xtn ) ;
	return( mycmsname ) ;
}

init_scr_pw( maxZscr,defCmap )
psint maxZscr,defCmap ;                 /* the maximum depth of image that can be accessed - and which cmap to use */
{
	char gfx_win[ 1024 ] ;		/* test for a gfx window */

	ppw_fd = we_getgfxwindow( gfx_win ) ;
	if ( ppw_fd == 0 )		/* is there a window */
	{				/* try to open it */
		ppw_fd = open( gfx_win,2 ) ;
		if ( ppw_fd != -1 )
		{			/* get a new window to blanket the current one */
			pw_fd = win_getnewwindow() ;
                        if ( !win_insertblanket( pw_fd,ppw_fd ) )
                        {
                                scr_pw = pw_open( pw_fd ) ;
                                if ( maxZscr == 1 )
                                        pw_use_fast_monochrome( scr_pw ) ;
                        }
			if ( scr_pw == ( struct pixwin * ) 0 )
			{
				close( pw_fd ) ;
				close( ppw_fd ) ;
			} else
			{		/* what is number of parent's frame */
				Ppw_fd = win_getlink( ppw_fd,WL_PARENT ) ;
					/* construct its name */
				win_numbertoname( Ppw_fd,gfx_win ) ;
					/* open it and find its size */
				Ppw_fd = open( gfx_win,2 ) ;
				if ( Ppw_fd == -1 )
				{	/* abandon using scr_pw */
					scr_pw = ( struct pixwin * ) 0 ;
                                        close( pw_fd ) ;
                                        close( ppw_fd ) ;
				} else
                                {
                                        int cnt ;
 
                                        /* how big is this window ? */
                                        win_getrect( ppw_fd,&mywin ) ;
                                        if ( Zscr == 1 )			/* if its a monochrome display - do prepare */
                                                pw_preparesurface( scr_pw,&mywin ) ;
 
                                        Xscr = mywin.r_width ;
                                        Yscr = mywin.r_height ;
                                        Zscr = scr_pw->pw_pixrect->pr_depth ;
 
                                        realZscr = Zscr ;
                                        if ( Zscr > maxZscr ) Zscr = maxZscr ; /* restrict the maximum number of planes to the max requested */
 
                                        cmapsize = 1 << Zscr ; /* map is depth to power of 2 - therefore - 2 or 256 */
                                        if ( cmapsize > 256 ) cmapsize = 256 ; /* limit in case of deeper fbs */
 
                                        pw_getattributes( scr_pw,&ScrMask ) ;
                                        ScrMask &= ~( -1 << realZscr ) ; /* save the write enable mask for the window */
                                        lastmask = ScrMask ;
 
                                        /* how big is the parent ? */
                                        win_getrect( Ppw_fd,&pwin ) ;
 
                                        /* set up the colour map */
                                        pw_setcmsname( scr_pw,mkcmsname() ) ;
 
                                        /* put a predefined colour map - so S text can be read */
                                        *red = *green = *blue = ( unsigned char)( defCmap ? 0 : 255 ) ;
                                        for ( cnt = 1 ; cnt < cmapsize ; cnt++ )
					{       /* grey scale - simulation */
                                                int Cnt ;
 
                                                Cnt = defCmap ? ( ( ( cmapsize - cnt ) * 255 ) / ( cmapsize - 1 ) - 1 ) :
                                                                ( ( cnt * 255 ) / ( cmapsize - 1 ) - 1 ) ;
                                                red[ cnt ] = ( unsigned char )( Cnt ) ;
                                                green[ cnt ] = ( unsigned char )( Cnt ) ;
                                                blue[ cnt ] = ( unsigned char )( Cnt ) ;
                                        }
                                        red[ cmapsize - 1 ] = green[ cmapsize - 1 ] = blue[ cmapsize -1 ] = ( unsigned char )( defCmap ? 255 : 0 ) ;

/* setting the colour map does a prepare */
                                        pw_putcolormap( scr_pw,0,cmapsize,red,green,blue ) ;
                                        pw_putcolormap( scr_pw,0,cmapsize,red,green,blue ) ;
 

/* now set the write enable mask */
                                        pw_putattributes( scr_pw,&ScrMask ) ;
 
                                        /* start the event processing */
                                        start_events() ;
                                }
			}
		}
	}
}

psint get_defCmap()
{
        char *defcmap ;
        extern char *getenv() ;
 
        defcmap = getenv( SVSCMAP ) ;                  /* shell variable holding desired colour map */
        if ( defcmap != ( char * ) 0 )                  /* if one was specified use it! */
        {
                if ( strcmp( defcmap,"blackonwhite" ) == 0 ) return( ALT_CMAP ) ;
                if ( strcmp( defcmap,"whiteonblack" ) == 0 ) return( DEF_CMAP ) ;
        }
        return( ALT_CMAP ) ;
}

psint get_maxZscr()
{
        char *maxZscr ;
        extern char *getenv() ;
 
        maxZscr = getenv( SVSZDIM ) ;                  /* shell variable holding desired max Z dimension */
        if ( maxZscr != ( char * ) 0 )                  /* if one was specified use it! */
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
 

make_screen()           /* setup the system interface to the screen */
{
        char *myterm ;
        int scr_fd,maxZscr,defCmap ;
        extern char *ttyname() ;
 
        scr_pr = ( struct pixrect * ) 0 ;
        scr_pw = ( struct pixwin * ) 0 ;
 
        maxZscr = get_maxZscr() ;                       /* max depth of image the user wnats to know about */
        defCmap = get_defCmap() ;
        myterm = ttyname( 1 ) ;                         /* find name of my tty */
        /* only do graphics if requested and logged onto the console - and mouse available */
        if ( ( maxZscr > 0 ) && ( myterm != 0 ) )
        {
                if ( strcmp( "/dev/console",myterm ) == 0 )
                {
                        scr_fd = open( "/dev/fb",2 ) ;                  /* open the graphics device to make sure its there */
                        if ( scr_fd >= 0 )
                        {
                                close( scr_fd ) ;                       /* open OK so close extra fd */
 
                                scr_pr = pr_open( "/dev/fb" ) ;			/* open the graphics display */
 
                                if ( scr_pr != ( struct pixrect * ) 0 )		/* is there a screen ? */
                                {
                                        int cnt,cnt2 ;
 
                                        Xscr = scr_pr->pr_size.x ;		/* max X dim of a window */
                                        Yscr = scr_pr->pr_size.y ;		/* max Y dim of a window */
                                        Zscr = scr_pr->pr_depth ;		/* max Z dim of a window */
 
                                        zapEnablePlane = STRUE ;
                                        if ( ( Zscr > maxZscr ) && ( maxZscr == 1 ) )
                                        {
                                                pr_set_plane_group( scr_pr,PIXPG_OVERLAY_ENABLE ) ;
                                                Zscr = scr_pr->pr_depth ;
                                                if ( Zscr == 1 )
                                                {				/* did find the overlay enableplane */
                                                        pr_rop( scr_pr,0,0,Xscr,Yscr,PIX_SET,0,0,0 ) ;
                                                        pr_set_plane_group( scr_pr,PIXPG_OVERLAY ) ;
                                                        zapEnablePlane = SFALSE ;
                                                }
                                        }
                                        realZscr = Zscr ;
                                        if ( Zscr > maxZscr ) Zscr = maxZscr ; /* restrict the available planes to the max requested */
 
                                        cmapsize = 1 << Zscr ; /* map is depth to power of 2 - therefore - 2 or 256 */
                                        if ( cmapsize > 256 ) cmapsize = 256 ; /* in case we find deeper fbs */
                                        pr_getattributes( scr_pr,&ScrMask ) ;
                                        ScrMask &= ~( -1 << realZscr ) ; /* save mask must be for all planes just in case */
 
					/* put a predefined colour map - so S text can be read */
					for ( cnt2 = 0 ; cnt2 < 256 ; cnt2 += cmapsize )
					{
						red[ cnt2 ] = green[ cnt2 ] = blue[ cnt2 ] = ( unsigned char )( defCmap ? 0 : 255 ) ;
						for ( cnt = 1 ; cnt < cmapsize ; cnt++ )
						{	/* grey scale - simulation */
							int Cnt ;

							Cnt = defCmap ? ( ( ( cmapsize - cnt ) * 255 ) / ( cmapsize - 1 ) - 1 ) :
									( ( cnt * 255 ) / ( cmapsize - 1 ) - 1 ) ;
							red[ cnt + cnt2 ] = ( unsigned char )( Cnt ) ;
							green[ cnt + cnt2 ] = ( unsigned char )( Cnt ) ;
							blue[ cnt + cnt2 ] = ( unsigned char )( Cnt ) ;
						}
						cnt = cmapsize + cnt2 - 1 ;	/* make sure the last entry is opposite of first entry */
						red[ cnt ] = green[ cnt ] = blue[ cnt ] = ( unsigned char )( defCmap ? 255 : 0 ) ;
					}
					pr_getcolormap( scr_pr,0,cmapsize,svred,svgreen,svblue ) ;	/* save the original cmap */

					pr_putcolormap( scr_pr,0,cmapsize,red,green,blue ) ;
 
                                        if ( start_mouse() == -1 )
                                        {
                                                pr_close( scr_pr ) ;		/* close the frame buffer pixrect */
                                                scr_pr = ( struct pixrect * ) 0 ;
                                        }
                                }
                        }
                }
                                                                /* grab the current pixwin - if using a tty */
                if ( scr_pr == ( struct pixrect * ) 0 ) init_scr_pw( maxZscr,defCmap ) ;
#ifdef  X11
                if ( ( scr_pr == ( struct pixrect * ) 0 ) && ( scr_pw == ( struct pixwin * ) 0 ) )
                {
                        init_scr_X( maxZscr,defCmap ) ;
                }
                if ( scr_pr || scr_pw || Xscreen != ( psint ) -1 ) init_pstty() ;               /* set up text output tty emulator */
        }
        if ( ( scr_pr == ( struct pixrect * ) 0 ) && ( scr_pw == ( struct pixwin * ) 0 ) && ( Xscreen == ( psint ) -1 ) )
#else
                if ( scr_pr || scr_pw ) init_pstty() ;          /* set up text output tty emulator */
        }
        if ( ( scr_pr == ( struct pixrect * ) 0 ) && ( scr_pw == ( struct pixwin * ) 0 ) )
#endif  X11
        {                       /* if no graphics cut the screen down to 1by1by1 */
                Xscr = 1 ;
                Yscr = 1 ;
                Zscr = 1 ;
                cmapsize = 0 ;
        }
}

psint *init_screen()	/* make space for the screen */
{
	int i ;
	psint vecsz,*vec,slinc ;

							/* calculate the size of the screen copy */
	slinc = scan( Xscr,Yscr ) ;
	vecsz = ( Yscr * slinc ) / ( psint ) 4 ;	/* no. of 4 byte words to hold pixels */

	screen = c_image() ;		/* create an image descriptor */
	screen[ BITMAP ] = ( psint ) 0 ;
							/* if there is a screenmake it standard output */
#ifdef  X11
        screen[ WINDOW ] = ( scr_pr || scr_pw || Xscreen != ( psint ) -1 ) ? ( psb[ S_O_OFF ] ) : ( psint ) 0  ;
#else
	screen[ WINDOW ] = ( scr_pr || scr_pw ) ? ( psb[ S_O_OFF ] ) : ( psint ) 0  ;
#endif  X11
	screen[ X_OFFSET ] = ( psint ) 0 ;
	screen[ Y_OFFSET ] = ( psint ) 0 ;
	screen[ X_DIM ] = Xscr ;
	screen[ Y_DIM ] = Yscr ;

	vec = c_v_p( 1,Zscr ) ;	/* create a bmap vector */
	for ( i = 1 ; i <= Zscr ; i++ )
		vec[ 2 + i ] = ( psint ) 0 ;

	screen[ BITMAP ] = ( psint ) vec ;		/* fill in ptr to dummy vector */


	/* now create the bitmaps to hold a copy of the screen */

	for ( i = 1 ; i <= Zscr ; i++ )
	{
		vec = c_v_ib( -3,vecsz ) ;		/* create bitmap */
		vec[ DIM_X ] = Xscr ;
		vec[ DIM_Y ] = Yscr ;
		vec[ OFFSET ] = ( psint ) 7 ;
		vec[ NO_LINES ] = Yscr ;

		( ( psint * )( screen[ BITMAP ] ) )[ 2 + i ] = ( psint ) vec ;	/* fill in ptr to dummy vector */

		dstbm.pr_size.x = Xscr ;
		dstbm.pr_size.y = Yscr ;
		dstbm.pr_depth = 1 ;
		dstbm_data.md_image = ( short * )( vec + vec[ OFFSET ] ) ;
		dstbm_data.md_linebytes = slinc ;

		pr_rop( &dstbm,0,0,Xscr,Yscr,PIX_DST & PIX_NOT( PIX_DST ),0,0,0 ) ;
	}

									/* finally clear the screen devices */
	if ( scr_pr != ( struct pixrect * ) 0 )
	{
		pr_rop( scr_pr,0,0,Xscr,Yscr,PIX_DST & PIX_NOT( PIX_DST ),0,0,0 ) ;
	}
	if ( scr_pw != ( struct pixwin * ) 0 )
	{
		int rtx,rty ;

		rtx = ( int )( win_getwidth( pw_fd ) ) ;
		rty = ( int )( win_getheight( pw_fd ) ) ;
		pw_write( scr_pw,0,0,rtx,rty,PIX_DST & PIX_NOT( PIX_DST ),0,0,0 ) ;
	}
#ifdef  X11
        if ( Xscreen != ( psint ) -1 )
        {
                create_XImage( screen ) ;
        } else
#endif  X11
	sigwinch_ok = STRUE ;						/* its now safe to handle pixwin changes */

	return( screen ) ;
}

Screen( i )								/* setup Scrbm for plane i of the screen */
psint i ;								/* i starts at 0*/
{
	psint *scrbm ;

	Scrbm.pr_size.x = Xscr ;
	Scrbm.pr_size.y = Yscr ;
	Scrbm.pr_depth = 1 ;

	scrbm = ( psint * )( screen[ BITMAP ] ) ;
	scrbm = ( psint * )( scrbm[ ( psint ) 3 + i ] ) ;

	Scrbm_data.md_linebytes = bitchk( scrbm ) ;
	Scrbm_data.md_image = ( short * )( scrbm + scrbm[ OFFSET ] ) ;
}

#ifdef  X11
ps_refresh( sx,sy,sz,dw,dh,dd,ropfn )
int sx,sy,sz,dw,dh,dd,ropfn ;
#else
ps_refresh( sx,sy,sz,dw,dh,dd )
int sx,sy,sz,dw,dh,dd ;
#endif  X11
{
	int i ;
	int newmask ;				/* masks to control what planes are drawn on */

	if ( scr_pr != ( struct pixrect * ) 0 )
	{
		psint cv = curvis ;

		if ( cv ) HideCursor() ;

		if ( Zscr == 1 )
		{
			Screen( 0 ) ;
			pr_rop( scr_pr,sx,sy,dw,dh,PIX_SRC,&Scrbm,sx,sy ) ;
		} else
		{
			for ( i = 1 ; i <= dd ; i++ )
			{
				newmask = 1 << sz ;		/* set the write enable mask for the screen */
                                if ( newmask != ScrMask )
                                        pr_putattributes( scr_pr,&newmask ) ;

				Screen( sz++ ) ;
				pr_rop( scr_pr,sx,sy,dw,dh,PIX_SRC,&Scrbm,sx,sy ) ;
			}
                        if ( newmask != ScrMask )
				pr_putattributes( scr_pr,&ScrMask ) ;	/* save the write enable mask for the screen */
		}
		if ( cv ) ShowCursor() ;
	} else
	{
		if ( scr_pw != ( struct pixwin * ) 0 )
		{
			int rty,Dy ;
			psint save_ok ;

			save_ok = sigwinch_ok ;
			sigwinch_ok = SFALSE ;				/* disable pixwin repairs */
			rty = ( int )( win_getheight( pw_fd ) ) ;
			Dy = sy + rty - Yscr ;
			if ( Dy < 0 ) { dh += Dy ; sy -= Dy ; Dy = 0 ; }
			if ( dh > 0 )
			{
				if ( Zscr == 1 )
				{
					Screen( 0 ) ;
					pw_write( scr_pw,sx,Dy,dw,dh,PIX_SRC,&Scrbm,sx,sy ) ;
				} else
				{
					for ( i = 1 ; i <= dd ; i++ )
					{
                                                newmask = ( 1 << sz ) ; /* set the write enable mask for the screen */
                                                if ( newmask != lastmask )
                                                {
                                                        pw_putattributes( scr_pw,&newmask ) ;
                                                        lastmask = newmask ;
                                                }
 
                                                Screen( sz++ ) ;
                                                pw_write( scr_pw,sx,Dy,dw,dh,PIX_SRC,&Scrbm,sx,sy ) ;
					}
				}
			}
			sigwinch_ok = save_ok ;				/* reenable pixwin repairs */
			if ( repaint_pixwin ) refresh_screen() ;	/* check for damage to repair */
		}
#ifdef  X11
                  else
                {
                        if ( Xscreen != ( psint ) -1 ) refresh_Xscreen( sx,sy,sz,dw,dh,dd,ropfn ) ;
                }
#endif  X11
	}
}

ps_scrop( dx,dy,dw,dh,op,spr,sx,sy )
int dx,dy,dw,dh,op ; struct pixrect *spr ; int sx,sy ;
{
	int	inplace ;

	Screen( 0 ) ;
	if ( spr != ( struct pixrect * ) 0 ) inplace = SFALSE ; else
	{
		inplace = STRUE ;
		spr = &Scrbm ;
	}
	pr_rop( &Scrbm,dx,dy,dw,dh,op,spr,sx,sy ) ;
#ifdef  X11
        if ( Xscreen != -1 && inplace )
        {
                refreshRop_Xscreen( sx,sy,0,dw,dh,1,dx,dy,0,op ) ;
        } else
        {
                ps_refresh( dx,dy,0,dw,dh,1,op ) ;
        }
#else
        ps_refresh( dx,dy,0,dw,dh,1 ) ;
#endif  X11
}

resize_screen()
{
        if ( scr_pw != ( struct pixwin * ) 0 )
        {
                Rect twin ;
                psint save_ok ;
                int win_flags ;
 
                save_ok = sigwinch_ok ;
                sigwinch_ok = SFALSE ;
 
                                        /* resize if window not an icon */
                win_flags = win_getuserflags( Ppw_fd ) ;
                if ( !( win_flags & WUF_WMGR1 ) )
                {                       /* resize the output window */
                        win_getrect( Ppw_fd,&twin ) ;
                        if ( ( twin.r_width != pwin.r_width ) || ( twin.r_height != pwin.r_height ) )
                        {
                                twin.r_width = pwin.r_width ;
                                twin.r_height = pwin.r_height ;
                                win_setrect( Ppw_fd,&twin ) ;
                        }
                }
                                        /* finally do prepare surface so win is visibile */
                twin.r_top = 0 ; twin.r_left = 0 ;
                twin.r_width = win_getwidth( Ppw_fd ) ;
                twin.r_height = win_getheight( Ppw_fd ) ;
                pw_preparesurface( scr_pw,&twin ) ;
 
                sigwinch_ok = save_ok ;
        }
}

refresh_screen()
{
	psint save_ok ;

	save_ok = sigwinch_ok ;
	sigwinch_ok = SFALSE ;

	repaint_pixwin = SFALSE ;
	resize_screen() ;
#ifdef  X11
        ps_refresh( 0,0,0,Xscr,Yscr,Zscr,PIX_SRC ) ;
#else
        ps_refresh( 0,0,0,Xscr,Yscr,Zscr ) ;
#endif  X11

	sigwinch_ok = save_ok ;
}

void sigwinch_hndlr()
{
	( void ) signal( SIGWINCH,SIG_IGN ) ;
	if ( scr_pw != ( struct pixwin * ) 0 )
	{
		pw_damaged( scr_pw ) ;
		pw_donedamaged( scr_pw ) ;
		repaint_pixwin = STRUE ;
	}
	( void ) signal( SIGWINCH,sigwinch_hndlr ) ;
	if ( sigwinch_ok ) refresh_screen() ;
}

ps_scrvector( x1,y1,x2,y2,col,sz,dd )
int x1,y1,x2,y2,col,sz,dd ;
{
        int newmask,val ;                                       /* mask to control what planes are drawn on */

        if ( scr_pr != ( struct pixrect * ) 0 )
        {
                psint cv = curvis ;

/*
                switch( col )
                {
                case PIX_SET:   col = PIX_SRC ;
                                val = -1 ;
                                break ;
                case PIX_CLR:   col = PIX_SRC ;
                                val = 0 ;
                                break ;
                default:        val = 0 ;
                }
*/

                if ( cv ) HideCursor() ;
                if ( Zscr == 1 )
                {
                        pr_vector( scr_pr,x1,y1,x2,y2,PIX_SRC,col & 1 ) ;
                } else
                {
                        newmask = ~( ( psint ) -1 << dd ) ;     /* a mask for the first dd planes */
                        newmask <<= sz ;                        /* a mask for dd planes starting at plane sz */
                        newmask &= ~( -1 << Zscr ) ;
                        if ( newmask != ScrMask )               /* set the write enable mask for the screen */
                                pr_putattributes( scr_pr,&newmask ) ;

                        pr_vector( scr_pr,x1,y1,x2,y2,PIX_SRC,col ) ;

                        if ( newmask != ScrMask )               /* restore the write enable mask for the screen */
                                pr_putattributes( scr_pr,&ScrMask ) ;
                }
                if ( cv ) ShowCursor() ;
        } else
        {
                if ( scr_pw != ( struct pixwin * ) 0 )
                {
                        int rty ;

/*
                        switch( col )
                        {
                        case PIX_SET:   col = PIX_SRC ;
                                        val = -1 ;
                                        break ;
                        case PIX_CLR:   col = PIX_SRC ;
                                        val = 0 ;
                                        break ;
                        default:        val = 0 ;
                        }
*/

                        sigwinch_ok = SFALSE ;                 /* disable pixwin repairs */
                        rty = ( int )( win_getheight( pw_fd ) ) ;
                        y1 = y1 + rty - Yscr ;
                        y2 = y2 + rty - Yscr ;

                        if ( Zscr == 1 )
                        {
                                pw_vector( scr_pw,x1,y1,x2,y2,PIX_SRC,col & 1 ) ;
                        } else
                        {
                                newmask = ~( ( psint ) -1 << dd ) ;     /* a mask for the first dd planes */
                                newmask <<= sz ;                        /* a mask for dd planes starting at plane sz */
                                newmask &= ~( -1 << Zscr ) ;
                                if ( newmask != lastmask )              /* set the write enable mask for the window */
                                {
                                        pw_putattributes( scr_pw,&newmask ) ;
                                        lastmask = newmask ;            /* record the write enable mask for the window */
                                }

                                pw_vector( scr_pw,x1,y1,x2,y2,PIX_SRC,col ) ;
                        }

                        sigwinch_ok = STRUE ;                  /* enable pixwin repairs */
                        if ( repaint_pixwin ) refresh_screen() ;        /* check for damage to repair */
                }
#ifdef  X11
                  else
                {
                        if ( Xscreen != ( psint ) -1 ) refresh_Xvector( x1,y1,x2,y2,col,sz,dd ) ;
                }
#endif  X11
        }  
}

psint *init_cursor()	/* make space for the cursor */
{
	psint i,vecsz,*vec,slinc ;

	if ( scr_pw != ( struct pixwin * ) 0 )
	{
		my_curs = cursor_create( CURSOR_OP,PIX_SRC ^ PIX_DST,CURSOR_IMAGE,&pscurs,0 ) ;
		win_setcursor( pw_fd,my_curs ) ;
	}

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

	cursor[ BITMAP ] = ( psint ) vec ;	/* fill in pointer to bitmap vector */

	/* now initialise the bitmap to the default cursor */

	vec = c_v_ib( -3,vecsz ) ;				/* create bitmap */
	vec[ DIM_X ] = CURXDIM ;
	vec[ DIM_Y ] = CURYDIM ;
	vec[ OFFSET ] = ( psint ) 7 ;
	vec[ NO_LINES ] = CURYDIM ;

	( ( psint * )( cursor[ BITMAP ] ) )[ 3 ] = ( psint ) vec ;	/* fill in pointer to bitmap vector */

	/* fill in cursor bitmap with initial value */
	{
		int i ;

		for ( i = 0 ; i < 16 ; i++ )	/* copy in 1 short at a time */
		{
			short *v ;

						/* each 16 bits is 32 bits in the bitmap */
			v = ( short * )( vec + 7 + i ) ;
			*v = curs_init[ i ] ;
		}
	}

	SetCursor( 0,0 ) ;		/* set hot spot to SUN 0,0 */
	ShowCursor() ;			/* display the cursor if mouse inuse */

	mouse_live = STRUE ;		/* so it can be used? */
	return( cursor ) ;
}

HideCursor()			/* hide the cursor */
{
	if ( scr_pr != ( struct pixrect * ) 0 )
	{
               int newmask ;
 
                newmask = 1 ;           /* top plane of screen */
                curlive = SFALSE ;     /* kill cursor */
                curvis = SFALSE ;
                if ( newmask != ScrMask )
                        pr_putattributes( scr_pr,&newmask ) ;
                pr_rop( scr_pr,xpos,ypos,16,16,PIX_SRC ^ PIX_DST,&pscurs,0,0 ) ;
                if ( newmask != ScrMask )
                        pr_putattributes( scr_pr,&ScrMask ) ;
                curlive = STRUE ;      /* kick it again */
	} else
	{
		curvis = SFALSE ;
		if ( scr_pw != ( struct pixwin * ) 0 )
		{
			cursor_set( my_curs,CURSOR_SHOW_CURSOR,SFALSE,0 ) ;
			win_setcursor( pw_fd,my_curs ) ;
		}
#ifdef  X11
                  else
                {
                        if ( Xscreen != -1 ) hide_Xcursor() ;
                }
#endif  X1
	}
}

ShowCursor()			/* make the cursor visible */
{
	if ( scr_pr != ( struct pixrect * ) 0 )
	{
                int newmask ;
 
                newmask = 1 ;           /* top plane of screen */
                curlive = SFALSE ;     /* kill cursor */
                if ( newmask != ScrMask )
                        pr_putattributes( scr_pr,&newmask ) ;
                pr_rop( scr_pr,xpos,ypos,16,16,PIX_SRC ^ PIX_DST,&pscurs,0,0 ) ;
                if ( newmask != ScrMask )
                        pr_putattributes( scr_pr,&ScrMask ) ;
                curvis = STRUE ;
                curlive = STRUE ;      /* kick it again */
	} else
	{
		curvis = STRUE ;
		if ( scr_pw != ( struct pixwin * ) 0 )
		{
			cursor_set( my_curs,CURSOR_SHOW_CURSOR,STRUE,0 ) ;
			win_setcursor( pw_fd,my_curs ) ;
		}
#ifdef  X11
                  else
                {
                        if ( Xscreen != -1 ) show_Xcursor() ;
                }
#endif  X11
	}
}

SetCursor( x,y )		/* move the cursor hotspot */
psint x,y ;
{
	if ( scr_pr != ( struct pixrect * ) 0 )
	{
		stop_mtimer() ;
		curx = x ; cury = y ;
		start_mtimer() ;
	} else
	{
		if ( scr_pw != ( struct pixwin * ) 0 )
		{
			cursor_set( my_curs,CURSOR_XHOT,x,CURSOR_YHOT,y,0 ) ;
			curx = x ; cury = y ;
			win_setcursor( pw_fd,my_curs ) ;
		}
#ifdef  X11
                  else
                {
                        if ( Xscreen != -1 ) set_Xcursor( x,y ) ;
                        curx = x ; cury = y ;
                }
#endif  X11
	}
}

ChangeCursor( ptr )		/* change the cursor pattern */
psint *ptr ;
{
#ifdef  X11
        if ( scr_pr || scr_pw || Xscreen != ( psint ) -1 )
#else
        if ( scr_pr || scr_pw )
#endif  X11
	{
		int i ;
		psint cv = curvis && scr_pr ;
		extern void check_startstop() ;

		check_startstop() ;		/* wait if ^S,^Q pending */
		if ( cv ) HideCursor() ;

		for ( i = 0 ; i < 16 ; i++ )
		{
			short *v ;

				/* address of next 16 bits in bitmap vector */
			v = ( short * )( ptr + 7 + i ) ;
			curs_init[ i ] = *v ;
		}
		if ( scr_pw != ( struct pixwin * ) 0 )
			win_setcursor( pw_fd,my_curs ) ;

#ifdef  X11
                if ( Xscreen != ( psint ) -1 ) change_Xcursor() ;
#endif  X11
		if ( cv ) ShowCursor() ;
	}
}

undo_cursor()	/* reset cursor mode, pattern and disable tablet */
{
	HideCursor() ;
	if ( scr_pr || scr_pw ) stop_mtimer() ;
	mouse_live = SFALSE ;
}

click_to_quit()
{
#ifdef  X11
        if ( mouse_live && Xscreen != ( psint ) -1 ) quit_Xscreen() ; else
#endif  X11
	if ( mouse_live && ( scr_pw != ( struct pixwin * ) 0 || scr_pr != ( struct pixrect * ) 0 ) )
	{
		int i ;

		if ( curvis ) HideCursor() ;			/* amke sure cursor is hidden */

		for ( i = 0 ; i < 16 ; i++ )			/* change it to "click to stop" */
			curs_init[ i ] = quit_curs[ i ] ;

		ShowCursor() ;					/* display the new cursor */

		( void ) signal( SIGALRM,read_mouse ) ;			/* make sure the mouse will be read */
		start_mtimer() ;				/* make sure the interval timer is running */

			/* wait until all buttons released - lazy wait - timer will kill off sigpause after 1/4 sec. */
		while( ( but1 || but2 || but3 ) )
		{
			int i ;

			for ( i = 0 ; i < 10 ; i++ ) sigpause( 0 ) ;
		}
			/* wait until a button is hit - lazy wait - timer will kill off sigpause after 1/4 sec.  */
		while( !( but1 || but2 || but3 ) )
		{
			int i ;

			for ( i = 0 ; i < 10 ; i++ ) sigpause( 0 ) ;
		}

		reset_cmap() ;					/* to make sure the console is readable */
		stop_mtimer() ;
		undo_cursor() ;					/* reset the cursor */

		if ( scr_pw != ( struct pixwin * ) 0 )		/* reselect the original window */
		{
			win_removeblanket( pw_fd ) ;
			win_set_kbd_focus( Ppw_fd,win_fdtonumber( ppw_fd ) ) ;
		}
	}
}

setcursor( mode )	/* proc set.cursor.mode( int the.mode ) */
psint	mode ;
{
	switch( mode )
	{
	case 0:	if ( !curvis ) ShowCursor() ;
			break ;
	case 1:	if ( curvis ) HideCursor() ;
			break ;
	default: ;					/* ignore silly cursor modes */
	}
}

curstip()				/* proc cursor.tip( pntr -> pntr )  */	
{					/* make the cursor tip be x,y  */
	psint x,y,*pstrc,*nstrc ;

	nstrc = c_structure( POINT_STRC ) ;	/* create a structure for result - a point.strc */
	nstrc[ 1 ] = curx ;
	nstrc[ 2 ] = ( psint )( CURYDIM - cury - 1 ) ;

	pstrc = ( psint * )( *psp ) ;	/* get old point.strc structure off stack */
	*psp = ( psint ) nstrc ;	/* return pointer to new point.strc */

					/* Compare trade marks */
	if ( ( *pstrc & B_0_15 ) != POINT_STRC ) error( STRUCTERR ) ;

	x = pstrc[ 1 ] ;		/* take x out of strc, in range 0->CURXDIM - 1 */
	y = pstrc[ 2 ] ;		/* take y out of strc, in range 0->CURYDIM - 1 */
	if ( x < ( psint ) 0 || x >= ( psint ) CURXDIM || y < ( psint ) 0 || y >= ( psint ) CURYDIM )
		error( "Cursor tip may not be assigned outside cursor area" ) ;

						/* set the cursor tip of the real cursor */
	SetCursor( x,CURYDIM - y - 1 ) ;

}
	
badwin( offset )
psint offset ;
{
	psint *wst ;

	/* image contains a file or if this is 0 a vector of bitmaps */
	wst = ( psint * )( psp[ offset ] ) ;	/* get image desc */

	/* wst points to image descriptor  */
	wst = ( psint * )( wst[ BITMAP ] ) ;

	/* wst points to vector of bitmaps ; check each plane is not a pid */
}

wait_for()	/* wait for one of flagged events to occur - then return */
{
}

/*
int chk_keyb()
{
	return( in_pend ) ;
}
*/

locator()	/* read mouse position & buttons ( file -> pntr ) */
{
	psint *res,lbut1,lbut2,lbut3,xp,yp ;

	res = c_structure( MOUSE_STRC ) ;	/* global loc struct */
	*--psp = ( psint ) res ;		/* since stand func skip closure */

	curlive = SFALSE ;			/* disable cursor */
	if ( scr_pw != ( struct pixwin * ) 0 )
	{					/* invert the Y pos */
		sigpause( 0 ) ;			/* hang around until something is input from the mouse */
		yp = ( psint )( win_getheight( pw_fd ) ) - ypos - ( psint ) 1 ;
		xp = xpos ;			/* x pos of mouse */
	} else
	{
#ifdef  X11
                if ( Xscreen != ( psint ) -1 )
                {
                        Xlocator() ;
                        xp = xpos ;                             /* x pos of mouse */
                        yp = Yscr - ypos - ( psint ) 1 ;        /* inverted y pos */
                } else
#endif  X11
                {
                        sigpause( 0 ) ;         /* hang around until something is input from the mouse */
                        xp = xpos + curx ;                      /* x pos of mouse */
			yp = Yscr - ypos - cury - ( psint ) 1 ;	/* inverted y pos */
		}
	}
	lbut1 = but1 ;				/* state of mouse buttons */
	lbut2 = but2 ;
	lbut3 = but3 ;
	curlive = STRUE ;			/* kick cursor again */

	res[ 1 ] = ( psint ) 0 ;	/* vec of button values */
	res[ 2 ] = xp ;			/* x - window relative position */
	res[ 3 ] = yp ;			/* inverted y pos */
	res[ 4 ] = STRUE ;		/* window selected ? - dummy value */

	res = c_v_ib( 1,3 ) ;				/* create vec of bools for the buttons */
	res[ 3 ] = lbut1 ;
	res[ 4 ] = lbut2 ;
	res[ 5 ] = lbut3 ;

	( ( psint * )( *psp ) )[ 1 ] = ( psint ) res ;		/* fill in ptr to vec of bools from mouse strc */
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
	psint x1,x2,y1,y2,col,Xdim,Ydim,a_cursor,nplanes ;
	psint *wdes_d,*wst,*ptr ;
        extern void check_startstop() ;

	badwin( ( psint ) 0 ) ;			/* check for pids etc. */
	wst = ( psint * )( *psp++ ) ;
	a_cursor = SFALSE ;
	if ( wst[ WINDOW ] )			/* a window therefore only 1 plane - mustbe the screen */
	{
		wdes_d = ( psint * )( wst[ WINDOW ] ) ;
		if ( !( wdes_d[ FLAGS ] & CLOSED_FLAG ) && ( wdes_d[ FLAGS ] & WP_FLAG ) ) f_lush( wdes_d ) ;
                check_startstop() ;             /* do this first or the display may get out of step */
	}

	wdes_d = ( psint * )( wst[ BITMAP ] ) ;
	a_cursor = ( wdes_d == ( psint * )( cursor[ BITMAP ] ) ) ;	/* is it the cursor */
	nplanes = wdes_d[ UPB ] ;						/* how many planes */
	ptr = ( psint * )( wdes_d[ 3 ] ) ;
	Xdim = ptr[ DIM_X ] ;
	Ydim = ptr[ DIM_Y ] ;

	srcbm.pr_size.x = Xdim ;
	srcbm.pr_size.y = Ydim ;
	srcbm.pr_depth = 1 ;

	col = *sp++ ;
	y2  = *sp++ + wst[ Y_OFFSET ] ;	/* normalise image -	*/
	x2  = *sp++ + wst[ X_OFFSET ] ;	/* back to 0,0		*/
	y1  = *sp++ + wst[ Y_OFFSET ] ;
	x1  = *sp++ + wst[ X_OFFSET ] ;

		/* line clipping algorithm to keep coords on area */
		/* give up if definitely nothing to draw */
	if ( lclip( &x1,&y1,&x2,&y2,wst[ X_OFFSET ],
		    wst[ X_DIM ] + wst[ X_OFFSET ] - ( psint ) 1,
		    wst[ Y_OFFSET ],wst[ Y_DIM ] + wst[ Y_OFFSET ] - ( psint ) 1 ) )
		return ;

	y1 = Ydim - y1 - ( psint ) 1 ;
	y2 = Ydim - y2 - ( psint ) 1 ;

/*
        switch( col )
        {
        case 0:         col = PIX_SET ;
                        break ;
        case 1:         col = PIX_CLR ;
                        break ;
        case 2:
        default:        col = PIX_NOT( PIX_DST ) ;
        }
*/

	while ( nplanes-- > ( psint ) 0 )			/* do draw.line on every plane */
	{
		psint *ptr,tmp ;

		ptr = ( psint * )( wdes_d[ nplanes + 3 ] ) ;

		tmp = ( col >> nplanes ) & 1 ? PIX_DST | PIX_NOT( PIX_DST ) : PIX_DST & PIX_NOT( PIX_DST ) ;
		srcbm_data.md_linebytes = bitchk( ptr ) ;
		srcbm_data.md_image = ( short * )( ptr + ptr[ OFFSET ] ) ;
		pr_vector( &srcbm,x1,y1,x2,y2,tmp,0 ) ;
	}

	if ( wst[ WINDOW ] )				/* if its a draw the vector on the screen */
	{
		psint start,*screenbits,ptr ;

		nplanes = wdes_d[ UPB ] ;			/* how many planes were there ? */
		ptr = wdes_d[ 3 ] ;

		start = 0 ;
		screenbits = ( psint * )( screen[ BITMAP ] ) ;
		while( start < Zscr && ptr != screenbits[ start + ( psint ) 3 ] ) start++ ;

		if ( start + nplanes <= Zscr )
		{
			ps_scrvector( x1,y1,x2,y2,col,start,nplanes ) ;
		}
	}
	if ( a_cursor )		/* if the destination is the cursor */
	{			/*   must tell o.s it has changed   */
		psint *ptr ;

				/* find ptr to image and check alignment */
		ptr = ( psint * )( wdes_d[ 3 ] ) ;
		bitchk( ptr ) ;
		ChangeCursor( ptr ) ;
	}
}

rastop( ropfn )	/* rasterop for bitmaps and/or windows */
psint ropfn ;
{
	psint *wdes_s,*wst_s,*wdes_d,*wst_d,a_cursor ;
	psint *ptr,next_bm,nplanes,overlap ;
	psint SXdim,SYdim,DXdim,DYdim,wd_s,wd_d ;
	psint width,height,sx,sy,dx,dy,src_is_dst,src_eq_dst ;
        extern void check_startstop() ;

	badwin( ( psint ) 0 ) ;
	badwin( ( psint ) 1 ) ;

	wst_d = ( psint * )( *psp ) ;				/* destination image desc */
	wst_s = ( psint * )( psp[ 1 ] ) ;			/* source image desc */
	psp += 2 ;						/* pop params from stack */


	/* first find out how big source is */

	if ( wst_s[ WINDOW ] )					/* if a file */
	{
		wdes_s = ( psint * )( wst_s[ WINDOW ] ) ;		/* get the window - ps file */
		wd_s = wdes_s[ FLAGS ] ;			/* window descriptor src - unix file */
								/* if write pending */
		if ( !( wd_s & CLOSED_FLAG ) && ( wd_s & WP_FLAG ) ) f_lush( wdes_s ) ;
	}
	wdes_s = ( psint * )( wst_s[ BITMAP ] ) ;		/* get the vector of bitmaps */
	ptr = ( psint * )( wdes_s[ 3 ] ) ;		/* get the first plane */
			/* find start of src bitmap - the actual bits  */
	SXdim = ptr[ DIM_X ] ;				/* src X dimension */
	SYdim = ptr[ DIM_Y ] ;				/* src Y dimension */

	srcbm.pr_size.x = SXdim ;
	srcbm.pr_size.y = SYdim ;
	srcbm.pr_depth = 1 ;

	/* next find out how big destination is */

	if ( wst_d[ WINDOW ] )					/* if a window file */
	{
		wdes_d = ( psint * )( wst_d[ WINDOW ] ) ;	/* get the actual file */
		wd_d = wdes_d[ FLAGS ] ;		/* next few lines as above */
		if ( !( wd_d & CLOSED_FLAG ) && ( wd_d & WP_FLAG ) ) f_lush( wdes_d ) ;
                check_startstop() ;                             /* do this first so the display keeps in step */
	}
	wdes_d = ( psint * )( wst_d[ BITMAP ] ) ;		/* get the actual bitmap */
	ptr = ( psint * )( wdes_d[ 3 ] ) ;		/* get 1st plane */

	DXdim = ptr[ DIM_X ] ;				/* destination X dimension */
	DYdim = ptr[ DIM_Y ] ;				/* destination Y dimension */
	a_cursor = ( wdes_d == ( psint * )( cursor[ BITMAP ] ) ) ;

	dstbm.pr_size.x = DXdim ;
	dstbm.pr_size.y = DYdim ;
	dstbm.pr_depth = 1 ;

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
	wdes_s = ( psint * )( wst_s[ BITMAP ] ) ;
	wdes_d = ( psint * )( wst_d[ BITMAP ] ) ;

        src_eq_dst = ( wdes_s[ 3 ] == wdes_d[ 3 ] ) ? STRUE : SFALSE ;
        src_is_dst = ( src_eq_dst && dx == sx && dy == sy ) ? STRUE : SFALSE ;
        if ( src_is_dst )
	{
		switch( ropfn )
		{
		case 0: return ;
		case 1:	ropfn = PIX_NOT( PIX_DST ) ;
			break ;
		case 2:	return ;
		case 3:	ropfn = PIX_NOT( PIX_DST ) ;
			break ;
		case 4:	return ;
		case 5:	ropfn = PIX_NOT( PIX_DST ) ;
			break ;
		case 6:	ropfn = PIX_DST & PIX_NOT( PIX_DST ) ;
			break ;
		case 7:	ropfn = PIX_DST | PIX_NOT( PIX_DST ) ;
			break ;
		default:
			return ;
		}
	} else
	{
		switch( ropfn )
		{
		case 0:	ropfn = PIX_SRC ;
				break ;
		case 1:	ropfn = PIX_NOT( PIX_SRC ) ;
				break ;
		case 2:	ropfn = PIX_SRC & PIX_DST ;
				break ;
		case 3:	ropfn = PIX_NOT( PIX_SRC & PIX_DST ) ;
				break ;
		case 4:	ropfn = PIX_SRC | PIX_DST ;
				break ;
		case 5:	ropfn = PIX_NOT( PIX_SRC | PIX_DST ) ;
				break ;
		case 6:	ropfn = PIX_SRC ^ PIX_DST ;
				break ;
		case 7:	ropfn = PIX_NOT( PIX_SRC ^ PIX_DST ) ;
				break ;
		default:
			return ;
		}
	}

						/* index into vector of planes */
                                                /* test for over lapping planes and do in correct order */
                                                /* overlap set if rop must be done in order from 1st to last */
        nplanes = wdes_s[ UPB ] < wdes_d[ UPB ] ? wdes_s[ UPB ] : wdes_d[ UPB ] ;
        next_bm = ( psint ) 3 ;                 /* next bmap to look at */
        ptr = ( psint * )( wdes_s[ 3 ] ) ;      /* the first source plane */
        overlap = SFALSE ;                     /* flag to detect overlap */
        while( nplanes-- > ( psint ) 0 && !overlap )
        {                                       /* search for the 1st source plane in the destination */
                if ( ( psint ) ptr == wdes_d[ next_bm ] ) overlap = STRUE ;
                next_bm++ ;
        }

                                                /* do the rop allowing for ovelapping planes */
        nplanes = wdes_s[ UPB ] < wdes_d[ UPB ] ? wdes_s[ UPB ] : wdes_d[ UPB ] ;
                                                /* if planes overlap start at beginning */
        next_bm = overlap ? ( psint ) 3 : ( nplanes + ( psint ) 2 ) ;
        while( nplanes-- > ( psint ) 0 )
	{
		psint *plane1,*plane2 ;

						/* get address and scanl inc for src and dst planes */
		plane1 = ( psint * )( wdes_s[ next_bm ] ) ;
		srcbm_data.md_linebytes = bitchk( plane1 ) ;
		srcbm_data.md_image = ( short * )( plane1 + plane1[ OFFSET ] ) ;

		plane2 = ( psint * )( wdes_d[ next_bm ] ) ;
		dstbm_data.md_linebytes = bitchk( plane2 ) ;
		dstbm_data.md_image = ( short * )( plane2 + plane2[ OFFSET ] ) ;

						/* mark destination plane as being updated */
		/* now do rasterop store to store */
                if ( src_is_dst )
                pr_rop( &dstbm,dx,dy,width,height,ropfn,0,sx,sy ) ; else
                pr_rop( &dstbm,dx,dy,width,height,ropfn,&srcbm,sx,sy ) ;
 
                if ( overlap )
                {
                        next_bm++ ;             /* inc next_bm */
                } else
                {
                        next_bm-- ;             /* dec next_bm */
                }
	}

	if ( wst_d[ WINDOW ] )
	{					/* refresh the altered part of the screen */
		psint start,*screenbits ;
		extern void check_startstop() ;

		nplanes = wdes_s[ UPB ] < wdes_d[ UPB ] ? wdes_s[ UPB ] : wdes_d[ UPB ] ;
		ptr = ( psint * )( wdes_d[ 3 ] ) ;

		start = 0 ;
		screenbits = ( psint * )( screen[ BITMAP ] ) ;
		while( start < Zscr && ( psint ) ptr != screenbits[ start + ( psint ) 3 ] ) start++ ;

		if ( start + nplanes <= Zscr )
		{
#ifdef  X11
                        if ( Xscreen != -1 && src_eq_dst )
                        {
                                refreshRop_Xscreen( sx,sy,start,width,height,nplanes,dx,dy,start,ropfn ) ;
                        } else
                        {
                                ps_refresh( dx,dy,start,width,height,nplanes,ropfn ) ;
                        }
#else
                        ps_refresh( dx,dy,start,width,height,nplanes ) ;
#endif  X11
		}
	}
	if ( a_cursor )						/* if the destination is the cursor */
	{							/*   must tell o.s it has changed   */
		int *ptr ;

		ptr = ( int * )( wdes_d[ 3 ] ) ;
		bitchk( ptr ) ;
		ChangeCursor( ptr ) ;
	}
}

save_screen()		/* to copy screen onto its bitmap     */
			/* the bitmap is created if necessary */
{
}

mk_image()
{
	psint pix,xdim,ydim,nplanes,vecsz,*vec,*res,slinc,col ;

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

	*--psp = ( psint ) res ;			/* save the image descr. in case of GC */

	vec = c_v_p( 1,nplanes ) ;			/* build intermediate vector */

						/* initialise ptr fields to 0s in case of GC */
	while( nplanes-- != ( psint ) 0 ) vec[ nplanes + ( psint ) 3 ] = ( psint ) 0 ;
	nplanes = vec[ UPB ] ;

						/* fill in ptr to vec of bmaps from image desc. */
	( ( psint * )( *psp ) )[ BITMAP ] = ( psint ) vec ;

	slinc = scan( xdim,ydim ) ;
	vecsz = ( ydim * slinc ) / ( psint ) 4 ;	/* size of a plane's bitmap in 32bit words */

	srcbm.pr_size.x = xdim ;
	srcbm.pr_size.y = ydim ;
	srcbm.pr_depth = 1 ;
	srcbm_data.md_linebytes = slinc ;

	while( nplanes-- != ( psint ) 0 )			/* next create bitmap vectors and     */
	{					/* pointers to them from vec of bmaps */
		psint *next ;

						/* create the next bitmap */
		next = c_v_ib( -3,vecsz ) ;
		next[ DIM_X ] = xdim ;
		next[ DIM_Y ] = ydim ;
		next[ OFFSET ] = ( psint ) 7 ;
		next[ NO_LINES ] = ( psint ) 0 ;

		srcbm_data.md_image = ( short * )( next + next[ OFFSET ] ) ;
		col = ( pix >> nplanes ) & 1 ? PIX_DST | PIX_NOT( PIX_DST ) : PIX_DST & PIX_NOT( PIX_DST ) ;

		pr_rop( &srcbm,0,0,xdim,ydim,col,0,0,0 ) ;
	
						/* save ptr to new bmap in the vector of bitmaps */
		vec = ( psint * )( ( ( psint * )( *psp ) )[ BITMAP ] ) ;
		vec[ nplanes + ( psint ) 3 ] = ( psint ) next ;
	}
}
	
lnend()
{
	psint *wst,bmapinc,*wdes_s,x,y,*vec,Ydim ;
	psint cy1,cy2,cx1,cx2,direct,col,col1,depth,ok,*scanl ;

	badwin( ( psint ) 0 ) ;
	wst = ( psint * )( *psp++ ) ;

	direct = *sp++ ;
	y = *sp++ ;
	x = *sp++ ;
	col = *sp++ ;
	if ( x < ( psint ) 0 || x > wst[ X_DIM ] - ( psint ) 1 || y < ( psint ) 0 || y > wst[ Y_DIM ] - ( psint ) 1 )
		error( "attempt to read pixels outside image" ) ;
	x += wst[ X_OFFSET ] ;
	y += wst[ Y_OFFSET ] ;

	wdes_s = ( psint * )( wst[ BITMAP ] ) ;
	vec = ( psint * )( wdes_s[ 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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
				line = ( psint * )( wdes_s[ i + ( psint ) 3 ] ) ;
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

	badwin( 0 ) ;

	wst = ( psint * )( *psp++ ) ;

	y = *sp++ ;
	x = *sp++ ;
	if ( x < ( psint ) 0 || x >= wst[ X_DIM ] || y < ( psint ) 0 || y >= wst[ Y_DIM ] )
		error( "attempt to read pixel outside image" ) ;
	x += wst[ X_OFFSET ] ;
	y += wst[ Y_OFFSET ] ;

	wdes_s = ( psint * )( wst[ BITMAP ] ) ;				/* get the vector of bitmaps */
	nplanes = wdes_s[ 2 ] ;
	pixel = nplanes << ( psint ) 24 ;
	ptr = ( int * )( wdes_s[ 3 ] ) ;			/* ptr points at bitmap */
	y = ( ( psint * ) ptr )[ DIM_Y ] - y - ( psint ) 1 ;					/* invert Y dimension */

	while( nplanes-- > ( psint ) 0 )						/* find pixel in each plane */
	{
		ptr = ( int * )( wdes_s[ nplanes + ( psint ) 3 ] ) ;
		bmapinc = bitchk( ( psint * ) ptr ) ;
		ptr += 7 + y * bmapinc / 4  ;			/* NB 7 is offset from start of bitmap vector to the bitmap */
		if ( ( ptr[ LWRD(x) ] >> BIT(x) ) & 1 ) pixel |= ( ( psint ) 1 << nplanes ) ;
	}
	*--sp = pixel ;
}

Ydim()		/* Y dimension of an image */
{
	psint *wst ;

	wst = ( psint * )( *psp++ ) ;	/* get pointer to image desc */
	*--sp = wst[ Y_DIM ] ;	/* return size */
}

Xdim()		/* X dimension of an image */
{
	psint *wst ;

	wst = ( psint * )( *psp++ ) ;	/* get pointer to image desc */
	*--sp = wst[ X_DIM ] ;	/* return size */
}

plane()
{
	psint *wst,p ;

	wst = ( psint * )( *psp ) ;			/* get pointer to image's vector of bitmaps */
	wst = ( psint * )( wst[ BITMAP ] ) ;	/* either way wst points at vec of planes */

	p = *sp++ ;					/* index to required plane */
	if ( p < ( psint ) 0 || p >= wst[ UPB ] )
		error( "attempt to select non-existent plane of an image" ) ;
	wst = ( psint * )( wst[ p + ( psint ) 3 ] ) ;	/* wst is the required plane */
	*psp = ( psint ) wst ;				/* put plane on pointer stck */
}

depth()
{
	psint *wst ;

					/* get pointer to image's vector of bitmaps */
	wst = ( psint * )( *psp++ ) ;
	wst = ( psint * )( wst[ BITMAP ] ) ;

	*--sp = wst[ 2 ] ; 		/* Return upb of vector */
}

cimage()
{
	psint *oimage,*ovec,*nimage,*nvec,i,vecsz,nplanes ;
	psint xdim,ydim,*bmap,sx,sy,slinc ;

	badwin( ( psint ) 0 ) ;			/* bring in the original bitmap */

						/* make the new image descriptor */
	nimage = c_image() ;
	nimage[ BITMAP ] = ( psint ) 0 ;
	nimage[ WINDOW ] = ( psint ) 0 ;
	nimage[ X_OFFSET ] = ( psint ) 0 ;
	nimage[ Y_OFFSET ] = ( psint ) 0 ;

						/* how large is the new bitmap */
	oimage = ( psint * )( *psp ) ;
	xdim = oimage[ X_DIM ] ;
	ydim = oimage[ Y_DIM ] ;
	ovec = ( psint * )( oimage[ BITMAP ] ) ;
	nplanes = ovec[ 2 ] ;			/* upb is no. of planes */

	slinc = scan( xdim,ydim ) ;
	vecsz = ( ( slinc * ydim ) + ( psint ) 3 ) / ( psint ) 4 ;

	dstbm.pr_size.x = xdim ;
	dstbm.pr_size.y = ydim ;
	dstbm.pr_depth = 1 ;
	dstbm_data.md_linebytes = slinc ;

						/* complete and save the new image descriptor */
	nimage[ X_DIM ] = xdim ;
	nimage[ Y_DIM ] = ydim ;
	*psp = ( psint ) nimage ;

						/* create new vector of bitmaps */
	nvec = c_v_p( 1,nplanes ) ;
	nimage[ BITMAP ] = ( psint ) nvec ;
						/* copy the old vector of bitmaps to initialise the new */
	for( i = ( psint ) 0 ; i < ( psint ) 3 + nplanes ; i++ ) nvec[ i ] = ovec[ i ] ;

						/* calculate the source bitmap template */
	bmap = ( psint * )( ovec[ 3 ] ) ;
	srcbm.pr_size.x = bmap[ DIM_X ] ;
	srcbm.pr_size.y = bmap[ DIM_Y ] ;
	srcbm.pr_depth = 1 ;

						/* calculate the source rectangle */
	sy = bmap[ DIM_Y ] - oimage[ Y_OFFSET ] - ydim ;
	sx = oimage[ X_OFFSET ] ;


					/* make new vectors for the planes - if they exist */
					/* the screen should have 1 plane - vec entry is 0 */
	for( i = ( psint ) 0 ; i < nplanes ; i++ )
	{
		psint *vec1,*vec2 ;

		vec2 = c_v_ib( -3,vecsz ) ;	/* create the new bitmap */

					/* find the original plane */
		vec1 = ( psint * )( ovec[ i + ( psint ) 3 ] ) ;

					/* fill in ptr to new vec from ne vec of bmaps */
		nvec[ i + ( psint ) 3 ] = ( psint ) vec2 ;
		vec2[ DIM_X ] = xdim ;
		vec2[ DIM_Y ] = ydim ;
		vec2[ OFFSET ] = ( psint ) 7 ;
		vec2[ NO_LINES ] = ydim ;

					/* complete source and dest bitmaps */
		srcbm_data.md_linebytes = bitchk( vec1 ) ;
		srcbm_data.md_image = ( short * )( vec1 + vec1[ OFFSET ] ) ;
		dstbm_data.md_image = ( short * )( vec2 + vec2[ OFFSET ] ) ;
		pr_rop( &dstbm,0,0,xdim,ydim,PIX_SRC,&srcbm,sx,sy ) ;
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
		if ( scr_pr != ( struct pixrect * ) 0 ) pr_getcolormap( scr_pr,0,cmapsize,red,green,blue ) ; else
		{
			if ( scr_pw != ( struct pixwin * ) 0 ) pw_getcolormap( scr_pw,0,cmapsize,red,green,blue ) ;
		}
		*sp = ( psint )( red[ pix ] << 16 | green[ pix ] << 8 | blue[ pix ] ) ;
	}
}

colmap()					/* given pixel and its colour map index  */
{
	psint pix,index ;
	extern void check_startstop() ;

	index = *sp++ ;
	pix = *sp++ & B_0_23 ;		/* ignore the depth */

	if ( cmapsize > pix )			/* don't bother if there isn't an entry for the pixel */
	{
							/* load the colour map values */
		if ( scr_pr != ( struct pixrect * ) 0 )
		{
			pr_getcolormap( scr_pr,0,cmapsize,red,green,blue ) ;
		} else
		{
			if ( scr_pw != ( struct pixwin * ) 0 )
			{
				pw_getcolormap( scr_pw,0,cmapsize,red,green,blue ) ;
			}
		}
		check_startstop() ;		/* check for ^S,^Q */

		if ( cmapsize == 2 )		/* must make sure entries 0 and 1 are -1 or 0 but must be different */
		{
			unsigned char i0,i1 ;
	
			if ( index != ( psint ) 0 )
			{
				i0 = -1 ; i1 = 0 ;
			} else
			{
				i0 = 0 ; i1 = -1 ;
			}
			if ( pix != ( psint ) 0 )
			{
				i0 = ~i0 ; i1 = ~i1 ;
			}
			blue[ 0 ] = i0 ; blue[ 1 ] = i1 ;
			green[ 0 ] = i0 ; green[ 1 ] = i1 ;
			red[ 0 ] = i0 ; red[ 1 ] = i1 ;
		} else
		{
			blue[ pix ] = ( unsigned char )( index & B_0_7 ) ; index >>= ( psint ) 8 ;
			green[ pix ] = ( unsigned char )( index & B_0_7 ) ; index >>= ( psint ) 8 ;
			red[ pix ] = ( unsigned char )( index & B_0_7 ) ;
		}

#ifdef  X11
                if ( Xscreen != ( psint ) -1 )
                {
                        if ( cmapsize != 2 ) put_Xcolor( pix ) ; else
                        {
                                put_Xcolor( 0 ) ;
                                put_Xcolor( 1 ) ;
                        }
                } else
#endif  X11
	
		if ( scr_pr != ( struct pixrect * ) 0 ) pr_putcolormap( scr_pr,0,cmapsize,red,green,blue ) ; else
		{
			if ( scr_pw != ( struct pixwin * ) 0 ) pw_putcolormap( scr_pw,0,cmapsize,red,green,blue ) ;
		}
	
		if ( cmapsize == 2 )			/* redraw the screen to see the effect - in case on b/w */
#ifdef  X11
                {
                        if ( Xscreen == ( psint ) -1 ) ps_refresh( 0,0,0,Xscr,Yscr,Zscr,PIX_SRC ) ;
                }
#else
                        ps_refresh( 0,0,0,Xscr,Yscr,Zscr ) ;
#endif  X11
	}
}

reset_cmap()            /* will restore the colour map on program exit .... */
{
        if ( scr_pr != ( struct pixrect * ) 0 ) pr_putcolormap( scr_pr,0,cmapsize,svred,svgreen,svblue ) ;
}
