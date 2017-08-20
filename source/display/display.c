
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/errno.h>
#include "display.h"
#include "icon.h"
#include "setjmp.h"

typedef struct locatorEvents
{
	int		datestamp ;				/* X server datestamp when event generated */
	int		xpos,ypos ;				/* the x and y positions of the pointer */
	int		lflags ;				/* flags - 5 buttons encoded as bits */
} locatorEvents ;

typedef struct Xdisplay
{
	int		window_id ;				/* which window is this in the list of open windows */
	Display		*this_display ;				/* display on which graphics to be displayed */
	int		max_width,max_height ;			/* max X and Y dimensions of the physical display */
	char		displayName[ MAXFNAME ] ;		/* name of the display - minus the '.' screen number postfix */
	int		this_screen ;				/* this screen */
	Window		this_root ;				/* root window on this screen */
	Window		this_window ;				/* this window */
	Pixmap		pixmaps[ MAXZ ] ;			/* bitmaps 1 per plane */
	Visual		*this_visual ;				/* what screen layout is this window using? */
	int		thisX,thisY,thisZ,maxZ,thisSL ;		/* dimensions of the window - including bytes per scan line */
	int		mapped,subWindow,first ;		/* is this a Frame Buffer, has it been mapped yet? is it the first on the display ? */
	int		frozen,refresh ;			/* are screen updates frozen, if so is the screen uptodate? */
	struct Xdisplay	*next_window ;				/* next window on the same display connection - circular list */
	XImage		**this_image ;				/* screen images */
	Atom		delete_atom ;				/* Atom sent when window mustbe deleted */
	int		xhot,yhot ;				/* the x and y hotspots for the cursor */
	Cursor		cursor_id,visible_cursor,blank_cursor ;	/* possible ids of the displayed cursor */
	XColor		ForeClr,BackClr ;			/* X colors for foreground and background */
	int		inverted ;				/* are the colours on a monochrome display inverted?? */
	Colormap	cmap_id ;				/* the colour map id - None if no colour map was created */
	char		indefcmap[ MAXCMAP ] ;			/* list of flags to indicate if pixel is in the default colour map */
	int		cmapsize ;				/* size of the colour in entries */
	int		cmapInstalled ;				/* is the colourmap installed? */
	int		noButtons ;				/* the number of buttons supported by this windows pointer */
	locatorEvents	events[ LOCBUFFER ] ;			/* keep a buffer of locator events for this window */
	int		nextLocator,freeLocator ;		/* record the next event to read and the next free position in the buffer */
	D_keyboardEvents	input_buffer[ INPUTBUFFER ] ;		/* max number of (datestamp ++ keysymbol) input buffer */
	int		nextKeySym,freeKeySym ;			/* record the next event to read and the next free position in the buffer */
	int		inSync ;				/* has all output to the display been completed? */
	GC		this_gc,pixmap_gc ;			/* default graphics context, pixmap gc */
	Pixmap		this_icon ;				/* the icon's pixmap */
} Xdisplay ;

static Xdisplay	*windows[ MAXOPENWINDOWS ] ;
static int window_seq = 1 ;
static Window parentOfFirst = ( Window ) -1 ;
static int madeFirstWindow = MYFALSE ;				/* has the first window being requested? */
static int canPing = MYFALSE ;					/* can we ping the display? */
static int unPingedOps = 0 ;					/* number of ops since last ping */
static int Xdead = MYFALSE ;					/* is the X stuff dead - broken or otherwise not working */
static jmp_buf ferrorEnv ;					/* where to long jump on an X error */
static int CmapCounter = 0 ;					/* counter to space out sigpause()s - it avoids trashing the server */

static unsigned char reversedByte[ 256 ] ;			/* table for reversing bits in a byte */

extern int icon_bitmap_width ;
extern int icon_bitmap_height ;
extern unsigned char icon_bitmap_bits[] ;

static unsigned char initial_cursor_data[ MAXCURSOR ] ;		/* uninitialised - therefore blank cursor data */

extern void D_pingDisplay(void) ;

static void convert_icon(void)
{
	int i ;

	i = 0 ;
	while ( i < icon_bitmap_height )
	{
		int j ;
		unsigned char *row ;

		j = 0 ;
		row = icon_bitmap_bits + i * icon_bitmap_width / 8 ;
		i++ ;
		while ( j < icon_bitmap_width )
		{
			*row = reversedByte[ *row & 255 ] ;
			row++ ; j += 8 ;
		}
	}
}

static Xdisplay *newXdisplay(void)
{
	char *ptr ;

	ptr = malloc( sizeof( Xdisplay ) ) ;
	return( ( Xdisplay * ) ptr ) ;
}

static void get_Visual( Xdisplay *display )
{
	display->cmapsize = 0 ;
	display->cmapInstalled = MYFALSE ;
	display->cmap_id = None ;
	display->this_visual = DefaultVisual( display->this_display,display->this_screen ) ;

	if ( display->maxZ > 1 )			/* if its not a colour screen - give up */
	{
		int v,nvisuals,dclass,dcmapsize ;
		XVisualInfo template,*list ;

		template.screen = display->this_screen ;
		template.depth = display->thisZ ;
		list = XGetVisualInfo( display->this_display,VisualScreenMask,&template,&nvisuals ) ;

		if ( list == NULL ) return ;

		dcmapsize = 1 ;
		if ( display->thisZ >= 8 ) dcmapsize = 256 ; else	/* work out the appropriate size of the colormap */
		{
			for ( v = 1 ; v <= display->thisZ ; v++ )
			{
				dcmapsize *= 2 ;			/* double the cmap size for each plane in the window */
			}
		}

		if ( display->thisZ <= 8 ) dclass = PseudoColor ; else	/* work out what the desired class would be */
		if ( display->thisZ > 8 ) dclass = DirectColor ;

		v = 0 ;
		while( v < nvisuals && display->cmapsize == 0 )		/* look for any match on the visuals */
		{
			if ( list[ v ].depth >= display->thisZ )	/* try any visual of at least the right depth */
			{
				if ( list[ v ].class == dclass && list[ v ].colormap_size >= dcmapsize )	/* we've found a visual of the desired class */
				{
					display->this_visual = list[ v ].visual ;
					display->cmapsize = list[ v ].colormap_size ;
				}
			}
			v++ ;
		}
		XFree( (char *) list ) ;					/* finally, free the store used for the list */
	}
}

static void get_Cmap( Xdisplay *display )
{
	if ( display->cmapsize != 0 )					/* we've found a visual of the desired class */
	{
		int i,stat,npixs ;
		unsigned long pixs[ 256 ] ;
		Colormap cm ;

				/* now tweek the default colourmap in case our own isnt used */
		cm = DefaultColormap( display->this_display,display->this_screen ) ;
		npixs = 256 ;
		stat = 0 ;
		while ( npixs > 0 && stat == 0 )
		{
			stat = XAllocColorCells( display->this_display,cm,False,NULL,0,pixs,npixs ) ;
			if ( stat == 0 ) npixs-- ;
		}
		for ( i = 0 ; i < display->cmapsize ; i++ )		/* init our cmap from the default */
		{
			display->indefcmap[ i ] = ( char ) MYFALSE ;
		}
		for ( i = 0 ; i < npixs ; i++ )
		{
			int intensity ;
			XColor new_colour ;

			display->indefcmap[ pixs[ i ] ] = ( char ) MYTRUE ;
			intensity = pixs[ i ] & 1 ? 0 : 65535 ;	/* on => black, off => white */
			new_colour.pixel = pixs[ i ] ;
			new_colour.red = ( short ) intensity ;
			new_colour.green = ( short ) intensity ;
			new_colour.blue = ( short ) intensity ;
			new_colour.flags = DoRed | DoGreen | DoBlue ;

			XStoreColor( display->this_display,cm,&new_colour ) ;
		}

		display->cmap_id = XCreateColormap( display->this_display,display->this_window,display->this_visual,AllocAll ) ;
		for ( i = 0 ; i < display->cmapsize ; i++ )		/* init our cmap from the default */
		{
			XColor new_colour ;

			new_colour.pixel = i ;
			new_colour.flags = DoRed | DoGreen | DoBlue ;

			XQueryColor( display->this_display,cm,&new_colour ) ;

			new_colour.pixel = i ;
			new_colour.flags = DoRed | DoGreen | DoBlue ;

			XStoreColor( display->this_display,display->cmap_id,&new_colour ) ;
		}
		XSetWindowColormap( display->this_display,display->this_window,display->cmap_id ) ;

		if ( display->subWindow )
		{
			/* we are in a subwindow so we should add the colour map to the CMAP properties of the parent? */
			Window windows[ 2 ] ;

			windows[ 0 ] = display->this_window ;
			windows[ 1 ] = parentOfFirst ;
			XSetWMColormapWindows( display->this_display,parentOfFirst,windows,2 ) ;
		}
		XSync( display->this_display,False ) ;
	}
}

