#include "display.h"

/* the operators used by the rop functions */
#define	ROPCLEAR		srcw = 0
#define	ROPNOR			srcw = ~( srcw | dstw )
#define	ROPNOTAND		srcw = ~srcw & dstw
#define	ROPNOTCOPY		srcw = ~srcw
#define	ROPANDNOT		srcw = srcw & ~dstw
#define	ROPNOT			srcw = ~dstw
#define	ROPXOR			srcw = srcw ^ dstw
#define	ROPNAND			srcw = ~( srcw & dstw )
#define	ROPAND			srcw = srcw & dstw
#define	ROPNOTXOR		srcw = ~srcw ^ dstw

#define	ROPNOTOR		srcw = ~srcw | dstw
#define	ROPCOPY
#define	ROPORNOT		srcw = srcw | ~dstw
#define	ROPOR			srcw = srcw | dstw
#define	ROPSET			srcw = ~0

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

/* define macros for rop function 0 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_0
#define	GETSOURCE
#define	NEXTSOURCE
#define GETSHIFTEDSOURCE
#define GETDESTINATION
#define	OPERATOR		ROPCLEAR

#include "fropTBLR.h"

/* define macros for rop function 1 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_1
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPNOR
#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_1
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_1
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 2 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_2
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPNOTAND

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_2
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_2
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 3 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_3
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION
#define	OPERATOR		ROPNOTCOPY

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_3
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_3
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 4 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_4
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPANDNOT

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_4
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_4
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 5 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_5
#define	GETSOURCE
#define	NEXTSOURCE
#define GETSHIFTEDSOURCE
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPNOT

#include "fropTBLR.h"

/* define macros for rop function 6 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_6
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPXOR

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_6
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_6
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 7 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_7
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPNAND

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_7
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_7
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 8 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_8
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPAND

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_8
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_8
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 9 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_9
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPNOTXOR

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_9
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_9
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 11 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_11
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPNOTOR

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_11
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_11
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 12 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_12
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION
#define	OPERATOR		ROPCOPY

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_12
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_12
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 13 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_13
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPORNOT

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_13
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_13
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 14 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_14
#define	GETSOURCE		srcw = *nsrc++
#define	NEXTSOURCE		src0 = src1 ; src1 = *nsrc++
#define GETSHIFTEDSOURCE	srcw = ( src0 << Sshift1 ) | ( src1 >> Sshift2 )
#define GETDESTINATION		dstw = *ndst
#define	OPERATOR		ROPOR

#include "fropTBLR.h"

#undef ROPFUNC
#define ROPFUNC	N_ropBTLR_14
#include "fropBTLR.h"

#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#define ROPFUNC	N_ropTBRL_14
#define	GETSOURCE		srcw = *--nsrc
#define	NEXTSOURCE		src1 = src0 ; src0 = *--nsrc
#include "fropTBRL.h"

/* define macros for rop function 15 */
#undef ROPFUNC
#undef GETSOURCE
#undef NEXTSOURCE
#undef GETSHIFTEDSOURCE
#undef GETDESTINATION
#undef OPERATOR

#define ROPFUNC	N_ropTBLR_15
#define	GETSOURCE
#define	NEXTSOURCE
#define GETSHIFTEDSOURCE
#define GETDESTINATION
#define	OPERATOR		ROPSET

#include "fropTBLR.h"

#define BTLR( n ) 	( n + 16 )
#define	TBRL( n ) 	( n + 32 )

void D_raster_op( int *dst,int dslw,int dx,int dy,int w,int h,int rule,int *src,int sslw,int sx,int sy )
{
							/* select the appropriate rop in one switch */
	if ( dst == src )				/* modify rule number for special cases */
	{
		if ( dy > sy ) rule += 16 ;
		if ( dy == sy && dx > sx ) rule += 32 ;
	}
	switch( rule )
	{
	case 0:			N_ropTBLR_0( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 1:			N_ropTBLR_1( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 2:			N_ropTBLR_2( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 3:			N_ropTBLR_3( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 4:			N_ropTBLR_4( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 5:			N_ropTBLR_5( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 6:			N_ropTBLR_6( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 7:			N_ropTBLR_7( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 8:			N_ropTBLR_8( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 9:			N_ropTBLR_9( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 10:		break ;
	case 11:		N_ropTBLR_11( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 12:		N_ropTBLR_12( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 13:		N_ropTBLR_13( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 14:		N_ropTBLR_14( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case 15:		N_ropTBLR_15( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 0 ):		N_ropTBLR_0( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 1 ):		N_ropBTLR_1( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 2 ):		N_ropBTLR_2( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 3 ):		N_ropBTLR_3( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 4 ):		N_ropBTLR_4( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 5 ):		N_ropTBLR_5( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 6 ):		N_ropBTLR_6( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 7 ):		N_ropBTLR_7( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 8 ):		N_ropBTLR_8( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 9 ):		N_ropBTLR_9( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 10 ):	break ;
	case BTLR( 11 ):	N_ropBTLR_11( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 12 ):	N_ropBTLR_12( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 13 ):	N_ropBTLR_13( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 14 ):	N_ropBTLR_14( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case BTLR( 15 ):	N_ropTBLR_15( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 0 ):		N_ropTBLR_0( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 1 ):		N_ropTBRL_1( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 2 ):		N_ropTBRL_2( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 3 ):		N_ropTBRL_3( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 4 ):		N_ropTBRL_4( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 5 ):		N_ropTBLR_5( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 6 ):		N_ropTBRL_6( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 7 ):		N_ropTBRL_7( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 8 ):		N_ropTBRL_8( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 9 ):		N_ropTBRL_9( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 10 ):	break ;
	case TBRL( 11 ):	N_ropTBRL_11( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 12 ):	N_ropTBRL_12( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 13 ):	N_ropTBRL_13( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 14 ):	N_ropTBRL_14( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	case TBRL( 15 ):	N_ropTBLR_15( (unsigned int *) dst,dslw,dx,dy,w,h,(unsigned int *) src,sslw,sx,sy ) ;
				break ;
	}
}

void D_raster_vector( int *dst,int dslw,int x1,int y1,int x2,int y2,int rule,int pixel )
{
	Ndrawline( dst,x1,y1,x2,y2,dslw,pixel,rule ) ;
}

