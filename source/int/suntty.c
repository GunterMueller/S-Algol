/**************************************************************************/
/*                                                                        */
/*            SIMPLE TTY EMULATOR FOR PS-algol TEXT OUTPUT                */
/*                                                                        */
/**************************************************************************/

#include "int.h"
#include <sgtty.h>
#include "errno.h"

extern psint Xscreen ;		/* screen device if using X11 */
extern psint Xscr,Yscr ;	/* screen X and Y dimensions */

static struct pixfont *font ;	/* the font being used */
static int op ;			/* ropfn for writing text */
static int cx,mx,cy,my ;	/* current and max x and y char positions */
static int dx,dy ;		/* dimensions of a char */
static int extrax ;		/* number of pixels after last char on a line */
static int extray ;		/* number of pixels below last line on screen */

#define	IBSIZE		1024	/* size of input for buffer */
static char ibuffer[ IBSIZE ] ;	/* input buffer for keyb processing */
static int ib_pos = 0 ;		/* next position in ibuffer */
static int ib_end = 0 ;		/* last position in ibuffer */
static int keybdeof = SFALSE ;	/* flag to indicate eof should be simulated */
static struct sgttyb save_tty ;	/* saved settings for stty */
static struct tchars save_tchars ;	/* saved settings for ^S,^Q */
static struct ltchars save_ltchars ;	/* saved settings for ^V,^Z,^Y */
static int tty_saved = SFALSE ;	/* don't reset tty with gibberish */

static int si_escape = SFALSE ;	/* processing an escape seq. */
static char si_escchars[ 8 ] ;		/* the characters i the sequence */
static int si_nextesc = 0 ;		/* the next escape char to be read */
static int si_echo = SFALSE ;		/* don't echo input from s.i */
static int si_cbreak = SFALSE ;	/* don't block from s.i until '\n' */
static int si_curvis = SFALSE ;	/* is the character cursor visible */
psint si_console = SFALSE ;		/* s.i and s.o are rasters */

static init_input()			/* initialise keyboard read and echo */
{					/* only called if s.o is a raster */
	struct sgttyb iocmd ;
	struct tchars newtchars ;
	struct ltchars newltchars ;
	char *myterm ;			/* name of my login tty */
	extern char *ttyname() ;

	myterm = ttyname( 0 ) ;		/* find name of my tty */
					/* is s.i the console */
	if ( myterm != 0 )
	{
		si_console = STRUE ;

		if ( ioctl( 0,TIOCGETP,&save_tty ) != 0 )	/* save the current tty state */
			error( "unable to read tty mode for s.i" ) ;
		if ( ioctl( 0,TIOCGETC,&save_tchars ) != 0 )	/* save the current ^S,^Q */
			error( "unable to read tty mode for s.i" ) ;
		if ( ioctl( 0,TIOCGLTC,&save_ltchars ) != 0 )	/* save the current ^V,^Z,^Y */
			error( "unable to read tty mode for s.i" ) ;
		tty_saved = STRUE ;

		if ( ioctl( 0,TIOCGETP,&iocmd ) != 0 )		/* read the current tty state */
			error( "unable to read tty mode for s.i" ) ;

		iocmd.sg_flags |= ( short ) CBREAK ;		/* switch on cbreak */
		iocmd.sg_flags &= ~( ( short ) ECHO ) ;		/* switch off echo */
		if ( ioctl( 0,TIOCSETN,&iocmd ) != 0 )
			error( "unable to change tty mode for s.i" ) ;

		if ( ioctl( 0,TIOCGETC,&newtchars ) != 0 )
			error( "unable to read tty mode for s.i" ) ;
		newtchars.t_startc = '\377' ;			/* disable chars trapped by cbreak */
		newtchars.t_stopc = '\377' ;
		if ( ioctl( 0,TIOCSETC,&newtchars ) != 0 )
			error( "unable to change tty mode for s.i" ) ;

		if ( ioctl( 0,TIOCGLTC,&newltchars ) != 0 )
			error( "unable to read tty mode for s.i" ) ;
		newltchars.t_suspc = '\377' ;			/* disable chars trapped by cbreak */
		newltchars.t_dsuspc = '\377' ;
		newltchars.t_flushc = '\377' ;
		newltchars.t_lnextc = '\377' ;
		if ( ioctl( 0,TIOCSLTC,&newltchars ) != 0 )
			error( "unable to change tty mode for s.i" ) ;

		si_cbreak = save_tty.sg_flags & ( short ) CBREAK ? STRUE : SFALSE ;
		si_echo = save_tty.sg_flags & ( short ) ECHO ? STRUE : SFALSE ;
	}
}

