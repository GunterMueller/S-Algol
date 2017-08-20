/*
** The HEAP 
**
** The heap looks like a contiguous memory of 32 bit units, all heap
** items are made up from these units - thus every heap item
** consists of a whole number of words.
** String, vectors, structure, and file descriptors live in the heap.
** The heap then consists of the above items, a linked list of
** free areas, and left over units of one word (32 bits). These
** left overs are coallesced during garbage collection.
** Every item is has a leading tag word which describes what it is -
** these tag words are described below.
**
**
** Format of heap items:
**
** note - format is given in bits from start of the heap item ,
**
** 	the maximun size of a vector is limited by the indices only
** 	( in theory at least )
**
** 	the maximum size of a string is 64K chars
**
** 	the maximum size of a file descriptor is 1M bytes
**
** 	the maximum size of a structure is compiler dependent
**
**
**
** string -
** 		0-15	length in chars
** 		16-31	reserved
** 		32 ...	the chars
**
** vectors -
** 		0-31	reserved
** 		32-63	lower bound
** 		64-95	upper bound
** 		96 ...	the elements
**
** structures -
** 		0-15	the trade mark
** 		16-31	reserved
** 		32 ...	the fields
**
** files -
** 		0-31	reserved
** 		32 ...	the descriptor
**
**
**
**
** Detailed definition of header :
**
**
**
** bits in header for S heap item -
**
** 	0-19	key field as above , or don't care
** 	20	mark bit
** 	21-27	unused
** 	28-31	type
** 			 8 string
** 			 4 file descriptor
** 			12 structure
** 			 2 vec with pointers
** 			10 vec of ints or bools
** 			 6 real vector
** 			 7 single ( used for garbage collection )
**			11 bad ( frig for perq )
#ifdef	RASTER
**			14 image
#endif	RASTER
**
** bits in header of free list link -
**
** 	0-27 offset to next link , or 0 for end of list
** 	28-31 type 
*/








#include <stdio.h>
#include "int.h"

#define X_HEAP_SIZE	16384		/* 16 K bytes    */
#define SINGLE		016000000000	/* bits 28 - 30  */
#define MARKED		0
#define FREE		1

int *c_file( size )
register int size ;
{
	register int *ptr ;
	extern int *claim_heap() ;

	ptr = claim_heap( size ) ;
	*ptr = size | B_30 ;
	return( ptr ) ;
}

int *c_v_ib( lwb,upb )
register int lwb ; 
int upb ;
{
	register int *ptr ;
	extern int *claim_heap() ;

	ptr = claim_heap( upb - lwb + 4 ) ;
	*ptr = B_29 | B_31 ;
	ptr[ 1 ] = lwb ;  
	ptr[ 2 ]= upb ;
	return( ptr ) ;
}

int *c_v_r( lwb,upb )
register int lwb ; 
int upb ;
{
	register int *ptr ;
	extern int *claim_heap() ;

	ptr = claim_heap( ( upb - lwb + 1 ) * 2 + 3 ) ;
	*ptr = B_29_30 ;
	ptr[ 1 ] = lwb ; 
	ptr[ 2 ] = upb ;
	return( ptr ) ;
}

int *c_v_p( lwb,upb )	/* claims vector of strings for options */
register int lwb ; 
int upb ;
{
	register int *ptr ;
	extern int *claim_heap() ;

	ptr = claim_heap( upb - lwb + 4 ) ;
	*ptr = B_29 ;
	ptr[ 1 ] = lwb ; 
	ptr[ 2 ] = upb ;
	return( ptr ) ;
}

int *c_structure( tm )
register int tm ;
{
	register int size,*ptr ;
	extern int *claim_heap() ;

	size = s_base[ tm / 2 ] ; 
	ptr = claim_heap( size ) ;
	*ptr = B_30_31 | tm ;
	return( ptr ) ;
}

int *c_string( length )
int length ;
{
	register int size,*ptr ;
	extern int *claim_heap() ;

	size = ( length + 7 ) / 4  ;
	ptr = claim_heap( size ) ;
	*ptr = B_31 | length ;
	return( ptr ) ;
}

#ifdef	RASTER
int *c_image()
{
	register int *ptr ;
	extern int *claim_heap() ;

	ptr = claim_heap( 7 ) ;
	*ptr = B_29 | B_30 | B_31 ;
	return( ptr ) ;
}
#endif	RASTER


