/*
	INDIRECTION - all local heap addresses are relative to heap_base
	PAM32 - 32bit SAM
	PAM64 - 64bit SAM 
	ADDR32 - 32bit addresses
	ADDR64 - 64bit addresses
	LITTLEENDIAN - bytes within words run from least to most significant e.g. VAX, Alpha,...
	BIGENDIAN - bytes within words run from most to least significant e.g. 68K, Sparc,...

	****  ADDR size should be specified in the Makefiles
	****  ENDIANNESS should be specified in the Makefiles

*/

#define	SAM32


#ifndef	SAM32
#ifndef	SAM64
#error S-algol abstract machine pointer size not specified in int.h
#endif	/*PAM64*/
#endif	/*PAM32*/

#ifndef	ADDR32
#ifndef	ADDR64
#error Address size not specified in int.h
#endif	/*ADDR64*/
#endif	/*ADDR32*/

#ifndef	LITTLEENDIAN
#ifndef	BIGENDIAN
#error Byte ordering must be specified as either little endian or big endian
#endif	/*BIGENDIAN*/
#endif	/*LITTLEENDIAN*/

/* the type of 32bit integers ( psint,psuint ) & 64bit real ( psreal ), as required. */

typedef int psint ;
typedef unsigned int psuint ;
typedef	double psreal ;

#ifdef	SAM32
/***************************  32 bit SAM Configuration ************************/

/* check address and postore options are compatible with a 32bit SAM */

#ifdef	ADDR32
/******* 32 bit addresses *********/

typedef int pslong ;				/* int large enough to hold a (char *) */
/*typedef psint *psptr ;				/* assumes pointer fields can be native addresses */
/*#define	mkptr( p )	((psint *) p)		/* make C pointer to a psint - p is a psptr */
/*#define mkpptr( p )	((psptr *) p)		/* make C pointer to a psptr - p is a psptr */
/*#define mkpsptr( p )	((psptr) p)		/* make psptr from a C pointer */
typedef psint psptr ;				/* assumes pointer fields are 32bit storeBase relative offsets in words */
extern psint *storeBase ;			/* hence the external definition */
#define	INDIRECTION				/* so the heap knows what to do */
#define	mkptr( p )	((psint *)(storeBase+(p)))		/* make C pointer to a psint - p is a psptr */
#define mkpptr( p )	((psptr *)(storeBase+(p)))		/* make C pointer to a psptr - p is a psptr */
#define mkpsptr( p )	((psptr)((p)-storeBase))		/* make psptr from a C pointer */
/*
#define	PREFIX		((psint)1)		/* 1 word prefix no alignment constraints */
#define	ROUNDUP( s )	

/**********************************/

#else

/******* 64 bit addresses *********/

typedef long pslong ;				/* int large enough to hold a (char *) */
typedef psint psptr ;				/* assumes pointer fields are 32bit storeBase relative offsets in words */
extern psint *storeBase ;			/* hence the external definition */
#define	INDIRECTION				/* so the heap knows what to do */
#define	mkptr( p )	((psint *)(storeBase+(p)))		/* make C pointer to a psint - p is a psptr */
#define mkpptr( p )	((psptr *)(storeBase+(p)))		/* make C pointer to a psptr - p is a psptr */
#define mkpsptr( p )	((psptr)((p)-storeBase))		/* make psptr from a C pointer */
/*
#define	ROUNDUP( s )	(s = s + (s&1) )	/* round up s to next even number */
#define	ROUNDUP( s )	

/**********************************/
#endif	/*ADDR32*/

#define	ptrwsize	((psint) 1)		/* a 32bit SAM 1 word per pntr */

/******************************************************************************/

#else

/***************************  64 bit SAM Configuration ************************/

#ifdef	ADDR32
#error 64bit SAM not implemented for 32bit addresses
#else
#endif	/*ADDR32*/

typedef long pslong ;				/* int large enough to hold a (char *) */
typedef long pskey ;
typedef psint *psptr ;				/* assumes pointer fields can be native addresses */
#define	mkptr( p )	((psint *) p)		/* make C pointer to a psint - p is a psptr */
#define mkpptr( p )	((psptr *) p)		/* make C pointer to a psptr - p is a psptr */
#define mkpsptr( p )	((psptr) p)		/* make psptr from a C pointer */
#define	PREFIX		((psint)2)		/* 2 word prefix for alignment */
#define	ROUNDUP( s )	(s = s + (s&1) )	/* round up s to next even number */
#define	ptrwsize	((psint) 2)		/* a 64bit PAM 2 words per pntr */

/******************************************************************************/
#endif	/*PAM32*/

#define	plfb		((psptr *) lfb)		/* lfb as a (psptr *) */
#define	ptrsize1	((psint) 1)		/* a 32bit PAM */
#define	ptrsize2	((psint) 2)
#define	ptrsize3	((psint) 3)
#define	ptrsize4	((psint) 4)