reset_input()
{						/* reset the tty mode for s.i */
	if ( Xscreen != -1 )
	{
		D_resetServer() ;		/* in case we're in the middle of some OP */
	}
	if ( tty_saved )
	{
		ioctl( 0,TIOCSETP,&save_tty ) ;
		ioctl( 0,TIOCSETC,&save_tchars ) ;
		ioctl( 0,TIOCSLTC,&save_ltchars ) ;
	}
}

/****************************************************/
/*         RASTER OUTPUT TO s.o ( A RASTER )        */
/****************************************************/

init_pstty()
{
	if ( Xscreen != ( psint ) -1 )
	{
		si_console = STRUE ;
		si_echo = STRUE ;
		si_cbreak = SFALSE ;
	} else
	init_input() ;				/* set up the echoing of s.i */

	font = pf_default() ;
	if ( font == ( struct pixfont * ) 0 )
		error( "cannot open the default system font" ) ;

	dx = font->pf_defaultsize.x ;
	dy = font->pf_defaultsize.y ;

	mx = Xscr / dx - 1 ;	/* max x char position */
	my = Yscr / dy - 1 ;	/* max y char position */
	cx = 0 ; cy = 0 ;	/* start at bottom of screen */
	extrax = Xscr - mx * dx ;	/* extra pixels after last char on a line */
	extray = Yscr - my * dy ;	/* extra pixels below last line on screen */

	op = PIX_SRC ;				/* destructive write */
}

static HideCharCursor()
{
	int xpos,ypos ;

	if ( si_curvis )
	{
		si_curvis = SFALSE ;
		xpos = cx * dx ;
		ypos = cy * dy ;
		ps_scrop( xpos,ypos,dx,dy,PIX_NOT( PIX_SRC ),0,xpos,ypos ) ;
	}
}

static ShowCharCursor()
{
	int xpos,ypos ;

	if ( !si_curvis )
	{
		si_curvis = STRUE ;
		xpos = cx * dx ;
		ypos = cy * dy ;
		ps_scrop( xpos,ypos,dx,dy,PIX_NOT( PIX_SRC ),0,xpos,ypos ) ;
	}
}

static clear()
{
	ps_scrop( 0,0,Xscr,Yscr,PIX_SRC ^ PIX_DST,0,0,0 ) ;
}

static flash()
{
	ps_scrop( 0,0,Xscr,Yscr,PIX_NOT( PIX_SRC ),0,0,0 ) ;
	ps_scrop( 0,0,Xscr,Yscr,PIX_NOT( PIX_SRC ),0,0,0 ) ;
}

printtab()
{
	int nspaces ;

	nspaces = 8 - cx % 8 ;
	if ( cx + nspaces > mx ) nspaces = mx - cx + 1 ;
	while( nspaces-- > 0 ) psrop_char( 32 ) ;	/* expand tab to spaces */
}

static rlfeed()
{
	if ( cy > 0 ) cy-- ; else		/* go to next line */
	{					/* scroll backwards */
		ps_scrop( 0,dy,Xscr,Yscr - dy,PIX_SRC,0,0,0 ) ;
		ps_scrop( 0,0,Xscr,dy,PIX_SRC ^ PIX_DST,0,0,0 ) ;
		if ( extray > 0 )
		ps_scrop( 0,Yscr - extray,Xscr,extray,PIX_SRC ^ PIX_DST,0,0,Yscr - extray ) ;
	} 
}