static void SetCursorPixels( Xdisplay *display )
{
	Colormap cm ;

	cm = DefaultColormap( display->this_display,display->this_screen ) ;

	display->ForeClr.pixel = BlackPixel( display->this_display,display->this_screen ) ;
	XQueryColor( display->this_display,cm,&display->ForeClr ) ;

	display->BackClr.pixel = WhitePixel( display->this_display,display->this_screen ) ;
	XQueryColor( display->this_display,cm,&display->BackClr ) ;

	display->inverted = MYFALSE ;			/* draw everything inverted?? */
	if ( display->maxZ == 1 )
	{
		if ( display->ForeClr.pixel != 1 )	/* if fore ground is not 1 - some inversion tricks are needed */
		{					/* on is actually off so inplace rops will mess up */
			display->inverted = MYTRUE ;	/* draw everything inverted */
		}
	}
}

static long ForeGround( Xdisplay *display )
{
	if ( display->thisZ == 1 ) return( BlackPixel( display->this_display,display->this_screen ) ) ;
	return( ( 1 << display->thisZ ) - 1 ) ;
}

static long BackGround( Xdisplay *display )
{
	if ( display->thisZ == 1 ) return( WhitePixel( display->this_display,display->this_screen ) ) ;
	return( 0 ) ;
}

static void get_GC( Xdisplay *display )
{
	unsigned long   valuemask = 0 ; /* ignore XGCvalues and use defaults */
	XGCValues       values ;
	unsigned int    line_width = 0 ;
	int             line_style = LineSolid ;
	int             cap_style = CapButt ;
	int             join_style = JoinBevel ;
	unsigned long Black,White ;

	/* create default graphics context */
	display->this_gc = XCreateGC( display->this_display,display->this_window,valuemask,&values ) ;

	/* specify black foreground since default may be white on white */
	Black = ForeGround( display ) ;
	XSetForeground( display->this_display,display->this_gc,Black ) ;

	White = BackGround( display ) ;
	XSetBackground( display->this_display,display->this_gc,White ) ;

	SetCursorPixels( display ) ;

	/* set line attributes */
	XSetLineAttributes( display->this_display,display->this_gc,line_width,line_style,cap_style,join_style ) ;
	XSetFillStyle( display->this_display,display->this_gc,FillSolid ) ;
	XSetClipMask( display->this_display,display->this_gc,None ) ;
}

static void get_pixmapGC( Xdisplay *display )
{
	unsigned long   valuemask = 0 ; /* ignore XGCvalues and use defaults */
	XGCValues       values ;
	unsigned int    line_width = 0 ;
	int             line_style = LineSolid ;
	int             cap_style = CapButt ;
	int             join_style = JoinBevel ;

	/* repeat the above for the pixmap gc - its 1 bit deep so use pixels 0&1 */
	display->pixmap_gc = XCreateGC( display->this_display,display->pixmaps[ 0 ],valuemask,&values ) ;
	XSetForeground( display->this_display,display->pixmap_gc,1 ) ;
	XSetBackground( display->this_display,display->pixmap_gc,0 ) ;

	XSetLineAttributes( display->this_display,display->pixmap_gc,line_width,line_style,cap_style,join_style ) ;
	XSetFillStyle( display->this_display,display->pixmap_gc,FillSolid ) ;
	XSetClipMask( display->this_display,display->pixmap_gc,None ) ;
}

static int reverse_bits( int i,int b )
{
	int j,k ;

	k = 0 ;
	for ( j = 0 ; j < b ; j++ )
	{
		if ( i & ( 1 << j ) ) k |= 1 << ( b - j - 1 ) ;
	}
	return( k ) ;
}

static int XnewImages( Xdisplay *display,char **offsets )
{
	int i ;
	char *ptr ;

	ptr = malloc( display->thisZ * sizeof( XImage * ) ) ;
	if ( ptr == 0 ) return( MYFALSE ) ;
	display->this_image = ( XImage ** ) ptr ;

	for ( i = 0 ; i < display->thisZ ; i++ )
	{
		XImage *Ximage ;

		Ximage = XCreateImage( display->this_display,display->this_visual,1,XYPixmap,0,offsets[ i ],
						display->thisX,display->thisY,32,display->thisSL )  ;
		if ( Ximage == ( XImage * ) 0 )
		{
			while( i-- > 0 ) XFree( (char *)( display->this_image[ i ] ) ) ;
			free( display->this_image ) ;
			display->this_image = ( XImage ** ) 0 ;
			return( MYFALSE ) ;
		}
#ifdef	LSBFIRST
		Ximage->byte_order = LSBFirst ;
#else
		Ximage->byte_order = MSBFirst ;
#endif	LSBFIRST
		Ximage->bitmap_bit_order = MSBFirst ;
		Ximage->bitmap_unit = 32 ;
		display->this_image[ i ] = Ximage ;
	}
	return( MYTRUE ) ;
}

int D_queryCmap( int win_id,int pixel,unsigned char *red,unsigned char *green,unsigned char *blue )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return ( -1 ) ;

	if ( display->cmap_id == None )
	{
		return( -1 ) ;
	} else
	{
		XColor new_colour ;

		unPingedOps++ ;
		canPing = MYFALSE ;

		new_colour.pixel = pixel ;
		new_colour.flags = DoRed | DoGreen | DoBlue ;

		XQueryColor( display->this_display,display->cmap_id,&new_colour ) ;

		*red = ( unsigned char )( new_colour.red ) ;
		*green = ( unsigned char )( new_colour.green ) ;
		*blue = ( unsigned char )( new_colour.blue ) ;

		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
		return( 0 ) ;
	}
}

