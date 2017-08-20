#include "errno.h"
#include "libdisplay.h"

#define	BIT31			( 1 << 31 )
#define	MAXFNAME		2048						/* max number of chars in a file/display name */
#define	MAXOFFSETS		25						/* max number of offsets in setScreen: 24 planes + Cmap */

#define	MYFALSE			0
#define	MYTRUE			1

#define	WINPWROF2	( 6 )					/* allow for upto 64 w & WINMASKindows */
#define	MAXOPENWINDOWS	( 1 << WINPWROF2 )			/* 64 windows */
#define	WINMASK		( MAXOPENWINDOWS - 1 )			/* need mask for finding the window table index */
#define	LOCBUFFER	4096					/* up to 4K locator events are remembered */
#define	LOCFREE		( 1 )					/* mask to mark locator buffer entry as free */
#define	LOCBUTTON( n )	( 1 << n )				/* mask for locator button n - in the flags field of the loc buffer */
#define	INPUTBUFFER	4096					/* up to 4K key stroke events are remembered */
#define	BORDERWIDTH	4
#define	CURSORSIZE	( 16 / 8 * 16 )				/* size of a cursor in bytes */
#define	MAXZ		( 8 )					/* the deepest window we will accept */
#define	MAXCMAP		( 256 * 3 ) 				/* 256 entries depths 1 -> 8, 3 * 256 for depth 8+ (direct colour) */
#define	TESTSECS	5					/* perform a test every 5 seconds */
#define	LOOPWAIT	100000					/* approximately 1 loop every 100millsecs */
#define	TESTFREQ	( 1000000 / LOOPWAIT * TESTSECS )	/* the number of loops to obtain TESTSECS */
#define	CMAPFREQ	( 5 )					/* the number of cmap changes between sigpauses */
#define	FBREFRESH	( 150 )					/* the number of mouse reads between FB refreshs */
#define	MAXUNPINGED	( 10 )					/* max. number of ops between ping's */
#define	MAXCURSORX	( 256 )					/* assume max cursor xdim is 256 pixels */
#define	MAXCURSORY	( 256 )					/* assume max cursor ydim is 256 pixels */
#define	MAXCURSOR	( ( MAXCURSORX / 8 ) * MAXCURSORY )	/* assume max cursor is 256 x 256 pixels */

extern void Ndrawline( int *,int,int,int,int,int,int,int ) ;