static lfeed()
{
	if ( cy < my ) cy++ ; else		/* go to next line */
	{					/* scroll */
		ps_scrop( 0,0,Xscr,cy * dy,PIX_SRC,0,0,dy ) ;
		ps_scrop( 0,cy * dy,Xscr,dy + extray,PIX_SRC ^ PIX_DST,0,0,cy * dy ) ;
	} 
}

int number( s,l )
char *s ; int l ;
{
	int res = 0 ;

	while( l-- > 0 ) res = res * 10 + *s++ ;
	return( res ) ;
}

do_escape()
{
	int i ;

	switch( *si_escchars )
	{
	case 'A':
		if ( cy > 0 ) cy-- ;
		break ;
	case 'B':
		if ( cy < my ) cy++ ;
		break ;
	case 'C':
		if ( cx < mx ) cx++ ;
		break ;
	case 'D':
		if ( cx > 0 ) cx-- ;
		break ;
	case 'E':		/* enable echo mode */
		si_echo = STRUE ;
		break ;
	case 'I':		/* enable cbreak mode */
		si_cbreak = STRUE ;
		break ;
	case 'N':		/* disable echo mode */
		si_echo = SFALSE ;
		break ;
	case 'P':		/* position the char cursor */
		if ( si_nextesc != 7 ) return ;
				/* <esc>Pxxxyyy */
		for( i = 1 ; i <= 6 ; i++ )
		{
			char c ;

			c = si_escchars[ i ] ;
			if ( ( c < '0' ) || ( c > '9' ) )
			{	/* ignore if non digits found */
				si_escape = SFALSE ;
				return ;
			}
			si_escchars[ i ] = c - '0' ;
		}
				/* position the char cursor */
		cx = number( si_escchars + 1,3 ) ;
		if ( cx > mx ) cx = mx ;
		cy = number( si_escchars + 4,3 ) ;
		if ( cy > my ) cy = my ;
		break ;
	case 'W':		/* disable cbreak mode */
		si_cbreak = SFALSE ;
		break ;
	default:
		si_escape = SFALSE ;
		psrop_char( *si_escchars ) ;
	}
	si_escape = SFALSE ;
}

static psrop_char( c )
char c ;
{
	if ( si_escape )
	{
		check_startstop() ;
		si_escchars[ si_nextesc++ ] = ( char ) c ;
		do_escape() ;
	} else
	{
		struct pixchar *the_char ;

		switch( ( int ) c )	/* control character? */
		{
		case 7:
			check_startstop() ;
			flash() ;	/* visual bell */
			break ;
					/* backspace */
		case 8:
			check_startstop() ;
			if ( cx > 0 ) cx-- ;
			break ;
		case 9:
			check_startstop() ;
			printtab() ;	/* tab - generate spaces */
			break ;
		case 10:		/* new line */
			check_startstop() ;
			cx = 0 ;	/* start new line */
			lfeed() ;
			break ;
		case 11:		/* reverse new line */
			check_startstop() ;
			cx = 0 ;	/* start new line */
			rlfeed() ;
			break ;
		case 12:		/* form feed - clear & home */
			check_startstop() ;
			cx = 0 ; cy = 0 ;
			clear() ;
			break ;
		case 13:		/* carriage return */
			check_startstop() ;
			cx = 0 ;	/* back to start of line */
			break ;
		case 27:		/* escape - sequence ? */
			check_startstop() ;
			si_escape = STRUE ;
			si_nextesc = 0 ;
			break ;
		default:
			the_char = font->pf_char + ( int ) c ;
			if ( the_char->pc_pr != 0 )
			{
				int xpos,ypos ;

				xpos = cx * dx ;
				ypos = cy * dy ;
				ps_scrop( xpos,ypos,dx,dy,op,the_char->pc_pr,0,0 ) ;
				cx++ ;
				if ( cx > mx )		/* past end of line */
				{
					check_startstop() ;
					cx = 0 ;	/* start new line */
					lfeed() ;
				}
			}
		}
	}
}

psrop_str( s,len )
char *s ; psint len ;
{
	psint cv ;

	cv = si_curvis ;
	if ( cv ) HideCharCursor() ;
	check_startstop() ;
	while( len-- > ( psint ) 0 ) psrop_char( *s++ ) ;
	if ( cv ) ShowCharCursor() ;
}

