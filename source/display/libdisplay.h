
#define	BITMAPSFORMAT		1
#define	BITMAPFORMAT		2
#define	PIXELSFORMAT		3

typedef struct
{
	int		datestamp ;				/* X server datestamp when event generated */
	int		X11KeySym ;				/* X Key Symbol generated */
} D_keyboardEvents ;

typedef struct							/* pointer info and 5 buttons */
{
	int dateStamp ;
	int xpos ;
	int ypos ;
	int noButtons ;
	int but1 ;
	int but2 ;
	int but3 ;
	int but4 ;
	int but5 ;
} D_locatorReply ;

extern int	D_firstWindow(int,int*,int*,int*) ;
extern int	D_openWindow(char*,int,int*,int*,int*) ;
extern int	D_setScreen(int,int,int,int,int,int,int,int,char**) ;
extern int	D_closeWindow(int) ;
extern int	D_setCursor(int,int*,int*,int,int,int,unsigned char*) ;
extern int	D_showCursor(int) ;
extern int	D_hideCursor(int) ;
extern int	D_readW(int,D_keyboardEvents*,int) ;
extern int	D_locatorI(int,D_locatorReply*) ;
extern int	D_locatorOne(int,D_locatorReply*) ;
extern void	D_raster_op( int*,int,int,int,int,int,int,int*,int,int,int ) ;
extern void	D_raster_vector( int*,int,int,int,int,int,int,int ) ;
extern int	D_queryCmap(int,int,unsigned char*,unsigned char*,unsigned char*) ;
extern int	D_refreshCmap(int,int,unsigned char,unsigned char,unsigned char) ;
extern int	D_refreshRect(int,int,int,int,int,int,int,int) ;
extern int	D_refreshCopy(int,int,int,int,int,int,int,int,int,int,int) ;
extern int	D_refreshLine(int,int,int,int,int,int,int,int,int) ;
extern int	D_flushWait(int) ;
extern int	D_pendingChars(int) ;
extern int	D_readWindowName(int,char*) ;
extern int	D_renameWindow(int,char*) ;
extern void	D_initDisplay(void) ;
extern void	D_shutdownDisplay(void) ;
extern void	D_pingDisplay(void) ;
extern int	D_freeze(int) ;
extern int	D_unfreeze(int) ;
extern int	D_windowExists(int) ;