mark( pitem )		/* Not so recursive marking algorithm */
int pitem ;
{
	int i,*ptr,nptrs,*p ;

	while ( pitem && ( ( *(p = mkptr( pitem ) ) & B_20 ) == 0 ) )		/* unmarked and not nil  */
	{
		*p |= B_20 ;			/* mark it */
		if ( ( *p & B_29_31 ) == B_29 )	 /* a vector of pntr's */
		{
			ptr = p + 3 ; 
			nptrs = p[ 2 ] - p[ 1 ] + 1 ;
			for ( i = 1 ; i < nptrs ; i++ ) mark( *ptr++ ) ;
			pitem = *ptr ;
		}
#ifdef	RASTER
		else
			if ( ( *p & B_29_31 ) == B_29_31 ) /* An image */
			{
				ptr = p + 1 ;
				mark( *ptr++ ) ;
				pitem = *ptr ;
			}
#endif	RASTER
		else
			if ( ( ( *p & B_29_31 ) == B_30_31 ) &&
			    ( nptrs = s_base[ ( *p & B_0_15 ) / 2 + 1 ] ) ) /* a structure containing pointer(s) */
			{
				ptr = p + 1 ;
				for ( i = 1 ; i < nptrs ; i++ ) mark( *ptr++ ) ;
				pitem = *ptr ;
			}
	}
	return ;
}

#ifndef	RASTER
expand_heap()
{
	psint *new_size ;

	new_size = heap_max + X_HEAP_SIZE ;
	if ( new_size < process_size )
#ifdef	COMPACT
	{
		heap_max = new_size ;
		/* write( 2,"\nheap expanded",14 ) ; */
	}
#else

	{
		int *ptr ;

		ptr = heap_max ;
		ptr = mkptr( heap_base[ 1 ] ) ;		/* start of free list */
		while ( ptr[ 1 ] ) ptr = mkptr( ptr[ 1 ] ) ;
		if ( ptr + *ptr == heap_max ) /* free from ptr to heap_max */
		{
			ptr[ 1 ] = mkpsptr( new_size ) ;
			*ptr = *ptr + new_size - heap_max ;
		}
		else
		{
			ptr = heap_max ;
			*ptr = new_size - heap_max ;
			ptr[ 1 ] = mkpsptr( new_size ) ;
		}
		heap_max = new_size ;
		/* write( 2,"\nheap expanded",14 ) ; */
	}
#endif	COMPACT
}
#endif	RASTER

int *claim_heap( size )
int size ;
{
	register int *ptr ;
	extern int *claim() ;
	if ( !( ptr = claim( size ) ) )
	{
		garb_col() ;
#ifdef	RASTER
		if( !( ptr = claim( size ) ) )
			error( "Heap overflow" ) ;
#else
		if( !( ptr = claim( size ) ) ) 
		{
			expand_heap() ;
			if( !( ptr = claim( size ) ) )
				error( "Heap overflow" ) ;
		}
#endif	RASTER
	}
	return( ptr ) ;
}

int *claim( size )
register int size ;
{
#ifdef	COMPACT
	register int *res = heap_p ;

	heap_p += size ;
	if ( heap_p >= heap_max )
	{
		heap_p -= size ;
		return( 0 ) ;
	} else return( res ) ;
#else
	register psint *ptr,*pptr,tmp ;

	ROUNDUP( size ) ;
	pptr = heap_base ;
	tmp = heap_base[ 1 ] ;
	while ( tmp )
	{
		ptr = mkptr( tmp ) ;
		if ( *ptr < size )
		{ /* too small - jump over it */
			pptr = ptr ;
			tmp = ptr[ 1 ] ;
		}
		else
			if ( *ptr == size )
			{ /* exact fit - remove from free list */
				pptr[ 1 ] = ptr[ 1 ] ;
				return( ptr ) ;
			}
			else
				if ( *ptr == ( size + 1 ) )
				{ /* one left - remove from free list, mark left over word */
					pptr[ 1 ] = ptr[ 1 ] ;
					*ptr++ = SINGLE ;
					return( ptr ) ;
				}
				else
				{ /* too big - pinch top of it */
					pptr = ptr + ( *ptr -= size ) ;
					return( pptr ) ;
				}
	}
	return( 0 ) ;
#endif	COMPACT
}