/****************************************/
/*     ^S,^Q PROCESSING FOR s.i/s.o     */
/****************************************/

check_startstop()			/* wait for any pending ^S to have ^Q typed */
{					/* NB will never stop if input buffer is full of unread chars */
	int nchars ;

	if ( si_console )
	{
		if ( Xscreen != -1 )
		{
			if ( D_pendingChars( Xscreen ) > 0 ) read_chars( SFALSE ) ;
		} else
		if ( ioctl( 0,FIONREAD,&nchars ) == 0 )	/* if FIONREAD doesn't work don't bother */
		{
			if ( nchars > 0 )		/* if there are chars yet to be read - read them! */
				read_chars( SFALSE ) ;	/* read_chars will test for ^S and take appropriate action */
							/* we don;t care if any chars are read after a ^Q */
		}
	}
}

/****************************************/
/*     INPUT PROCESSING FOR s.i         */
/****************************************/

int tty_empty()					/* is the I/O buffer empty? */
{
	if ( Xscreen != ( psint ) -1 )
	{
		if ( ib_pos != ib_end ) return( SFALSE ) ; else
		return( D_pendingChars( Xscreen ) == 0 ) ;
	} else
	return( ib_pos == ib_end ) ;		/* return true if buffer empty */
}

static read_chars( more )		/* read any characters on s.i - preserving unread chars */
psint more ;				/* must at least one char be read if possible */
{
	int sz,nchars ;
	psint suspended ;
	char *base,*pos ;

	if ( ib_pos == ib_end )		/* is the buffer empty */
	{
		ib_pos = 0 ;		/* YES, so reset the pos and end pointers */
		ib_end = 0 ;
	} else
	{				/* preserve any unread characters in the buffer */
		if ( ib_pos > 0 )	/* if ib_pos ~= 0 the chars must be moved */
		{
			sz = ib_end - ib_pos ;	/* the number of characters to save */
			ib_end = 0 ;		/* use ib_end to copy chars - will be one past last char copied */
			while( sz-- < 0 )	/* copy the chars */
				ibuffer[ ib_end++ ] = ibuffer[ ib_pos++ ] ;
			ib_pos = 0 ;		/* reset ib_pos */
		}
	}

	suspended = SFALSE ;			/* read is never entered waiting for ^S/^Q */
	base = ibuffer + ib_pos ;		/* init the pntr base */
	do
	{
		nchars = sizeof( ibuffer ) - ib_end ;	/* the max chars to read */
							/* read up to the size of the buffer */
		if ( keybdeof || ( nchars == 0 ) ) sz = 0 ; else
		{
			if ( Xscreen != ( psint ) -1 ) sz = D_readOneW( Xscreen,ibuffer + ib_end,nchars ) ; else
			 sz = read( 0,ibuffer + ib_end,nchars ) ;
		}
		while( sz < 0 )
		{
			if ( errno != EINTR ) error( "read failed on s.i" ) ;
			if ( Xscreen != ( psint ) -1 ) sz = D_readOneW( Xscreen,ibuffer + ib_end,nchars ) ; else
			sz = read( 0,ibuffer + ib_end,nchars ) ;
		}
					
		if ( sz == 0 )				/* don't wait for more chars if at eof */
		{
			suspended = SFALSE ;
			more = SFALSE ;
		} else
		{					/* check the new chars for ^S, ^Q */
			pos = ibuffer + ib_end ;	/* next character to be checked */
			while( sz-- > 0 )
			if ( suspended )		/* already waiting for ^Q */
			{
				if ( *pos++ == ( char ) 17 )	/* ^Q */
				{
					nchars = sz ;		/* number of chars to copy */
					while( nchars-- > 0 )
						*base++ = *pos++ ;	/* copy the chars */
					suspended = SFALSE ;
				}
			} else
			{				/* nothing special */
				switch( ibuffer[ ib_end++ ] )
				{
				case 17:	/* ^Q */
					ib_end-- ;		/* ignore ^Q */
					break ;
				case 19:	/* ^S */
					pos = ibuffer + ib_end ;	/* pos of next char to be checked */
					ib_end-- ;			/* move ib_end back one to discard ^S */
					base = ibuffer + ib_end ;	/* dest of chars to be copied */
					suspended = STRUE ;
					break ;
				default:
					more = SFALSE ;	/* have read something - so OK to stop */
				}
			}
		}
	} while( suspended || more ) ;
}

