/* macro definitions for graphics.c */

/* offsets from the end of the class identifier to: */
#define GINSTRC		5			/* mouse.strc  returned by locator */
#define PNTSTRC		6			/* point.strc  returned by cursor.tip */

								/* dimensions of the cursor bitmap */
#define CURXDIM		16
#define CURYDIM		16

								/* bit addressing macros for a scanline */
#define LWRD(x)		( x >> ( psint ) 5 )
#define BIT(x)		( ( psint ) 31 - ( x & ( psint ) 31 ) )
								/* dimensions of the cursor bitmap */

#define PSCMSNAME "SALGOL"					/* name of a colour map segment for a PS progs suntools' window */
#define SVSZDIM        "SDEPTH"                               /* name of shell variable that describes the maximum depth of the screen */
#define DEF_ZDIM        ( ( psint ) 1 )                         /* the default depth is 1 plane */
#define SVSCMAP        "SCMAP"                                /* name of shell variable that describes the maximum depth of the screen */
#define DEF_CMAP        STRUE                                  /* use the default colourmap */
#define ALT_CMAP        SFALSE                                 /* don't use the default colourmap */

								/* names for indexing an image object */
#define BITMAP		1
#define WINDOW		2
#define X_OFFSET	3
#define Y_OFFSET	4
#define X_DIM		5
#define Y_DIM		6

								/* names for indexing a bitmap vector */
#define LWB		1
#define UPB		2
#define DIM_X		3
#define DIM_Y		4
#define OFFSET		5
#define NO_LINES	6
#define BITS		7