/*	For emergencies 

check_heap( filename )
char	*filename ;
{
	register int *pfp,*cp ;
	int	tmp,tmp1,no_objs = 0,valid = 1 ;
	FILE 	*f ;

	pfp = heap_base ;
	cp = heap_base + 2 ;

	f = fopen( filename,"w" ) ;
	while ( cp < heap_max  )
	{
		tmp = ( *cp >> 28 ) & 15 ;
		switch( tmp )
		{
			case 2  :
			case 6  :
			case 10 : tmp1 = obj_size( cp ) ;
				  no_objs++ ;
				  fprintf( f,"tmp %d : *cp %x : cp is %x : lwb is %d upb is %d\n",tmp,*cp,(int)cp,cp[ 1 ],cp[ 2 ] ) ;
				  cp += tmp1 ;
			  	  break ;

			case 0  : tmp1 = obj_size( cp ) ;
				  no_objs++ ;
				  fprintf( f,"tmp %d : *cp %x : cp is %x : cp[ 1 ] is %x\n",tmp,*cp,(int)cp,cp[ 1 ] ) ;
				  cp += tmp1 ;
			  	  break ;
			case 4  :
			case 8  :
			case 12 :
			case 14 : tmp1 = obj_size( cp ) ;
				  no_objs++ ;
				  fprintf( f,"tmp %d : *cp %x : cp is %x : obj_no is %x\n",tmp,*cp,(int)cp,no_objs ) ;
				  cp += tmp1 ;
			  	  break ;
	
			case 7 : cp++ ;
				 break ;

			default : tmp1 = obj_size( cp ) ;
				  fprintf( f,"*cp %x : cp is %x\n",*cp,(int)cp ) ;
				  cp += tmp1 ;
		}
	}

	fclose( f ) ;
}
 
*/