#ifdef	LITTLEENDIAN
#define	BYTEORDERMASK	(3)
#define	BYTEORDER( p )	( (unsigned char *)((pslong) p ^ 3) )
/*#define	NEXTSAMOP	{ void (*fn)(void) = opcodes[ *BYTEORDER(pc) ] ; pc++ ; fn() ; } */
#define	THISSAMOP	( *BYTEORDER(pc) )
#define	NEXTBYTE( n )	{ n = *BYTEORDER(pc) ; pc++ ; }
#define	THISBYTE	( *BYTEORDER(pc) )
#define	NEXTPARAM( n )	{ n = *BYTEORDER(pc) << 8 ; pc++ ; n |= *BYTEORDER(pc) ; pc++ ; }
#define	FLOAT0		(1)
#define	FLOAT1		(0)
#define EPSILON0		( ( psint ) 0x3bF00000 )
#define EPSILON1		( ( psint ) 0x00000000 )
#define MAXREAL0		( ( psint ) 0x7FEFFFFF )
#define MAXREAL1		( ( psint ) 0xFFFFFFFF )
#define PI0			( ( psint ) 0x400921FB )
#define PI1			( ( psint ) 0x54524550 )
#else
#define	BYTEORDERMASK	(0)
#define	BYTEORDER( p )	( p ) 
#define	NEXTSAMOP	{ opcodes[ *pc++ ]() ; }
#define	THISSAMOP	( *pc )
#define	NEXTBYTE( n )	{ n = *pc++ ; }
#define	THISBYTE	( *pc )
#define	NEXTPARAM( n )	{ n = *pc++ << 8 ; n |= *pc++ ; }
#define	FLOAT0		(1)
#define	FLOAT1		(0)
#define EPSILON1		( ( psint ) 0x3bF00000 )
#define EPSILON0		( ( psint ) 0x00000000 )
#define MAXREAL1		( ( psint ) 0x7FEFFFFF )
#define MAXREAL0		( ( psint ) 0xFFFFFFFF )
#define PI1			( ( psint ) 0x400921FB )
#define PI0			( ( psint ) 0x54524550 )
#endif	/*LITTLEENDIAN*/

#define	WORDSIZE	( ( psint ) 4 )			/* the size of a WORD -> psint, in bytes */

/* Boolean literals */

#define STRUE		( ( psint ) 1 )
#define SFALSE		( ( psint ) 0 )

/* Standard Identifiers */

#define MAXINT			( ( psint ) 0x7FFFFFFF )
#define MININT			( -MAXINT - ( psint ) 1 )
/*
#define EPSILON0		( ( psint ) 0x3bF00000 )
#define EPSILON1		( ( psint ) 0x00000000 )
#define MAXREAL0		( ( psint ) 0x7FEFFFFF )
#define MAXREAL1		( ( psint ) 0xFFFFFFFF )
#define PI0			( ( psint ) 0x400921FB )
#define PI1			( ( psint ) 0x54524550 )
*/

#define INT_BOOL	( ( psint ) 0 )
#define PNTR		( ( psint ) 4 )
#define REAL		( ( psint ) 1 )
#define PC		( ( psint ) -1 )
#define SL		( ( psint ) -2 )
#define DL		( ( psint ) -3 )
#define PDL		( ( psint ) -4 )
#define RA		( ( psint ) -5 )
#define LINE_NO		( ( psint ) -6 )

#define B_6	( ( psint ) 0100 )		/* bit 6 */
#define B_7	( ( psint ) 0200 )		/* bit 7 */
#define B_12	( ( psint ) 010000 )		/* bit 12 */
#define B_13	( ( psint ) 020000 )		/* bit 13 */
#define B_14	( ( psint ) 040000 )		/* bit 14 */
#define B_15	( ( psint ) 0100000 )		/* bit 15 */
#define B_16	( ( psint ) 0200000 )		/* bit 16 */
#define B_20	( ( psint ) 04000000 )		/* bit 20 */
#define B_28	( ( psint ) 02000000000 )	/* bit 28 */
#define B_29	( ( psint ) 04000000000 )	/* bit 29 */
#define B_30	( ( psint ) 010000000000 )	/* bit 30 */
#define B_31	( ( psint ) 020000000000 )	/* bit 31 */
#define B_0_1	( ( psint ) 03 )		/* bits 0 - 1 */
#define B_0_3	( ( psint ) 017 )		/* bits 0 - 3 */
#define B_0_4	( ( psint ) 037 )		/* bits 0 - 4 */
#define B_0_6	( ( psint ) 0177 )		/* bits 0 - 6 */
#define B_0_7	( ( psint ) 0377 )		/* bits 0 - 7 */
#define B_0_11	( ( psint ) 07777 )		/* bits 0 - 11 */
#define B_0_14	( ( psint ) 077777 )		/* bits 0 - 14 */
#define B_0_15	( ( psint ) 0177777 )		/* bits 0 - 15 */
#ifdef	RASTER
#define B_0_23	( ( psint ) 077777777 )	/* bits 0 - 27 */
#endif	RASTER
#define B_0_27	( ( psint ) 01777777777 )	/* bits 0 - 27 */
#define B_0_30	( ( psint ) 017777777777 )	/* bits 0 - 30 */
#define B_7_15	( ( psint ) 0177600 )		/* bits 7 - 15 */
#define B_7_31	( ( psint ) 037777777600 )	/* bits 7 - 31 */
#define B_8_14	( ( psint ) 077400 )		/* bits 8 - 14 */
#define B_8_31	( ( psint ) 037777777400 )	/* bits 8 - 31 */
#define B_12_14 ( ( psint ) 070000 )		/* bits 12 - 14 */
#define B_14_15 ( ( psint ) 0140000 )		/* bits 14 - 15 */
#define B_29_30 ( ( psint ) 014000000000 )	/* bits 29 - 30 */
#define B_29_31 ( ( psint ) 034000000000 )	/* bits 29 - 31 */
#define B_30_31 ( ( psint ) 030000000000 )	/* bits 30 - 31 */