int D_refreshCmap( int win_id,int pixel,unsigned char red,unsigned char green,unsigned char blue )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return ( -1 ) ;

	if ( display->cmap_id != None )
	{
		XColor new_colour ;

		unPingedOps++ ;
		canPing = MYFALSE ;

		new_colour.pixel = pixel ;
		new_colour.red = ( short )( red << 8 ) ;
		new_colour.green = ( short )( green << 8 ) ;
		new_colour.blue = ( short )( blue << 8 ) ;
		new_colour.flags = DoRed | DoGreen | DoBlue ;

		XStoreColor( display->this_display,display->cmap_id,&new_colour ) ;

		new_colour.pixel = pixel ;
		if ( display->indefcmap[ new_colour.pixel ] )
		{
			Colormap cm ;

			cm = DefaultColormap( display->this_display,display->this_screen ) ;

			new_colour.red = ( short )( red << 8 ) ;
			new_colour.green = ( short )( green << 8 ) ;
			new_colour.blue = ( short )( blue << 8 ) ;
			new_colour.flags = DoRed | DoGreen | DoBlue ;

			XStoreColor( display->this_display,cm,&new_colour ) ;
		}
		if ( CmapCounter++ == CMAPFREQ )
		{
			extern int sigpause(int) ;

			XSync( display->this_display,False ) ;		/* Sync the change just made and */
			sigpause( 0 ) ;					/* periodic pauses - every 1/3sec? */
			CmapCounter = 0 ;
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

static int invertRule( int rule )
{
	switch( rule )
	{
	case GXclear:		rule = GXset ; break ;
	case GXand:		rule = GXor ; break ;
	case GXandReverse:	rule = GXorReverse ; break ;
	case GXcopy:		break ;
	case GXandInverted:	rule = GXorInverted ; break ;
	case GXnoop:		break ;
	case GXxor:		rule = GXequiv ; break ;
	case GXor:		rule = GXand ; break ;
	case GXnor:		rule = GXnand ; break ;
	case GXequiv:		rule = GXxor ; break ;
	case GXinvert:		break ;
	case GXorReverse:	rule = GXandReverse ; break ;
	case GXcopyInverted:	break ;
	case GXorInverted:	rule = GXandInverted ; break ;
	case GXnand:		rule = GXnor ; break ;
	case GXset:		rule = GXclear ; break ;
	}
	return( rule ) ;
}

static int drawRule( int rule,int pixel )
{
	if ( pixel & 1 )
	{				/* rule to use if src pixel is on - 1 */
		switch( rule )
		{
		case GXclear:
		case GXandInverted:
		case GXnor:
		case GXcopyInverted:	rule = GXclear ; break ;
		case GXand:
		case GXnoop:
		case GXequiv:
		case GXorInverted:	rule = GXnoop ; break ;
		case GXandReverse:
		case GXxor:
		case GXinvert:
		case GXnand:		rule = GXinvert ; break ;
		case GXcopy:
		case GXor:
		case GXorReverse:
		case GXset:		rule = GXset ; break ;
		}
	} else
	{				/* rule to use if src pixel is off - 0 */
		switch( rule )
		{
		case GXclear:
		case GXand:
		case GXandReverse:
		case GXcopy:		rule = GXclear ; break ;
		case GXandInverted:
		case GXnoop:
		case GXxor:
		case GXor:		rule = GXnoop ; break ;
		case GXnor:
		case GXequiv:
		case GXinvert:
		case GXorReverse:	rule = GXinvert ; break ;
		case GXcopyInverted:
		case GXorInverted:
		case GXnand:
		case GXset:		rule = GXset ; break ;
		}
	}
	return( rule ) ;
}

static void inPlaceRefresh( Xdisplay *display,int sx,int sy,int w,int h,int dx,int dy,int z1,int z2,int rule )
{
	if ( display->maxZ > 1 )
	{
		int i,mask1,mask2,mask ;

		XSetFunction( display->this_display,display->pixmap_gc,rule ) ;
		for ( i = z1 ; i <= z2 ; i++ )
		{
			XCopyArea( display->this_display,display->pixmaps[ i ],display->pixmaps[ i ],display->pixmap_gc,sx,sy,w,h,dx,dy ) ;
		}

		if ( display->thisZ == 1 )
		{
			XSetFunction( display->this_display,display->this_gc,GXcopy ) ;
			XSetPlaneMask( display->this_display,display->this_gc,AllPlanes ) ;
			XCopyPlane( display->this_display,display->pixmaps[ 0 ],display->this_window,display->this_gc,dx,dy,w,h,dx,dy,1 ) ;
		} else
		{
			mask1 = ( 1 << z1 ) - 1 ;
			mask2 = ( 1 << (z2 + 1) ) - 1 ;
			mask = mask2 & ~mask1 ;

			XSetFunction( display->this_display,display->this_gc,rule ) ;
			XSetPlaneMask( display->this_display,display->this_gc,mask ) ;
			XCopyArea( display->this_display,display->this_window,display->this_window,display->this_gc,sx,sy,w,h,dx,dy ) ;
		}
	} else
	{
		if ( display->inverted ) rule = invertRule( rule ) ;
		XSetFunction( display->this_display,display->this_gc,rule ) ;
		XCopyArea( display->this_display,display->this_window,display->this_window,display->this_gc,sx,sy,w,h,dx,dy ) ;
	}
}

static void XRefresh( Xdisplay *display,int x1,int x2,int y1,int y2,int z1,int z2,int rule )
{
	if ( x1 > x2 ) XRefresh( display,x2,x1,y1,y2,z1,z2,rule ) ; else
	if ( y1 > y2 ) XRefresh( display,x1,x2,y2,y1,z1,z2,rule ) ; else
	if ( z1 > z2 ) XRefresh( display,x1,x2,y1,y2,z2,z1,rule ) ; else
	if ( display != ( Xdisplay * ) 0 )
	{
		if ( display->mapped && display->frozen ) display->refresh = MYTRUE ; else
		if ( display->mapped && !display->frozen )
		{
			int i,w,h ;

			w = x2 - x1 + 1 ;
			h = y2 - y1 + 1 ;
			switch( rule )
			{
			case GXnoop:	return ;
			case GXclear:
			case GXinvert:
			case GXset:
					inPlaceRefresh( display,x1,y1,w,h,x1,y1,z1,z2,rule ) ;
					break ;
			default:	
					if ( display->maxZ == 1 )
					{
						if ( display->inverted )
						{
							XSetFunction( display->this_display,display->this_gc,GXcopyInverted ) ;
						} else
						{
							XSetFunction( display->this_display,display->this_gc,GXcopy ) ;
						}
						XPutImage( display->this_display,display->this_window,display->this_gc,
								display->this_image[ 0 ],x1,y1,x1,y1,w,h ) ;
					} else
					{
						XSetFunction( display->this_display,display->this_gc,GXcopy ) ;
						XSetPlaneMask( display->this_display,display->this_gc,AllPlanes ) ;
						XSetFunction( display->this_display,display->pixmap_gc,GXcopy ) ;
						for ( i = z1 ; i <= z2 ; i++ )
						{
							XPutImage( display->this_display,display->pixmaps[ i ],display->pixmap_gc,
								display->this_image[ i ],x1,y1,x1,y1,w,h ) ;

						}
						if ( display->thisZ == 1 )
						{
							XCopyPlane( display->this_display,display->pixmaps[ 0 ],display->this_window,
								display->this_gc,x1,y1,w,h,x1,y1,1 ) ;
						} else
						{
							XSetForeground( display->this_display,display->this_gc,AllPlanes ) ;
							XSetBackground( display->this_display,display->this_gc,0 ) ;
							XSetFunction( display->this_display,display->this_gc,GXcopy ) ;
							for ( i = z1 ; i <= z2 ; i++ )
							{
								XSetPlaneMask( display->this_display,display->this_gc,1 << i ) ;
								XCopyPlane( display->this_display,display->pixmaps[ i ],display->this_window,
										display->this_gc,x1,y1,w,h,x1,y1,1 ) ;
/*
								XSetFunction( display->this_display,display->this_gc,GXor ) ;
								XSetForeground( display->this_display,display->this_gc,1 << i ) ;
								XSetBackground( display->this_display,display->this_gc,0 ) ;
								XCopyPlane( display->this_display,display->pixmaps[ i ],display->this_window,
										display->this_gc,x1,y1,w,h,x1,y1,1 ) ;

								XSetFunction( display->this_display,display->this_gc,GXand ) ;
								XSetForeground( display->this_display,display->this_gc,AllPlanes ) ;
								XSetBackground( display->this_display,display->this_gc,~( 1 << i ) ) ;
								XCopyPlane( display->this_display,display->pixmaps[ i ],display->this_window,
										display->this_gc,x1,y1,w,h,x1,y1,1 ) ;
*/
							}
							XSetForeground( display->this_display,display->this_gc,ForeGround( display ) ) ;
							XSetBackground( display->this_display,display->this_gc,BackGround( display ) ) ;
						}
					}
			}
		}
	}
}

int D_refreshRect( int win_id,int x1,int x2,int y1,int y2,int z1,int z2,int rule )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	canPing = MYFALSE ;
	unPingedOps++ ;
	XRefresh( display,x1,x2,y1,y2,z1,z2,rule ) ;
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

int D_refreshCopy( int win_id,int xs,int ys,int zs,int w,int h,int d,int xd,int yd,int zd,int rule )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	unPingedOps++ ;
	switch( rule )
	{
	case GXnoop:	return( 0 ) ;
	case GXclear:
	case GXinvert:
	case GXset:
			canPing = MYFALSE ;
			inPlaceRefresh( display,xd,yd,w,h,xd,yd,zd,zd + d - 1,rule ) ;
			canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
			return( 0 ) ;
	default:;
	}

	canPing = MYFALSE ;
	if ( zs == zd )		/* is the starting plane the same ? */
	{
		inPlaceRefresh( display,xs,ys,w,h,xd,yd,zd,zd + d - 1,rule ) ;
	} else
	{					/* planes are different - just redraw from shared image */
		XRefresh( display,xd,xd + w - 1,yd,yd + h - 1,zd,zd + d - 1,GXcopy ) ;
	}
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

int D_refreshLine( int win_id,int x1,int y1,int x2,int y2,int z1,int z2,int rule,int pixel )
{
	int needPixel ;
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	switch( rule )
	{
	case GXnoop:	return( 0 ) ;
	case GXclear:
	case GXinvert:
	case GXset:	needPixel = MYFALSE ;
			break ;
	default:	needPixel = MYTRUE ;
	}

	unPingedOps++ ;
	canPing = MYFALSE ;

	if ( display->maxZ > 1 )
	{
		int i,mask1,mask2,mask,pmrule ;

		mask1 = ( 1 << z1 ) - 1 ;
		mask2 = ( 1 << (z2 + 1) ) - 1 ;
		mask = ( mask2 & ~mask1 ) ;

		for ( i = z1 ; i <= z2 ; i++ )
		{
			pmrule = needPixel ? drawRule( rule,pixel >> i ) : rule ;
			XSetFunction( display->this_display,display->pixmap_gc,pmrule ) ;
			if ( x1 == x2 && y1 == y2 )
			{
				XDrawPoint( display->this_display,display->pixmaps[ i ],display->pixmap_gc,x1,y1 ) ;
			} else
			{
				XDrawLine( display->this_display,display->pixmaps[ i ],display->pixmap_gc,x1,y1,x2,y2 ) ;
			}
		}
		if ( display->thisZ == 1 )
		{
			if ( needPixel ) rule = drawRule( rule,pixel ) ;
			XSetFunction( display->this_display,display->this_gc,pmrule ) ;
			XSetPlaneMask( display->this_display,display->this_gc,AllPlanes ) ;
			if ( x1 == x2 && y1 == y2 )
			{
				XDrawPoint( display->this_display,display->this_window,display->this_gc,x1,y1 ) ;
			} else
			{
				XDrawLine( display->this_display,display->this_window,display->this_gc,x1,y1,x2,y2 ) ;
			}
		} else
		{
			if ( needPixel )
			{
				XSetForeground( display->this_display,display->this_gc,pixel << z1 ) ;
			}
			XSetFunction( display->this_display,display->this_gc,rule ) ;
			XSetPlaneMask( display->this_display,display->this_gc,mask ) ;
			if ( x1 == x2 && y1 == y2 )
			{
				XDrawPoint( display->this_display,display->this_window,display->this_gc,x1,y1 ) ;
			} else
			{
				XDrawLine( display->this_display,display->this_window,display->this_gc,x1,y1,x2,y2 ) ;
			}
			if ( needPixel )
			{
				XSetForeground( display->this_display,display->this_gc,ForeGround( display ) ) ;
			}
		}
	} else
	{
		if ( needPixel ) rule = drawRule( rule,pixel ) ;
		if ( display->inverted ) rule = invertRule( rule ) ;
		XSetFunction( display->this_display,display->this_gc,rule ) ;
		if ( x1 == x2 && y1 == y2 )
		{
			XDrawPoint( display->this_display,display->this_window,display->this_gc,x1,y1 ) ;
		} else
		{
			XDrawLine( display->this_display,display->this_window,display->this_gc,x1,y1,x2,y2 ) ;
		}
	}
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

static int convertName( char *dname )
{
	int s = 0 ;
	char *c ;

	c = dname ;
	while( *c != ( char ) 0 ) c++ ;

	c-- ;
	while( c > dname && *c != '.' ) c-- ;

	if ( c > dname )
	{
		*c = 0 ;
		while( *++c != ( char ) 0 ) s = s * 10 + *c - '0' ;
	}
	return( s ) ;
}

static Xdisplay *findDisplay( char *dname )
{
	int i ;

	for ( i = 0 ; i < MAXOPENWINDOWS ; i++ )
	{
		Xdisplay *next ;

		next = windows[ i ] ;
		if ( next != ( Xdisplay * ) 0 )
		{
			if ( !strcmp( next->displayName,dname ) )
			{
				return( next ) ;
			}
		}
	}
	return( ( Xdisplay * ) 0 ) ;
}

static int firstWindow( int desiredZ,int *xdim,int *ydim,int *zdim )
{
	unsigned char	buttonMap[ 5 ] ;
	int nmap = 5,new_window,screen_number ;
	char *dname,*window_number ;
	extern char *getenv() ;
	Xdisplay *display ;

	if ( madeFirstWindow ) return( -1 ) ;	/* we can only do this once.... */

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	new_window = 0 ;
	display = newXdisplay() ;
	if ( display == ( Xdisplay * ) 0 )
	{
		return( -1 ) ;			/* give up if newXdisplay fails */
	}
	display->window_id = new_window ;

						/* search for an X window */

	dname = getenv( "DISPLAY" ) ;			/* make sure shell var DISPLAY is setup */
	if ( dname == ( char * ) 0 || *dname == ( char ) 0 )
	{						/* if not - or firstWindow made - give up */
		return( -1 ) ;
	}

	window_number = getenv( "WINDOWID" ) ;		/* next check the WINDOWID var. */
	if ( window_number == ( char * ) 0 )		/* if not set do a full openWindow */
	{
		return( D_openWindow( dname,desiredZ,xdim,ydim,zdim ) ) ;
	}

						/* we now have a DISPLAY & WINDOW - create a subWindow */
						/* to blanket the window we were given */
	screen_number = convertName( dname ) ;
	if ( screen_number < 0 )
	{
		free( display ) ;
		new_window = -1 ;
	} else
	{
		Display *connection ;

		canPing = MYFALSE ;
		connection = XOpenDisplay( dname ) ;
		if ( connection != ( Display * ) 0 && screen_number >= ScreenCount( connection ) )
		{
			XCloseDisplay( connection ) ;
			connection = ( Display * ) 0 ;
		}
		display->first = MYTRUE ;
		display->next_window = display ;			/* list of windows only contains this window */

		if ( connection == ( Display * ) 0 )
		{
			free( display ) ;
			new_window = -1 ;
		} else
		{
			Window dummy1 ;
			int dummy2,dummy2a ;
			unsigned int dummy3 ;

			display->this_display = connection ;

			strcpy( display->displayName,dname ) ;						/* save the display name */
			display->this_screen = screen_number ;						/* the screen */
			display->max_width = DisplayWidth( display->this_display,display->this_screen ) ;	/* screen dimensions */
			display->max_height = DisplayHeight( display->this_display,display->this_screen ) ;

			parentOfFirst = ( Window ) atoi( window_number ) ;

			if ( !XGetGeometry( display->this_display,parentOfFirst,&dummy1,&dummy2,&dummy2a,
						xdim,ydim,&dummy3,zdim ) )
			{
				XCloseDisplay( display->this_display ) ;
				free( display ) ;
				new_window = -1 ;
				parentOfFirst = ( Window ) -1 ;
			} else
			{
				int wa_mask ;
				Pixmap new_pixmap ;
				XSetWindowAttributes detail ;

				madeFirstWindow = MYTRUE ;		/* the first window has been allocated */
				display->subWindow = MYTRUE ;		/* this is a sub window not a top level window */

				display->thisZ = desiredZ ;		/* the user's choice of depth */
				if ( display->thisZ <= 0 ) display->thisZ = 1 ;
				if ( display->thisZ > display->maxZ ) display->thisZ = display->maxZ ;

				display->maxZ = *zdim ;			/* the parent's depth */

				get_Visual( display ) ;
				detail.background_pixel = 0 ;
				detail.bit_gravity = NorthWestGravity ;
				wa_mask = CWBackPixel | CWBitGravity ;
				display->this_window = XCreateWindow( display->this_display,
								parentOfFirst,
								0,0,*xdim,*ydim,dummy3,
								*zdim,InputOutput,display->this_visual,wa_mask,&detail ) ;

				/* create GC for text and drawing */
				get_GC( display ) ;
			
				display->xhot = 0 ;
				display->yhot = 0 ;

				new_pixmap = XCreatePixmapFromBitmapData( display->this_display,
							display->this_window,initial_cursor_data,16,16,1,0,1 )  ;

				display->visible_cursor = XCreatePixmapCursor( display->this_display,new_pixmap,new_pixmap,
						&display->ForeClr,&display->BackClr,display->xhot,display->yhot ) ;
				XFreePixmap( display->this_display,new_pixmap ) ;

				display->blank_cursor = display->visible_cursor ;
				display->cursor_id = display->visible_cursor ;

				XDefineCursor( display->this_display,display->this_window,display->cursor_id ) ;

				display->thisX = *xdim ;
				display->thisY = *ydim ;
				display->thisZ = *zdim ;
				display->maxZ = *zdim ;

				/* work out what kind of colourmap we might have - install it if nec. */
				get_Cmap( display ) ;

				/* Now get the number of logical / physical buttons available */
				display->noButtons = XGetPointerMapping( display->this_display,buttonMap,nmap ) ;
				if ( display->noButtons > 5 ) display->noButtons = 5 ;	/* Stick to 5 max */

				display->nextLocator = 0 ;
				display->freeLocator = 0 ;
				display->inSync = MYFALSE ;
				display->nextKeySym = 0 ;
				display->freeKeySym = 0 ;

				display->mapped = MYFALSE ;		/* we will map the window later on */
				display->frozen = MYFALSE ;

				windows[ new_window & WINMASK ] = display ;

				XSelectInput( display->this_display,parentOfFirst,ExposureMask | KeyPressMask |
							PointerMotionMask | StructureNotifyMask ) ;
				XSelectInput( display->this_display,display->this_window,ExposureMask | KeyPressMask |
							OwnerGrabButtonMask | ButtonPressMask | ButtonReleaseMask |
							ColormapChangeMask | PointerMotionMask | StructureNotifyMask ) ;

			}
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( new_window ) ;
}

int D_openWindow( char *dname,int desiredZ,int *xdim,int *ydim,int *zdim )
{
	unsigned char	buttonMap[ 5 ] ;
	int nmap = 5,new_window,screen_number ;
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	if ( dname == ( char * ) 0 || *dname == ( char ) 0 )
	{
		return( firstWindow( desiredZ,xdim,ydim,zdim ) ) ;
	}

	unPingedOps++ ;
	new_window = 1 ;
	while( ( new_window < MAXOPENWINDOWS ) && ( windows[ new_window ] != ( Xdisplay * ) 0 ) ) new_window++ ;
	if ( new_window == MAXOPENWINDOWS )
	{
		new_window = -1 ;		/* there are no free slots in windows... give up ! */
	} else
	{
		canPing = MYFALSE ;
		display = newXdisplay() ;
		if ( display == ( Xdisplay * ) 0 )
		{
			return( -1 ) ;			/* give up if newXdisplay fails */
		}
		new_window |= ( window_seq++ << WINPWROF2 ) ;
		display->window_id = new_window ;

		screen_number = convertName( dname ) ;
		if ( screen_number < 0 )
		{
			display->this_display = ( Display * ) 0 ;
		} else
		{
			Display *connection ;
			Xdisplay *found ;

			connection = ( Display * ) 0 ;
			found = findDisplay( dname ) ;
			if ( found != ( Xdisplay * ) 0 )
			{
				connection = found->this_display ;
				if ( screen_number < ScreenCount( connection ) )
				{
					display->next_window = found->next_window ;	/* and new window to circular list of windows */
					found->next_window = display ;
				} else
				{
					connection = ( Display * ) 0 ;
				}
				display->first = MYFALSE ;
			} else
			{
				connection = XOpenDisplay( dname ) ;
				if ( connection != ( Display * ) 0 && screen_number >= ScreenCount( connection ) )
				{
					XCloseDisplay( connection ) ;
					connection = ( Display * ) 0 ;
				}
				display->first = MYTRUE ;
				display->next_window = display ;			/* list of windows only contains this window */
			}
			display->this_display = connection ;
		}
		if ( display->this_display == ( Display * ) 0 )
		{
			free( display ) ;
			new_window = -1 ;
		} else
		{
			int wa_mask ;
			Pixmap new_pixmap ;
			XSetWindowAttributes detail ;

			strcpy( display->displayName,dname ) ;							/* save the display name */
			display->this_screen = screen_number ;							/* the screen */
			display->max_width = DisplayWidth( display->this_display,display->this_screen ) ;	/* screen dimensions */
			display->max_height = DisplayHeight( display->this_display,display->this_screen ) ;
			display->maxZ = DisplayPlanes( display->this_display,display->this_screen ) ;

			*xdim = display->max_width ;
			*ydim = display->max_height ;
			*zdim = display->maxZ ;

										/* create opaque window */
			display->subWindow = MYFALSE ;		/* this will be a top level window */
			display->thisZ = desiredZ ;		/* the user's choice of depth */
			if ( display->thisZ <= 0 ) display->thisZ = 1 ;
			if ( display->thisZ > display->maxZ ) display->thisZ = display->maxZ ;

			get_Visual( display ) ;
			detail.background_pixel = 0 ;
			detail.bit_gravity = NorthWestGravity ;
			wa_mask = CWBackPixel | CWBitGravity ;

			display->this_window = XCreateWindow( display->this_display,
							RootWindow( display->this_display,display->this_screen ),
							0,0,1,1,BORDERWIDTH,
							display->maxZ,InputOutput,display->this_visual,wa_mask,&detail ) ;

			/* create GC for text and drawing */
			get_GC( display ) ;
			
			display->xhot = 0 ;
			display->yhot = 0 ;

			new_pixmap = XCreatePixmapFromBitmapData( display->this_display,
						display->this_window,initial_cursor_data,16,16,1,0,1 ) ;

			display->visible_cursor = XCreatePixmapCursor( display->this_display,new_pixmap,new_pixmap,
							&display->ForeClr,&display->BackClr,display->xhot,display->yhot ) ;
			XFreePixmap( display->this_display,new_pixmap ) ;

			display->blank_cursor = display->visible_cursor ;
			display->cursor_id = display->visible_cursor ;

			XDefineCursor( display->this_display,display->this_window,display->cursor_id ) ;

			/* create pixmap of depth 1 (bitmap) for icon */

			display->this_icon = XCreateBitmapFromData( display->this_display,display->this_window,
								icon_bitmap_bits,icon_bitmap_width,icon_bitmap_height ) ;

			/* work out what kind of colourmap we might have - install it if nec. */
			get_Cmap( display ) ;

			/* Now get the number of logical / physical buttons available */
			display->noButtons = XGetPointerMapping( display->this_display,buttonMap,nmap ) ;
			if ( display->noButtons > 5 ) display->noButtons = 5 ;	/* Stick to 5 max */

			display->nextLocator = 0 ;
			display->freeLocator = 0 ;
			display->inSync = MYFALSE ;
			display->nextKeySym = 0 ;
			display->freeKeySym = 0 ;

			display->mapped = MYFALSE ;		/* we will map the window later on */
			display->frozen = MYFALSE ;

			windows[ new_window & WINMASK ] = display ;

			/* select event types wanted */
			XSelectInput( display->this_display,display->this_window,ExposureMask | KeyPressMask | OwnerGrabButtonMask |
					ColormapChangeMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask ) ;
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( new_window ) ;
}

int D_setScreen( int win_id,int Xpos,int Ypos,int Xdim,int Ydim,int Zdim,int SLlen,int Format,char **offsets )
{
	int i ;
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	if ( Format == PIXELSFORMAT || Format == BITMAPFORMAT ) return( -1 ) ;	/* dont bother with pixels or bitmap format */
	
	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;		/* give up if the display doesnt exist */

	if ( display->mapped )
	{
		if ( display->thisX == Xdim && display->thisY == Ydim &&
			display->thisZ == Zdim && display->thisSL == SLlen )
		{
			canPing = MYFALSE ;
			for ( i = 0 ; i < display->thisZ ; i++ )		/* create a new set of Ximages with new data fields */
			{
				XImage *Ximage ;

				Ximage = XCreateImage( display->this_display,display->this_visual,1,XYPixmap,0,offsets[ i ],
								display->thisX,display->thisY,32,display->thisSL )  ;
				if ( Ximage == ( XImage * ) 0 )
				{
					canPing = MYTRUE ;
					return( -1 ) ;
				}
#ifdef	LSBFIRST
				Ximage->byte_order = LSBFirst ;
#else
				Ximage->byte_order = MSBFirst ;
#endif	LSBFIRST
				Ximage->bitmap_bit_order = MSBFirst ;
				Ximage->bitmap_unit = 32 ;
				XFree( (char *) display->this_image[ i ] ) ;	
				display->this_image[ i ] = Ximage ;
			}

			if ( !display->frozen || display->refresh )		/* if not frozen - refresh entire screen */
			{							/* if frozen & out of date - refresh entire screen */
				display->frozen = MYFALSE ;
				XRefresh( display,0,display->thisX - 1,0,display->thisY - 1,0,display->thisZ - 1,GXcopy ) ;
			}
			canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
		}
	} else
	{
		XSizeHints size_hints ;
		XWMHints wm_hints ;
		XEvent report ;

		if ( Zdim > MAXZ ) Zdim = MAXZ ;			/* limit the max depth of screen used */
		if ( Zdim > display->maxZ ) Zdim = display->maxZ ;	/* dont use more planes than the window has */

		display->thisX = Xdim ;
		display->thisY = Ydim ;
		display->thisZ = Zdim ;
		display->thisSL = SLlen ;

		/* now setup the pixmaps for fast colour */
		if ( display->maxZ > 1 )
		{
			int i ;

			for ( i = 0 ; i < display->thisZ ; i++ )
			{
				display->pixmaps[ i ] = XCreatePixmap( display->this_display,display->this_window,Xdim,Ydim,1 ) ;
			}
			get_pixmapGC( display ) ;
		}

		canPing = MYFALSE ;

		if ( !XnewImages( display,offsets ) )		/* only complete the process if we can get hold of the images */
		{
			canPing = MYTRUE ;
			return( -1 ) ;
		}

		/* reset the foreground and background pixels in case we have a new depth */

       		XSetForeground( display->this_display,display->this_gc,ForeGround( display ) ) ;
		XSetBackground( display->this_display,display->this_gc,BackGround( display ) ) ;

		/* initialise size hint property for window manager */

		size_hints.flags = PPosition | PSize ;
		size_hints.x = Xpos ;
		size_hints.y = Ypos ;
		size_hints.width = display->thisX ;
		size_hints.height = display->thisY ;

		/* set standard properties for window manager - always before mapping */

		XSetStandardProperties( display->this_display,display->this_window,initial_name,initial_name,
					display->this_icon,( char ** ) 0,0,&size_hints ) ;

		/* Now fix the size to these values - so that window can't change size */
		/* To be on the safe size - trap attempts to change window size and ignore */

		size_hints.flags = USSize | PMinSize ;	/* MaxSize not set so zooming gets handled right */
		size_hints.width = display->thisX ;
		size_hints.height = display->thisY ;
		size_hints.max_width = display->thisX ;
		size_hints.max_height = display->thisY ;
		size_hints.min_width = display->thisX ;
		size_hints.min_height = display->thisY ;

		/* Fix standard properties for window manager incl. size, zoom size and input hints */
	
		XSetStandardProperties( display->this_display,display->this_window,initial_name,initial_name,
					display->this_icon,( char ** ) 0,0,&size_hints ) ;

		XSetZoomHints( display->this_display,display->this_window,&size_hints ) ;

		if ( display->subWindow )		/* we dont set all hints for subwindows */
		{
			wm_hints.flags = InputHint | StateHint | IconPixmapHint ;
		} else
		{
			Xdisplay *next ;

			wm_hints.flags = InputHint | StateHint | IconPixmapHint | WindowGroupHint ;
			next = display ;
			while( !next->first ) next = next->next_window ;
			wm_hints.window_group = next->this_window ;	/* so all windows are grouped by display */
		}
		wm_hints.input = 1 ;				/* true => passive input since hint is set */
		wm_hints.initial_state = NormalState ;		/* normal, fully visible window */
		wm_hints.icon_pixmap = display->this_icon ;	/* so the WM picks up on the icon */

		XSetWMHints( display->this_display,display->this_window,&wm_hints ) ;
		display->delete_atom = XInternAtom( display->this_display,"WM_DELETE_WINDOW",False ) ;
		if ( !display->subWindow )
			XSetWMProtocols( display->this_display,display->this_window,&display->delete_atom,1 ) ;

		/* Now draw the window so that we can get the width and height */

		XResizeWindow( display->this_display,display->this_window,display->thisX,display->thisY ) ;

		/* Display Window */
		XMapWindow( display->this_display,display->this_window ) ;

		/* wait until the screen's been exposed - it should then be clear? */
		XMaskEvent( display->this_display,ExposureMask,&report ) ;
		while ( XCheckTypedEvent( display->this_display,Expose,&report ) ) ;

		display->mapped = MYTRUE ;

		XRefresh( display,0,display->thisX - 1,0,display->thisY - 1,0,display->thisZ - 1,GXclear ) ;
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

int D_setCursor( int win_id,int *xdim,int *ydim,int slb,int xhot,int yhot,unsigned char *bits )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	if ( display->mapped )
	{
		int i,j,cxdim,cydim,curs_slb ;
		unsigned char cursor_data[ MAXCURSOR ] ;
		Pixmap new_pixmap ;
		Cursor old_cursor,new_cursor ;

		canPing = MYFALSE ;
									/* now check the cursor is big enough but not too big */
		if ( *xdim < 16 || *ydim < 16 ||
			XQueryBestCursor( display->this_display,display->this_window,*xdim,*ydim,&cxdim,&cydim ) != 0 )
		{
			if ( cxdim > MAXCURSORX ) cxdim = MAXCURSORX ;	/* check the xdim is OK */
			if ( *xdim > cxdim ) *xdim = cxdim ;

			if ( cydim > MAXCURSORY ) cydim = MAXCURSORY ;	/* check the ydim is OK */
			if ( *ydim > cydim ) *ydim = cydim ;

			curs_slb = ( *xdim + 7 ) / 8 ;			/* the cursor scanline is just enough bytes */
			display->xhot = xhot ;
			display->yhot = yhot ;

			for ( i = 0 ; i < *ydim ; i++ )
			{
				unsigned char *src_sl,*dst_sl ;

				src_sl = bits + i * slb ;			/* find the start of the scanline in the source */
				dst_sl = cursor_data + i * curs_slb ;		/* find the start of the scanline in the new cursor */
				for ( j = 0 ; j < curs_slb ; j++ )
				{
#ifdef	LSBFIRST
					dst_sl[ j ] = reversedByte[ src_sl[ j ^ 3 ] ] ;	/* reorder bytes when copied */
#else
					dst_sl[ j ] = reversedByte[ src_sl[ j ] ] ;
#endif	LSBFIRST
				}
			}

			new_pixmap = XCreatePixmapFromBitmapData( display->this_display,
								display->this_window,
								cursor_data,*xdim,*ydim,1,0,1 ) ;
			new_cursor = XCreatePixmapCursor( display->this_display,new_pixmap,new_pixmap,&display->ForeClr,
								&display->BackClr,display->xhot,display->yhot ) ;
			XFreePixmap( display->this_display,new_pixmap ) ;

			old_cursor = display->visible_cursor ;
			display->visible_cursor = new_cursor ;
			if ( display->cursor_id == old_cursor )
			{
				display->cursor_id = new_cursor ;
				XDefineCursor( display->this_display,display->this_window,new_cursor ) ;
			}
			if ( display->blank_cursor != old_cursor )
			{
				XFreeCursor( display->this_display,old_cursor ) ;
			}
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

int D_showCursor( int win_id )				/* make the cursor image visible */
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;
	if ( display->mapped )
	{
		canPing = MYFALSE ;
		if ( display->cursor_id != display->visible_cursor )
		{
			display->cursor_id = display->visible_cursor ;
			XDefineCursor( display->this_display,display->this_window,display->cursor_id ) ;
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

int D_hideCursor( int win_id )				/* make the cursor image visible */
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;
	if ( display->mapped )
	{
		canPing = MYFALSE ;
		if ( display->cursor_id != display->blank_cursor )
		{
			display->cursor_id = display->blank_cursor ;
			XDefineCursor( display->this_display,display->this_window,display->cursor_id ) ;
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

static void updateWindow( Xdisplay *display )			/* this window is no longer uptodate on the display */
{

	if ( display->mapped )
	{
		display->inSync = MYFALSE ;
	}
}

static void syncWindow( Xdisplay *display,int flush )		/* if this window is not uptodate on the display */
{								/* synchronise this display and mark all its windows as such */
	if ( display->mapped && !display->inSync )
	{
		Xdisplay *next ;

		XSync( display->this_display,flush ) ;
		display->inSync = MYTRUE ;
		next = display->next_window ;
		while( next != display )
		{
			next->inSync = MYTRUE ;
			next = next->next_window ;
		}
	}
}

static int readWindow( Xdisplay *display,D_keyboardEvents *buffer,int max )
{
	int n ;

	syncWindow( display,False ) ;

	n = 0 ;
	while( n < max && display->nextKeySym != display->freeKeySym )
	{
		*buffer++ = display->input_buffer[ display->nextKeySym ] ;
		display->nextKeySym = ( display->nextKeySym + 1 ) % INPUTBUFFER ;
		n++ ;
	}
	return( n ) ;
}

int D_readW( int win_id,D_keyboardEvents *buffer,int max )
{
	int nread ;
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	unPingedOps++ ;
	canPing = MYFALSE ;
	nread = readWindow( display,buffer,max ) ;
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( nread ) ;
}

static void locator( Xdisplay *display,int last,D_locatorReply *reply )
{
	int next ;

	syncWindow( display,False ) ;

	if ( display->nextLocator == display->freeLocator )			/* there are no unread locator events - give up */
	{
		reply->dateStamp = -1 ;						/* -1 indicates no input event */
	} else
	{
		next = last ? display->freeLocator - 1 : display->nextLocator ;
		if ( next < 0 ) next = LOCBUFFER - 1 ;

		reply->dateStamp = display->events[ next ].datestamp ;
		reply->xpos = display->events[ next ].xpos ;
		reply->ypos = display->events[ next ].ypos ;
		reply->noButtons = display->noButtons ;
		reply->but1 = display->events[ next ].lflags & LOCBUTTON( 1 ) ? 1 : 0 ;
		reply->but2 = display->events[ next ].lflags & LOCBUTTON( 2 ) ? 1 : 0 ;
		reply->but3 = display->events[ next ].lflags & LOCBUTTON( 3 ) ? 1 : 0 ;
		reply->but4 = display->events[ next ].lflags & LOCBUTTON( 4 ) ? 1 : 0 ;
		reply->but5 = display->events[ next ].lflags & LOCBUTTON( 5 ) ? 1 : 0 ;

		display->nextLocator = ( next + 1 ) % LOCBUFFER ;
	}
}

int D_locatorI( int win_id,D_locatorReply *reply )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	unPingedOps++ ;
	canPing = MYFALSE ;
	locator( display,MYFALSE,reply ) ;
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

int D_locatorOne( int win_id,D_locatorReply *reply )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	unPingedOps++ ;
	canPing = MYFALSE ;
	locator( display,MYTRUE,reply ) ;
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

static void freeWindow( int win_id ) 
{
	Xdisplay *display,*next_window ;

	display = windows[ win_id & WINMASK ] ;
	if ( display != ( Xdisplay * ) 0 && display->window_id == win_id )
	{
		windows[ win_id & WINMASK ] = ( Xdisplay * ) 0 ;				/* dont use it again */

										/* destroy any window specific resources - cursor/cmap */
		if ( display->cmap_id != None )
		{
			XUninstallColormap( display->this_display,display->cmap_id ) ;
			XFreeColormap( display->this_display,display->cmap_id ) ;
		}
		if ( display->visible_cursor != display->blank_cursor )
		{
			XFreeCursor( display->this_display,display->blank_cursor ) ;
		}
		XFreeCursor( display->this_display,display->visible_cursor ) ;
		if ( display->mapped && display->maxZ > 1 )
		{
			int i ;

			for ( i = 0 ; i < display->thisZ ; i++ )
			{
				XFreePixmap( display->this_display,display->pixmaps[ i ] ) ;
			}
			XFreeGC( display->this_display,display->pixmap_gc ) ;
		}
		XFreeGC( display->this_display,display->this_gc ) ;


		next_window = display->next_window ;
		if ( next_window == display )					/* if there are no other windows close the display */
		{
			XCloseDisplay( display->this_display ) ;
		} else
		{
			int regroup,newgroupid ;

			if ( display->first )						/* if this is the first window, mark the next window as first */
			{
				XWMHints wm_hints ;

				next_window->first = MYTRUE ;
				regroup = MYTRUE ;
				newgroupid = next_window->this_window ;
				wm_hints.flags = WindowGroupHint ;
				wm_hints.window_group = newgroupid ;
				XSetWMHints( next_window->this_display,next_window->this_window,&wm_hints ) ;
			} else
			{
				regroup = MYFALSE ;
			}
			while( next_window->next_window != display )		/* find the window before this one in the circular list */
			{
				next_window = next_window->next_window ;
				if ( regroup )					/* update grouping info if necessary */
				{
					XWMHints wm_hints ;

					wm_hints.flags = WindowGroupHint ;
					wm_hints.window_group = newgroupid ;
					XSetWMHints( next_window->this_display,next_window->this_window,&wm_hints ) ;
				}
			}
			next_window->next_window = display->next_window ;	/* pop the deleted window from the circular list */
		}


		if ( display->mapped )						/* if the window is mapped it contains images to be zapped */
		{
			if ( display->this_image != ( XImage ** ) 0 )		/* dispose of the screen images */
			{
				int i ;

				for ( i = 0 ; i < display->thisZ ; i++ )
				{
					XFree( (char *)(display->this_image[ i ]) ) ;	
				}
				free( display->this_image ) ;	
			}
		}
		free( display ) ;						/* dispose of the display itself */
	}
}


int D_closeWindow( int win_id ) 
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	canPing = MYFALSE ;
	if ( win_id == 0 ) parentOfFirst = ( Window ) -1 ;
	XDestroyWindow( display->this_display,display->this_window ) ;	/* zap the window */
	XFlush( display->this_display ) ;				/* immediately */

	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

int D_readWindowName( int win_id,char *name )
{
	int nl ;
	char *c,*oldname ;
	Xdisplay *display ;
	Window w ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	unPingedOps++ ;
	nl = 0 ;
	oldname = ( char * ) 0 ;
	canPing = MYFALSE ;

	w = ( win_id == 0 && parentOfFirst != ( Window ) -1 ) ? parentOfFirst : display->this_window ;
	if ( XFetchName( display->this_display,w,&oldname ) )
	{
		c = oldname ;
		while ( *c != ( char ) 0 )
		{
			*name++ = *c++ ;
			nl++ ;
		}
		XFree( oldname ) ;
	}
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( nl ) ;
}

int D_renameWindow( int win_id,char *newname )
{
	Xdisplay *display ;
	Window w ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	unPingedOps++ ;
	canPing = MYFALSE ;
	w = ( win_id == 0 && parentOfFirst != ( Window ) -1 ) ? parentOfFirst : display->this_window ;
	XStoreName( display->this_display,w,newname ) ;
	XSetIconName( display->this_display,w,newname ) ;
	canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	return( 0 ) ;
}

static int testButtonPress( XButtonEvent *xbutton,int but )
{
	switch( but )
	{
	case 1:	return( ( xbutton->state & Button1Mask ) || ( xbutton->button == Button1 ) ? 1 : 0 ) ;
	case 2:	return( ( xbutton->state & Button2Mask ) || ( xbutton->button == Button2 ) ? 1 : 0 ) ;
	case 3:	return( ( xbutton->state & Button3Mask ) || ( xbutton->button == Button3 ) ? 1 : 0 ) ;
	case 4:	return( ( xbutton->state & Button4Mask ) || ( xbutton->button == Button4 ) ? 1 : 0 ) ;
	case 5:	return( ( xbutton->state & Button5Mask ) || ( xbutton->button == Button5 ) ? 1 : 0 ) ;
	default:	return( 0 ) ;
	}
}

static int testButtonRelease( XButtonEvent *xbutton,int but )
{
	switch( but )
	{
	case 1:	return( ( xbutton->state & Button1Mask ) && ( xbutton->button != Button1 ) ? 1 : 0 ) ;
	case 2:	return( ( xbutton->state & Button2Mask ) && ( xbutton->button != Button2 ) ? 1 : 0 ) ;
	case 3:	return( ( xbutton->state & Button3Mask ) && ( xbutton->button != Button3 ) ? 1 : 0 ) ;
	case 4:	return( ( xbutton->state & Button4Mask ) && ( xbutton->button != Button4 ) ? 1 : 0 ) ;
	case 5:	return( ( xbutton->state & Button5Mask ) && ( xbutton->button != Button5 ) ? 1 : 0 ) ;
	default:	return( 0 ) ;
	}
}

static void read_Xevents( Xdisplay *display )
{
	int i ;

	/*XFlush( display->this_display ) ;*/
	i = XEventsQueued( display->this_display,QueuedAfterFlush ) ;
	while ( i-- > 0 )
	{
		int win_id,n,nextFree ;
		Window w ;
		Xdisplay *next ;
		XEvent report ;

		XNextEvent( display->this_display,&report ) ;
		w = report.xany.window ;
		next = display->next_window ;
		while ( next != display && next->this_window != w )
		{
			next = next->next_window ;
		}
		win_id = next->window_id ;
		if ( w == parentOfFirst || next->this_window == w )
		{
		switch( report.type )
		{
		case ButtonPress:
				nextFree = ( next->freeLocator + 1 ) % LOCBUFFER ;
				if ( nextFree != next->nextLocator )
				{
					next->events[ next->freeLocator ].datestamp = report.xbutton.time ;
					next->events[ next->freeLocator ].xpos = report.xbutton.x ;
					next->events[ next->freeLocator ].ypos = report.xbutton.y ;
					next->events[ next->freeLocator ].lflags = testButtonPress( &report.xbutton,1 ) ? LOCBUTTON( 1 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonPress( &report.xbutton,2 ) ? LOCBUTTON( 2 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonPress( &report.xbutton,3 ) ? LOCBUTTON( 3 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonPress( &report.xbutton,4 ) ? LOCBUTTON( 4 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonPress( &report.xbutton,5 ) ? LOCBUTTON( 5 ) : 0 ;
					next->freeLocator = nextFree ;
				}
				break ;
		case ButtonRelease:
				nextFree = ( next->freeLocator + 1 ) % LOCBUFFER ;
				if ( nextFree != next->nextLocator )
				{
					next->events[ next->freeLocator ].datestamp = report.xbutton.time ;
					next->events[ next->freeLocator ].xpos = report.xbutton.x ;
					next->events[ next->freeLocator ].ypos = report.xbutton.y ;
					next->events[ next->freeLocator ].lflags = testButtonRelease( &report.xbutton,1 ) ? LOCBUTTON( 1 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonRelease( &report.xbutton,2 ) ? LOCBUTTON( 2 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonRelease( &report.xbutton,3 ) ? LOCBUTTON( 3 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonRelease( &report.xbutton,4 ) ? LOCBUTTON( 4 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= testButtonRelease( &report.xbutton,5 ) ? LOCBUTTON( 5 ) : 0 ;
					next->freeLocator = nextFree ;
				}
				break ;
		case ClientMessage:
				if ( report.xclient.data.l[ 0 ] == next->delete_atom )
				{
					/*XDestroyWindow( next->this_display,next->this_window ) ;*/
					kill( 0,SIGQUIT ) ;
				}
				break ;
		case ColormapNotify:
				next->cmapInstalled = report.xcolormap.state == ColormapInstalled ? MYTRUE : MYFALSE ;
				if ( !next->cmapInstalled ) XSync( next->this_display,False ) ;	/* Sync the change just made and */
				break ;
		case ConfigureNotify:		/* window has been resized - fix it !! */
				if ( report.xconfigurerequest.width != next->thisX ||
				     report.xconfigurerequest.height != next->thisY )
				{
					XResizeWindow( next->this_display,w,next->thisX,next->thisY ) ;
				}
				break ;
		case DestroyNotify:		/* window has been zapped - dont use it again */
				freeWindow( win_id ) ;
				return ;	/* give up on the other events - if any */
		case Expose     :
				if ( next->maxZ > 1 )	/* if we have pixmaps - use them for refreshing */
				{
					int j ;

							/* zap the window with the 1st plane - plane 0 */
					XSetPlaneMask( next->this_display,next->this_gc,AllPlanes ) ;
					XSetFunction( next->this_display,next->this_gc,GXcopy ) ;
					XCopyPlane( next->this_display,next->pixmaps[ 0 ],next->this_window,next->this_gc,
						report.xexpose.x,report.xexpose.y,report.xexpose.width,report.xexpose.height,
						report.xexpose.x,report.xexpose.y,1 ) ;

							/* copy each subsequent plane in 2 parts - XCopyPlane doesnt work!! */
					XSetForeground( next->this_display,next->this_gc,AllPlanes ) ;
					XSetBackground( next->this_display,next->this_gc,0 ) ;
					XSetFunction( next->this_display,next->this_gc,GXcopy ) ;
					for ( j = 1 ; j < next->thisZ ; j++ )
					{
						XSetPlaneMask( next->this_display,next->this_gc,1 << j ) ;
						XCopyPlane( next->this_display,next->pixmaps[ j ],next->this_window,next->this_gc,
							report.xexpose.x,report.xexpose.y,report.xexpose.width,report.xexpose.height,
							report.xexpose.x,report.xexpose.y,1 ) ;
/*
						XSetFunction( next->this_display,next->this_gc,GXor ) ;
						XSetForeground( next->this_display,next->this_gc,1 << j ) ;
						XSetBackground( next->this_display,next->this_gc,0 ) ;
						XCopyPlane( next->this_display,next->pixmaps[ j ],next->this_window,next->this_gc,
							report.xexpose.x,report.xexpose.y,report.xexpose.width,report.xexpose.height,
							report.xexpose.x,report.xexpose.y,1 ) ;

						XSetFunction( next->this_display,next->this_gc,GXand ) ;
						XSetForeground( next->this_display,next->this_gc,AllPlanes ) ;
						XSetBackground( next->this_display,next->this_gc,~( 1 << j ) ) ;
						XCopyPlane( next->this_display,next->pixmaps[ j ],next->this_window,next->this_gc,
							report.xexpose.x,report.xexpose.y,report.xexpose.width,report.xexpose.height,
							report.xexpose.x,report.xexpose.y,1 ) ;
*/
					}
					XSetForeground( next->this_display,next->this_gc,ForeGround( next ) ) ;
					XSetBackground( next->this_display,next->this_gc,BackGround( next ) ) ;
				} else
				{
					XRefresh( next,report.xexpose.x,report.xexpose.width + report.xexpose.x - 1,
						report.xexpose.y,report.xexpose.height + report.xexpose.y - 1,
						0,next->thisZ - 1,GXcopy ) ;
				}
				break ;
		case KeyPress:
				nextFree = ( next->freeKeySym + 1 ) % INPUTBUFFER ;
				if ( nextFree != next->nextKeySym )			/* ignore event if buffer is full */
				{
					int ignore ;
					char ascii[ 1 ] ;
					KeySym keySymbol ;

					n = XLookupString( ( XKeyEvent * ) &report,ascii,1,&keySymbol,( XComposeStatus * ) 0 ) ;

					ignore = MYFALSE ;
					if ( n == 1 && ascii[ 0 ] < 32 )
					{
						switch( ascii[ 0 ] )
						{
						case 3:		kill( 0,SIGINT ) ; ignore = MYTRUE ;
								break ;
						case 28:	kill( 0,SIGQUIT ) ; ignore = MYTRUE ;
								break ;
						default:	keySymbol = ascii[ 0 ] ;
						}
					}
					if ( !ignore )
					{
						next->input_buffer[ next->freeKeySym ].datestamp = report.xkey.time ;
						next->input_buffer[ next->freeKeySym ].X11KeySym = keySymbol ;
						next->freeKeySym = nextFree ;
					}
				}
				break ;
		case MappingNotify:
				if ( report.xmapping.request == MappingKeyboard )
				{
					XRefreshKeyboardMapping( ( XMappingEvent * ) &report ) ;
				}
				break ;
		case MotionNotify:
				nextFree = ( next->freeLocator + 1 ) % LOCBUFFER ;
				if ( nextFree != next->nextLocator )
				{
					next->events[ next->freeLocator ].datestamp = report.xbutton.time ;
					next->events[ next->freeLocator ].xpos = report.xbutton.x ;
					next->events[ next->freeLocator ].ypos = report.xbutton.y ;
					next->events[ next->freeLocator ].lflags = report.xbutton.state & Button1Mask ? LOCBUTTON( 1 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= report.xbutton.state & Button2Mask ? LOCBUTTON( 2 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= report.xbutton.state & Button3Mask ? LOCBUTTON( 3 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= report.xbutton.state & Button4Mask ? LOCBUTTON( 4 ) : 0 ;
					next->events[ next->freeLocator ].lflags |= report.xbutton.state & Button5Mask ? LOCBUTTON( 5 ) : 0 ;
					next->freeLocator = nextFree ;
				}
				break ;
		default:	/* all events selected by StructureNotifyMask
				   * except ConfigureNotify are thrown away here,
				   * since nothing is done with them */
				  break ;
		}
		}
	}
}


static int errorHandler( Display *display,XErrorEvent *event )
{
	Xdead = MYTRUE ;		/* so no further display lib calls are made */
	longjmp( ferrorEnv,1 ) ;
					/* ignore errors if longjmp fails - they arent fatal anyway.... */
}

static int ferrorHandler( Display *display )
{
	Xdead = MYTRUE ;	/* so no further display lib calls are made */
	longjmp( ferrorEnv,1 ) ;
	exit( -1 ) ;		/* just give up the ghost in case longjmp fails.....*/
}

static void init_reversedByte(void)
{
	int i ;

	for ( i = 0 ; i < 256 ; i++ )
	{
		reversedByte[ i ] = reverse_bits( i,8 ) ;
	}
}

static counter = 0 ;

void D_initDisplay(void)
{
	int i ;

	if ( Xdead ) return ;			/* we only try this on once */

	for ( i = 0 ; i < MAXOPENWINDOWS ; i++ )
	{
		windows[ i ] = ( Xdisplay * ) 0 ;
	}

	init_reversedByte() ;
	convert_icon() ;

	( void ) XSetErrorHandler( errorHandler ) ;
	( void ) XSetIOErrorHandler( ferrorHandler ) ;

	counter = 0 ;

	canPing = MYTRUE ;			/* display may now be safely pinged */
	unPingedOps = 0 ;
}

void D_shutdownDisplay(void)
{
	int i ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return ;

	canPing = MYFALSE ;
	for ( i = 0 ; i < MAXOPENWINDOWS ; i++ )
	{
		Xdisplay *display ;

		display = windows[ i ] ;
		if ( display != ( Xdisplay * ) 0 )
		{
			if ( i == 0 ) parentOfFirst = ( Window ) -1 ;
			XDestroyWindow( display->this_display,display->this_window ) ;	/* zap the window */
			freeWindow( i ) ;
		}
		windows[ i ] = ( Xdisplay * ) 0 ;
	}
}

void D_pingDisplay(void)
{
	if ( canPing )
	{
		int i ;
		Xdisplay *next ;

		if ( Xdead || setjmp( ferrorEnv ) != 0 ) return ;

		canPing = MYFALSE ;
		counter++ ; counter %= TESTFREQ ;			/* inc the counter and reset at TESTFREQ */
		for ( i = 0 ; i < MAXOPENWINDOWS ; i++ )		/* now check all the open windows */
		{
			next = windows[ i ] ;
			if ( next != ( Xdisplay * ) 0 && next->first )
			{
				if ( !counter )
				{					/* excersise connection every TESTFREQ loops */
					XNoOp( next->this_display ) ;
				}
				read_Xevents( next ) ;
			}
		}
		canPing = MYTRUE ; unPingedOps = 0 ;
	} else
	{
		unPingedOps += MAXUNPINGED ;
	}
}

int D_flushWait( int win_id )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;
	if ( display->mapped )
	{
		canPing = MYFALSE ;
		syncWindow( display,True ) ;
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

int D_pendingChars( int win_id )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;

	if ( display->mapped )
	{
		int num ;

		num = display->freeKeySym - display->nextKeySym ;
		if ( num < 0 ) num += INPUTBUFFER ;
		return( num ) ;
	} else
	{
		return( 0 ) ;
	}
}

int D_unfreeze( int win_id )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;
	if ( display->frozen )
	{
		canPing = MYFALSE ;
		display->frozen = MYFALSE ;
		if ( display->refresh )
		{
			XRefresh( display,0,display->thisX - 1,0,display->thisY - 1,0,display->thisZ - 1,GXcopy ) ;
			updateWindow( display ) ;
		}
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

int D_freeze( int win_id )
{
	Xdisplay *display ;

	if ( Xdead || setjmp( ferrorEnv ) != 0 ) return( -1 ) ;

	unPingedOps++ ;
	display = windows[ win_id & WINMASK ] ;
	if ( display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( -1 ) ;
	if ( !display->frozen )
	{
		canPing = MYFALSE ;
		display->frozen = MYTRUE ;
		display->refresh = MYFALSE ;
		syncWindow( display,False ) ;
		canPing = MYTRUE ; if ( unPingedOps > MAXUNPINGED ) D_pingDisplay() ;
	}
	return( 0 ) ;
}

int D_windowExists( int win_id )
{
	Xdisplay *display ;

	display = windows[ win_id & WINMASK ] ;
	if ( Xdead || display == ( Xdisplay * ) 0 || display->window_id != win_id ) return( MYFALSE ) ;
	return( MYTRUE ) ;
}