psint psrd_str( s,len )			/* process up to len input chars */
char *s ; psint len ;
{
	psint rd_allowed ;		/* are calls to read(2) allowed */
	char *c ;			/* current position in s */

	c = s ;				/* start c at beginning of s */

	if ( si_echo )			/* if echoing input text - display a character cursor() */
		ShowCharCursor() ;

					/* a read(2) is allowed if waiting for '\n' or */
					/* the input buffer is empty */
	rd_allowed = !si_cbreak || ( ib_pos == ib_end ) ;

	while( STRUE )				/* loop until '\n' breaks out or no chars read */
	{					/* is at end of buffer do a read */
		if ( ib_pos == ib_end )
		{
			if ( rd_allowed )
			{			/* disable reads if in cbreak */
				rd_allowed = !si_cbreak ;
				read_chars( STRUE ) ;	/* do the read & test for eof - must read a char after ^Q */
				if ( ib_pos == ib_end )
				{
					HideCharCursor() ;
					return( ( psint )( c - s ) ) ;
				}
			} else
			{			/* switch off the cursor */
				HideCharCursor() ;
						/* return the whole buffer */
				return( ( psint )( c - s ) ) ;
			}
		}

		if ( len-- > ( psint ) 0 )
		{
			char ch ;

			ch = ibuffer[ ib_pos++ ] ;
			if ( si_cbreak )
			{
				*c++ = ch ;
				if ( si_echo ) echo_cstr( &ch,1 ) ;
			} else
			{
				switch( ch )
				{
				case 4:	keybdeof = STRUE ;
					ib_pos = ib_end ;
					HideCharCursor() ;
					return( ( psint )( c - s ) ) ;
				case 10:
				case 13:
					*c++ = ch ;
					if ( si_echo ) echo_cstr( &ch,1 ) ;
					HideCharCursor() ;
					return( ( psint )( c - s ) ) ;
				case 18:
					if ( si_echo )
					{
						echo_cstr( "\n",1 ) ;
						echo_cstr( s,c - s ) ;
					}
					len++ ;		/* ignore this char */
					break ;
				case 127:
					if ( c > s )
					{
						c-- ;		/* pop last byte from buffer */
						len += 2 ;	/* inc num bytes to read */
								/* erase last typed char */
						if ( si_echo )
						{
							if ( *c == 9 )	/* was it a tab ? */
							{		/* YES, so redraw the entire line */
								echo_cstr( "\n",1 ) ;
								echo_cstr( s,c - s ) ;	
							} else
							{		/* NO, so zap a single char */
								echo_cstr( "\b \b",3 ) ;
							}
						}
					} else len++ ;		/* ignore this char */
					break ;
				default:
					*c++ = ch ;
					if ( si_echo ) echo_cstr( &ch,1 ) ;
				}
			}
		} else
		{					/* read the desired no. of bytes */
			if ( si_cbreak )
			{
				HideCharCursor() ;	/* switch off the character cursor */
				return( ( psint )( c - s ) ) ;
			}

							/* blocking condition in place - no '\n' */
			if ( ibuffer[ ib_pos++ ] == 127 )
			{
				if ( c > s )
				{
					c-- ;		/* pop last byte from buffer */
					len += 2 ;	/* inc num bytes to read */
							/* erase last typed char */
					if ( si_echo )
					{
						if ( *c == 9 )	/* was it a tab ? */
						{		/* YES, so redraw the entire line */
							echo_cstr( "\n",1 ) ;
							echo_cstr( s,c - s ) ;	
						} else
						{		/* NO, so zap a single char */
							echo_cstr( "\b \b",3 ) ;
						}
					}
				} else len++ ;		/* ignore this char */
			} else len++ ;			/* ignore any other chars */
		}
	}
}