garb_col()
{
#ifdef	COMPACT
	register int *reuse_base = psp - 3 ;
	register int *item = reuse_base ;
	register int *shift_addr = reuse_base ;
	register int *dest = 0 ;
#ifdef  X11
        extern psint Xscreen ;
        extern freeze_XScreen(),moved_XImage() ;
#endif  X11

	gc_count++ ;

	mark_all() ; set_up_vector() ;

	/*
	 * SCAN 1 - reverse all backward pointers except those pointing into
	 *          the user code ( ie before psp )
	 */

	*heap_p = 0 ;
	while ( item < heap_p )
	{
		register int nptrs = 0, *ptr = 0 ;
		register int size = obj_size( item ) ;
		int *tmp = item + size ;

		if ( !( *item & MARK_BIT ) )
		{
			/* item not marked so can throw away.
			   Record size and jump over */

			item[ 1 ] = size ;
			item = tmp ;
			continue ;
		}
		switch ( *item & TAG_BITS )
		{
		case 0	       : error( "Found 0 instead of a header in SCAN 1\n" ) ;
#ifdef	RASTER
		case IMAGE     :
#endif	RASTER
		case STRING    :
		case S_FILE    : break ;
		case STRUCTURE : nptrs = s_base[ ( *item++ & B_0_15 ) / 2 + 1 ] ;
				 break ;
		case VEC_PNTRS : nptrs = ( item[ 2 ] - item[ 1 ] + 1 ) ; item += 3 ;
				 break ;
		case VEC_INTBS :
		case VEC_REALS : break ;
		default        : error( "OOPS bad heap header in SCAN 1\n" ) ;
		}

		for ( ; nptrs > 0 ; nptrs-- )
		if ( *item < (int)item && *item >= (int)psp )
		{
			dest = (int*)*item ;
			*item = *dest ;
			*dest = (int)item++ ;
		}
		else item++ ;

		item = tmp ;
	}

	/*
	 * SCAN 2 - slide down items, reverse all forward pointers and
	 *	    all backward pointers adusting addresses where neccessary
	 */

#ifdef  X11
        /* Since objects may move - including the screen, make sure the shared screen image isnt used for updates */
        if ( Xscreen != ( psint ) -1 ) freeze_XScreen() ;
#endif  X11
	item = reuse_base ;
	while ( item < heap_p )
	{
		register int size = 0, type = 0, *tmp = shift_addr ;
		register int nptrs = 0, *ptr = 0 ;

		/* First jump over garbage */
		while ( ( *item & TAG_BITS ) != 0 && !( *item & MARK_BIT ) )
			item += item[ 1 ] ;
		if ( item >= heap_p )
			continue ;

		type = *item & TAG_BITS ;
		if ( type == 0 )
		{
			find_header( item,shift_addr ) ;
			type = *item & TAG_BITS ;
		}

		*item ^= MARK_BIT ;	/* unmark item */
		size = obj_size( item ) ;
		if ( item != shift_addr )
#ifdef PERQ
#ifdef MCPY
			mcpy( item,shift_addr,size * 2 ) ;
		item += size ; shift_addr += size ;
#else
		{
			while( size-- ) *shift_addr++ = *item++ ;
		} else
		{
			item += size ;
			shift_addr += size ;
		}
#endif MCPY
#else
		{
			while( size >= B_16 ) /* copy in 32K chunks */
			{
				memcopy( item,shift_addr,B_15 ) ;
				item += B_15 ; shift_addr += B_15 ;
				size -= B_15 ;
			}
			memcopy( item,shift_addr,size ) ;
		}		
		item += size ; shift_addr += size ;
#endif PERQ


		switch ( type )
		{
		case 0         : error( "Found 0 instead of a header in SCAN 2\n" ) ;
#ifdef	RASTER
		case IMAGE     :
#endif	RASTER
		case STRING    :
		case S_FILE    : break ;
		case STRUCTURE : nptrs = s_base[ ( *tmp++ & B_0_15 ) / 2 + 1 ] ;
				 break ;
		case VEC_PNTRS : nptrs = ( tmp[ 2 ] - tmp[ 1 ] + 1 ) ; tmp += 3 ;
				 break ;
		case VEC_INTBS :
		case VEC_REALS : break ;
		default        : error( "OOPS bad heap header in SCAN 2\n" ) ;
		}

		for ( ; nptrs > 0 ; nptrs-- )
		if ( *tmp > (int)tmp && *tmp < (int) heap_p )
		{
			dest = (int*)*tmp ;
			*tmp = *dest ;
			*dest = (int)tmp++ ;
		}
		else tmp++ ;

	}

	heap_p = shift_addr ;
#ifdef  X11
        if ( Xscreen != ( psint ) -1 ) moved_XImage( screen ) ; else
#endif  X11
#else
	register int *ptr,ptype,*pfp,*cp ;
	psint	t ;

	if ( !gc_allowed ) return ;

	gc_count++ ;
	for ( ptr = psp ; ptr <= psb ; ptr++ )
		mark( *ptr ) ;	/* mark everything on pntr stack*/

	pfp = heap_base ; 
	cp = heap_base + 2 ;

	pfp[ 1 ] = 0 ; 
	ptype = MARKED ;
	while ( cp < heap_max )
	{
		if ( *cp & B_20 )
		{
			ptype = MARKED ; 
			*cp ^=  B_20 ;
			cp += obj_size( cp ) ;
			continue ;
		}
		if ( *cp == SINGLE )
		{
			if ( ptype == SINGLE )
			{
				ptype = FREE ; 
				cp[ -1 ] = 2 ;
				*cp = 0 ; 
				pfp[ 1 ] = mkpsptr( cp - 1 ) ;
				pfp = cp - 1 ;
			}
			else if ( ptype == MARKED ) ptype = SINGLE ;
			else *pfp += 1 ;
			cp++ ;
			continue ;
		}
		else
		{
			register int size ;

			size = obj_size( cp ) ;
			if ( ptype == FREE ) *pfp += size ;
			else
			{
				if ( ptype == SINGLE ) { 
					size++ ; 
					cp-- ; 
				}
				pfp[ 1 ] = mkpsptr( cp ) ; 
				*cp = size ;
				cp[ 1 ] = 0 ; 
				pfp = cp ;
				ptype = FREE ;
			}
			cp += size ;
		}
	}
#endif	COMPACT
}

#ifdef	COMPACT

mark_all()
{
	register int *dest = psp ;

	for ( ; dest <= psb ; dest++ )
		mark( (int *)( *dest ) ) ; /* mark everything on pntr stack */
}

set_up_vector()
{
	register int *base = psp - 3 ;

	base[0] = VEC_PNTRS | MARK_BIT ;	/* marked vector of pointers */
	base[1] = 0 ;				/* lower bound */
#ifdef PERQ
	base[2] = ( (int)psb - (int)psp ) / 2 ;	/* upper bound */
#else
	base[2] = ( (int)psb - (int)psp ) / 4 ;	/* upper bound */
#endif
}

find_header( item,shift_addr )
register int *item,*shift_addr ;
{
	register int *contents = 0 ;

	while ( ( *item & TAG_BITS ) == 0 )
	{
		contents = (int*)*item ;
		*item = *contents ;
		*contents = (int)shift_addr ;
	}
}

#endif	COMPACT