#ifndef	RASTER
#define BIT(x)	( ( psint ) 31 - ( ( psint )( x ) & ( psint ) 31 ) )		/* shift for bit no. x */
#define LWRD(x)	( x >> 5 )			/* pixel -> long words */

#endif	RASTER

#define MS_SIZE	( ( psint ) 6400 )		/* in words */
#define PS_SIZE	( ( psint ) 6400 )		/* in words */
#define WORK_SIZE ( ( psint ) 512 )		/* in bytes */
#define OVERHEAD ( ( psint ) 4 )		/* in words */
#define BUFF_SIZE ( ( psint ) 512 )		/* in bytes */

#ifndef LOCAL
#define LOCAL extern
#endif
LOCAL unsigned char *pc ;
LOCAL short int *s_base ;
LOCAL psint *flow_vec,*sp,*sf,*sb,*storeBase,*heap_base,*heap_max,*m_top,*p_top ;
LOCAL psptr *psp,*psf,*psb ;
LOCAL psint line_no,flow_sum,*char_tab,intrpt ;
LOCAL psint *process_size ;
LOCAL char work_space[ WORK_SIZE ],**envv ;
LOCAL psint *NIL_PTR ;

/* the following are used in the iliffe routines */

LOCAL psint type,res1,res2 ;

/* The folowing are for the compacting Garbage Collector */

#ifdef	COMPACT
#define MARK_BIT		 ( ( psint ) 1 << 20 )	/* bit 20 */

#define STRING			 ( ( psint ) 8 << 28 )
#define S_FILE			 ( ( psint ) 4 << 28 )
#define STRUCTURE		( ( psint ) 12 << 28 )
#define VEC_PNTRS		 ( ( psint ) 2 << 28 )
#define VEC_INTBS		( ( psint ) 10 << 28 )
#define VEC_REALS		 ( ( psint ) 6 << 28 )
#ifdef	RASTER
#define	IMAGE			( ( psint ) 14 << 28 )
#endif	RASTER
#define TAG_BITS		( ( psint ) 15 << 28 )

LOCAL psint	*heap_p ;			/* used as a stack top */
#endif	COMPACT
#ifdef	PERQ
LOCAL psint	*cur_table ;			/* table of cursor bitmaps */
#endif	PERQ
LOCAL psint	gc_count;			/* useful to know how many */
LOCAL psint	gc_allowed;			/* allow garbage collection? */
#ifdef	RASTOP
#include "graphics.h"
#include "libdisplay.h"
LOCAL psint	inerror ;			/* Error message flag */
#define EXTRACT_BIT( x,y )    ( ( ( psint )( y ) >> ( psint )( x ) ) & ( ( psint ) 1 ) )      /* take xth bit out of y */
LOCAL psint	*cursor ;                       /* Pointer to the cursor */
LOCAL psint	*screen,*scr_bits ;		/* Pointer to the screen + bits */
LOCAL psint	in_pend ;			/* Input pending flag */

#define SAMROP_SRC              (12)
#define SAMROP_DST              (10)
#define SAMROP_NOT(r)           ((~r) & B_0_3)
#define SAMROP_SET              (-1 & B_0_3)
#define SAMROP_CLR              (0)

#define	SGRAPHICS		"S-algol Graphics Window"
#define	CLICKTOQUIT		"Click to Quit"
#endif	RASTOP
LOCAL char	*sigerror ;
#define MAXCHARS                512                                             /* max number of chars returned by a read request */
#define MAXFNAME                2048                                            /* max number of chars in a file/display name */
#define MAXZDIM              8                                              /* max depth of screen */

