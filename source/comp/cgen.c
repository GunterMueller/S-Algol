#include <stdio.h>
#include "defines.h"
#include "stype.h"
#include "functions.h"
#include "symbols.h"

extern  psint   codeword_size ;
extern  unsigned char   *codeword ;
extern  psint   cp,code_base,code_size ;
extern  psint   coding,externals ;
extern  psint   max_seg_size ;
extern  FILE    *so ;
extern  pntr	nil,INT,CINT,REAL,CREAL,BOOL,CBOOL,PNTR,CPNTR,
		STRING,CSTRING,FILET,CFILET,
#ifdef	RASTER
		PIXEL,CPIXEL,IMAGE,CIMAGE,image_type,
#endif	RASTER
#ifdef	OUTLINE
		PIC,CPIC,
#endif	OUTLINE
		VOID,ANY,vector_type;
extern  pntr	proc_decl_list,str_decl_list,ex_decl_list,external_list,
		procedure_type,struct_list ;
extern  psint   last_segment ;
extern  psint   st_size, st_size2, st_size3, st_size4,st_id_size ;
extern  psint   ssp, psp, max_ps, max_ms, special ;
extern  psint   mscw_size, lex_level ;
extern  psint   newlin, line_count ;
extern  psint   newlins ;
extern	psint	last_retract ;
extern	psint	ext ;

psint	last_label ;
extern	char	*malloc() ;

init_codeword()
{
	codeword = ( unsigned char * ) malloc( ( unsigned ) codeword_size ) ;
}

externs( n )
psint	n ;
{
	externals |= n ;
}

check( n,m )
psint	n,m ;
{

	char emess[ MAXSIZE ] ;

	if ( n > m )
	{
		( void ) sprintf( emess,"instruction operand too big\n%d %d\n",n,m ) ;
		err_mess( emess ) ;
/*
		Not implemented in this "C" version - doesn't seem essential
		trace() ;
*/
	}
}

setlab( link )
psint	link ;
{
	register psint	a,n,spot ;

	last_label = cp ;
	if ( coding )
	while ( link != ( psint ) 0 )
	{
		a = code_base + link ;
#ifdef	M68000
		n = ( codeword[ a ] << 8 ) + codeword[ a + 1 ] ;
#else
		n = ( codeword[ a + 1 ] << 8 ) + codeword[ a ] ;
#endif
		spot = link ;
		link = n ;
		n = cp - spot - ( psint ) 2 ;
		check( n,twoto17 ) ;
#ifdef	M68000
		codeword[ a ] = n >> 8 ;
		codeword[ a + 1 ] = n & ( psint ) 255 ;
#else
		codeword[ a ] = n & ( psint ) 255 ;
		codeword[ a + 1 ] = n >> 8 ;
#endif	M68000
	}
}

bout( n )
psint	n ;
{
	( void ) fprintf( so,"%c",n ) ;
}

boutl( n )
psint	n ;
{
#ifdef	PERQ
	bout( n >> 16 ) ;
	bout( n >> 24 ) ;
	bout( n ) ;
	bout( n >> 8 ) ;
#else
#ifdef	M68000
	bout( n >> 24 ) ;
	bout( n >> 16 ) ;
	bout( n >> 8 ) ;
	bout( n ) ;
#else
	bout( n ) ;
	bout( n >> 8 ) ;
	bout( n >> 16 ) ;
	bout( n >> 24 ) ;
#endif	M68000
#endif	PERQ
}

psint out_list( t )
pntr t ;
{
	psint	size = ( psint ) 0 ;

	if ( coding )
	{
		while ( t->skind != NIL )
		{
#ifdef	M68000
			bout( t->class.struct_table.s_tdm >> 8 ) ;
			bout( t->class.struct_table.s_tdm ) ;
			bout( t->class.struct_table.s_ptrs >> 8 ) ;
			bout( t->class.struct_table.s_ptrs ) ;
#else
			bout( t->class.struct_table.s_tdm ) ;
			bout( t->class.struct_table.s_tdm >> 8 ) ;
			bout( t->class.struct_table.s_ptrs ) ;
			bout( t->class.struct_table.s_ptrs >> 8 ) ;
#endif	M68000
			t = ( pntr ) t->class.struct_table.str_next ;
			size += ( psint ) 4 ;
		}
		return( size ) ;
	} else return( ( psint ) 0 ) ;
}

add_byte( n )
psint	n ;
{
	if ( coding )
	{
		codeword[ cp + code_base ] = n ;
		cp++ ;
		if ( cp >= max_seg_size ) err_mess( "** Segment too big **\n" ) ;
		if ( cp + code_base >= codeword_size ) err_mess( "** Code vector overflow **\n" ) ;
	}
}

add16( n )
register psint	n ;
{
#ifdef	M68000
	add_byte( n >> 8 ) ;
	add_byte( n & ( psint ) 255 ) ;
#else
	add_byte( n & ( psint ) 255 ) ;
	add_byte( n >> 8 ) ;
#endif	M68000
}

gen8( n )
psint	n ;
{
	check( n,( psint ) 255 ) ;
	add_byte( n ) ;
}

gen16( n )
psint	n ;
{
	check( n,twoto16 - ( psint ) 1 ) ;
	add16( n ) ;
}

try_single( op,m,n )
psint	op,m,n ;
{
	if ( ( m > ( psint ) 255 ) || ( n > ( psint ) 255 ) )
	{
		gen8( op + ( psint ) 128 ) ;
		gen16( m ) ;
		gen16( n ) ;
	} else
	{
		gen8( op ) ;
		gen8( m ) ;
		gen8( n ) ;
	}
}

try_short( op,n )
register psint	op,n ;
{
	if ( n < ( psint ) 256 ) { gen8( op ); gen8( n ); }
	else { gen8( op + ( psint ) 128 ); gen16( n ); }
}

try_jump( op,n )
register psint	op,n ;
{
	if ( n < ( psint ) 254 ) { gen8( op ); gen8( n + ( psint ) 2 ); }
        else { gen8( op + ( psint ) 128 ); gen16( n + ( psint ) 3 ); }
}

psint gen_type0( t )
register pntr	t ;
{
	if ( scalar_eq( CINT,t )  || scalar_eq( INT,t ) ||
#ifdef	RASTER
	     scalar_eq( CPIXEL,t ) || scalar_eq( PIXEL,t ) ||
#endif	RASTER
	     scalar_eq( BOOL,t )|| scalar_eq( CBOOL,t ) ) return( ( psint ) 0 ) ; else
	if ( scalar_eq( CREAL,t ) || scalar_eq( REAL,t ) ) return( ( psint ) 128 ) ; else
	if ( scalar_eq( CSTRING,t ) || scalar_eq( STRING,t ) ) return( ( psint ) 1 ) ; else
	if ( scalar_eq( VOID,t ) ) return( ( psint ) 130 ) ;
	else return( ( psint ) 129 ) ;
}

gen_type1( t,base )
pntr	t ;
psint	base ;
{
	gen8( base + gen_type0( t ) ) ;
}

psint pointer( t )
register pntr	t ;
{
	if ( scalar_eq( t,CSTRING ) || scalar_eq( t,STRING ) ||
	     scalar_eq( t,CPNTR )   || scalar_eq( t,PNTR )   ||
	     scalar_eq( t,CFILET )  || scalar_eq( t,FILET )  ||
#ifdef	OUTLINE
	     scalar_eq( t,CPIC )    || scalar_eq( t,PIC )    ||
#endif	OUTLINE
#ifdef	RASTER
             scalar_eq( t,IMAGE )   || scalar_eq( t,CIMAGE ) ||
	     eq( image_type,t )     ||
#endif	RASTER
	     eq( vector_type,t ) ) return( TRUE ) ; else return( FALSE ) ;
}

psint type_size( t )
pntr	t ;
{
	if ( t->skind == PROC_CLASS || eq2( REAL,t ) ) return( st_size2 ); else return( st_size ) ;
}

push_ps()
{
	psp += st_size ;
	if ( psp > max_ps ) max_ps = psp ;
}

pop_ps()
{
	psp -= st_size ;
}

pop_ms()
{
	ssp -= st_size ;
}

inc_ms( n )
psint	n ;
{
	ssp += n ;
	if ( ssp > max_ms ) max_ms = ssp ;
}

inc_ps( n )
psint	n ;
{
	psp += n ;
	if ( psp > max_ps ) max_ps = psp ;
}

push_ms()
{
	inc_ms( st_size ) ;
}

dec_ms( n )
psint	n ;
{
	ssp -= n ;
}

dec_ps( n )
psint	n ;
{
	psp -= n ;
}

dec_stack( t )
register pntr	t ;
{
	if ( !scalar_eq( t,VOID ) )
	{
		if ( pointer( t ) ) dec_ps( type_size( t ) ) ;
		else dec_ms( type_size( t ) ) ;
	}
}

inc_stack( t )
register pntr	t ;
{
	if ( !scalar_eq( t,VOID ) )
	{
		if ( pointer( t ) ) inc_ps( type_size( t ) ) ;
		else inc_ms( type_size( t ) ) ;
	}
}

psint get_opcode( s )
register char	*s ;
{
	if ( *s == '=' ) return( ( psint ) 96 ) ; else				/* = */
	if ( *s == '~' ) return( ( psint ) 99 ) ; else				/* ~= */
	if ( *s == '<' ) return( s[1] == NUL ? ( psint ) 102 : ( psint ) 104 ) ; else	/* <,<= */
	return( s[1] == NUL ? ( psint ) 106 : ( psint ) 108 );				/* >,>= */
}

comp( s,t )
register char	*s ;
register pntr	t ;
{
	gen_type1( t,get_opcode( s ) ) ;
	dec_stack( t ) ;
	dec_stack( t ) ;
	push_ms() ;
}

psint get_arith_opcode( s )
register char	*s ;
{
	if ( *s == '+' ) return( ( psint ) 110 ) ; else
	if ( *s == '*' ) return( ( psint ) 238 ) ; else
	if ( *s == '-' ) return( ( psint ) 111 ) ; else
	if ( *s == '/' ) return( ( psint ) 239 ) ; else
	if ( *s == 'd' ) return( ( psint ) 239 ) ;
	else return( ( psint ) 112 ) ;
}

arith_op( t,s )
register pntr	t ;
register char	*s ;
{
	dec_ms( type_size( t ) ) ;
	gen8( get_arith_opcode( s ) + ( eq( REAL,t ) ? ( psint ) 3 : ( psint ) 0 ) ) ;
}

sub_op( t1,t2 )
register pntr	t1,t2 ;
{
	register psint	arai ;
	psint	ptr,size ;

	arai = eq( vector_type,t2 ) ;
	ptr = pointer( t1 ) ;
	if ( !ptr ) pop_ps() ;
        size = ptr ? ( arai ? st_size : st_size2 ) : ( arai ? ( psint ) 0 : st_size ) ; 
	dec_ms( size ) ;
	if ( eq2( REAL,t1 ) ) push_ms() ;
	gen_type1( t1, arai ? ( psint ) 78 : ( psint ) 66 ) ;
}

sub_ass( t1,t2 )
register pntr	t1,t2 ;
{
	register psint	arai ;

	arai = eq( vector_type,t2 ) ;
	dec_stack( t1 ) ;
	pop_ps() ;
	dec_ms( arai ? st_size : st_size2 ) ;
	gen_type1( t1, arai ? ( psint ) 81 : ( psint ) 69 ) ;
}

is_op( s )
psint	s ;
{
	pop_ps() ;
	gen8( s ? ( psint ) 65 : ( psint ) 193 ) ;
}

neg_op( t )
pntr	t ;
{
	gen8( ( psint ) 240 + ( eq( REAL,t ) ? ( psint ) 3 : ( psint ) 0 ) ) ;
}

not_op()
{
	gen8( ( psint ) 115 ) ;
}

bjump( n )
psint	n ;
{
	try_jump( ( psint ) 1,cp - n ) ;
}

bjumpt( n )
psint	n ;
{
	pop_ms() ;
	try_jump( ( psint ) 10,cp - n ) ;
}

psint cjump( t,n )
pntr	t ;
psint	n ;
{
	dec_stack( t ) ;
	gen_type1( t,( psint ) 7 ) ;
	gen16( n ) ;
	return( cp - ( psint ) 2 ) ;
}

psint fjump( n )
psint	n ;
{
	gen8( ( psint ) 128 ) ;
	gen16( n ) ;
	return( cp - ( psint ) 2 ) ;
}

psint jumpf( n )
psint	n ;
{
	pop_ms() ;
	gen8( ( psint ) 130 ) ;
	gen16( n ) ;
	return( cp - ( psint ) 2 ) ;
}

psint jumpff( n )
psint	n ;
{
	pop_ms() ;
	gen8( ( psint ) 131 ) ;
	gen16( n ) ;
	return( cp - ( psint ) 2 ) ;
}

psint jumptt( n )
psint	n ;
{
	pop_ms() ;
	gen8( ( psint ) 132 ) ;
	gen16( n ) ;
	return( cp - ( psint ) 2 ) ;
}

finish_op()
{
	gen8( ( psint ) 125 ) ;
}

bound( n )
psint	n ;
{
	push_ms() ;
	pop_ps() ;
	gen8( ( psint ) 84 + ( psint ) 128 * n ) ;
}

load_st_func( n )
psint n ;
{
	inc_ms( st_size2 ) ;
	gen8( ( psint ) 59 ) ;
	gen8( n ) ;
}

load_32( n )
psint	n ;
{
#ifdef	PERQ
	add16( n >> 16 ) ;
	add16( n & ( ( ( psint ) 1 << 16 ) - ( psint ) 1 ) )
#else
#ifdef	M68000
	add_byte( ( n >> 24 ) & ( psint ) 255 ) ;
	add_byte( ( n >> 16 ) & ( psint ) 255 ) ;
	add_byte( ( n >>  8 ) & ( psint ) 255 ) ;
	add_byte( n & ( psint ) 255 ) ;
#else
	add16( n & ( ( ( psint ) 1 << 16 ) - ( psint ) 1 ) ) ;
	add16( n >> 16 ) ;
#endif	M68000
#endif	PERQ
}

forward_op()
{
	inc_ms( st_size2 ) ;
	gen8( ( psint ) 62 ) ;
}

external_op( n,t )
char	*n ;
pntr	t ;
{
	psint	code ;

	code = cp ;
	gen8( ( psint ) 45 ) ;
	gen16( ssp ) ;
	gen16( ( psint ) -1 ) ;	
	gen16( ( psint ) -1 ) ;
	inc_ms( st_size2 ) ;
	ex_decl_list = mk_external( n,t,code,ex_decl_list ) ; 
}

return_op( t )
pntr	t ;
{
	psint	i ;

	if ( coding )
	{
		if ( last_retract == ( cp - ( psint ) 3 ) )
		{
			cp -= ( psint ) 3 ;
			if ( last_label == cp + ( psint ) 3 )
				for ( i = ( psint ) 0 ; i < ( psint ) 3 ; i++ ) add_byte( ( psint ) 0 ) ;
		} else
		if ( last_retract == cp - ( psint ) 5 )
		{
			switch ( codeword[ cp - ( psint ) 5 ] )
			{
			case ( psint ) 181 :
			case ( psint ) 182 :
			case ( psint ) 183 :
			case ( psint ) 184 :
			case ( psint ) 186 : cp -= ( psint ) 5 ;
				   if ( last_label == cp + ( psint ) 5 )
				   {
					for ( i = ( psint ) 0 ; i < ( psint ) 5 ; i++ )
						add_byte( ( psint ) 0 ) ;
				   }
				   break ;

			default : break ;
			}
		}

		gen_type1( t,( psint ) 47 ) ;
		if ( ext ) while ( ( cp + ( psint ) 2 ) % ( psint ) 4 != 0 ) add_byte( ( psint ) 0 ) ; else
		while( cp % ( psint ) 4 != ( psint ) 0 ) add_byte( ( psint ) 0 ) ;
	}
}


psint get_retract_type( t )
pntr	t ;
{
	if ( scalar_eq( CINT,t )  || scalar_eq( INT,t ) ||
#ifdef	RASTER
	     scalar_eq( CPIXEL,t ) || scalar_eq( PIXEL,t ) ||
#endif	RASTER
	     scalar_eq( BOOL,t )|| scalar_eq( CBOOL,t ) ) return( ( psint ) 0 ) ; else
	if ( scalar_eq( CREAL,t ) || scalar_eq( REAL,t ) ) return( ( psint ) 1 ) ; else
	if ( scalar_eq( CSTRING,t ) || scalar_eq( STRING,t ) ) return( ( psint ) 2 ) ; else
	if ( scalar_eq( VOID,t ) ) return( ( psint ) 5 ) ; else
	return( ( psint ) 3 ) ;
}

retract( ms,ps,t )
psint	ms,ps ;
pntr	t ;
{
	psint	ms1,ps1 ;

	ms1 = ms ; ps1 = ps ;
	if ( pointer( t ) ) ps1 += st_size ;
	else if ( !scalar_eq( VOID,t ) ) ms1 += type_size( t ) ;

	if( !( ( ms1 == ssp ) && ( ps1 == psp ) ) )
	{
		ssp = ms1 ; psp = ps1 ;
		last_retract = cp ;
		try_single( ( psint ) 53 + get_retract_type( t ),ms,ps ) ;
	}
}

mst_external( n )
psint	n ;
{
	psint	disp,tmp ;

	disp = mscw_size + ( n + ( psint ) 1 ) * st_size2 + st_id_size ;
	if ( n == ( psint ) 3 )
	{
		gen8( ( psint ) 61 ) ;
		gen8( disp ) ;
	} else try_short( ( psint ) 19,disp ) ;

	ssp += mscw_size ;
	switch ( n )
	{
	case ( psint ) 0   :
	case ( psint ) 1   :
	case ( psint ) 2   : tmp = ( psint ) 4 ;
		   break ;
	case ( psint ) 3   : tmp = ( psint ) 8 ;
		   break ;
	default  : tmp = ( psint ) 16 ;
		   break ;
	}
	externs( tmp ) ;
}

apply_op( ms,ps )
psint	ms,ps ;
{
	try_short( ( psint ) 44,ms ) ;
	dec_ms( ms + mscw_size ) ;
	psp -= ps ;
}

load_struct_base()
{
#ifdef	M68000
	bout( twoto16 >> 8 ) ;
	bout( twoto16 ) ;
#else
	bout( twoto16 ) ;
	bout( twoto16 >> 8 ) ;
#endif	M68000
#if	defined( M68000) || defined( ALPHA )
	bout( 0 ) ; bout( 0 ) ;
	code_size = ( psint ) 4 ;
#else
	code_size = ( psint ) 2 ;
#endif
}

load_trademark( n )
psint	n ;
{
	try_short( ( psint ) ( psint ) 86,n ) ;
	add16( ( psint ) 0 ) ; add16( ( psint ) 0 ) ;
	str_decl_list = mk_s_decl( str_decl_list,cp ) ;
	inc_ms( st_size ) ;
}

load_proc_name( s ) 
char	*s ;
{
	psint	i,k ;

	gen8( ( psint ) 92 ) ;
	k = strlen( s ) ;
#ifdef	PERQ
	while ( ( ( cp + 2 ) % 4 != 0 ) && coding ) add_byte( 0 ) ;
	gen16( 0 ) ;
	gen16( k ) ;
#else
	while ( cp % 4 != 0 && coding ) add_byte( 0 ) ;
#ifdef	M68000
	gen16( ( psint ) 0 ) ;
	add_byte( ( k >> 8 ) & ( psint ) 255 ) ;
	add_byte( k & ( psint ) 255 ) ;
#else
	gen16( k ) ;
	gen16( ( psint ) 0 ) ;
#endif	M68000
#endif	PERQ

	for ( i = ( psint ) 0 ; i < k - ( psint ) 1 ; i += ( psint ) 2 )
#ifdef	M68000
	gen16( ( psint ) s[ i ] * ( psint ) 256 + ( psint ) s[ i + 1 ] ) ;
#else
	gen16( ( psint ) s[ i + 1 ] * ( psint ) 256 + ( psint ) s[ i  ] ) ;
#endif	M68000
	if ( k % ( psint ) 2 == ( psint ) 1 ) gen8( ( psint ) s[ k - 1 ] ) ;
}

make_vector( t,level,plevel )
pntr	t ;
psint	level,plevel ;
{
	ssp = level ;
	psp = plevel + st_size ;
	gen_type1( t,( psint ) 72 ) ;
	gen16( pointer( t ) ? plevel : level ) ;
}

endsegment( n,add )
psint	n,add ;
{
	register psint	base,i,j,k,offset,new_offset ;
	pntr	start ;

	if ( coding ) 
	{
		base = code_base + cp ;
		start = ex_decl_list ;
	
		while ( ex_decl_list->skind != NIL )
		{
			ex_decl_list->class.EXTERNAL.e_addr += code_size ;
			ex_decl_list = ( pntr ) ex_decl_list->class.EXTERNAL.e_next ;
		}
	
		if ( start->skind != NIL ) external_list = mk_cons( start,external_list ) ;
	
		while ( str_decl_list->skind != NIL )
		{
			i = str_decl_list->class.s_decl.s_int ;
			j = base + i - ( psint ) 4 ;
			k = -( code_size + i ) ;
#ifdef	PERQ
			codeword[ j ] = k >> 16 ;
			codeword[ j + 1 ] = k >> 24 ;
			codeword[ j + 2 ] = k ;
			codeword[ j + 3 ] = k >> 8 ;
#else
#ifdef	M68000
			codeword[ j ] = k >> 24 ;
			codeword[ j + 1 ] = k >> 16 ;
			codeword[ j + 2 ] = k >> 8 ;
			codeword[ j + 3 ] = k ;
#else
			codeword[ j ] = k ;
			codeword[ j + 1 ] = k >> 8 ;
			codeword[ j + 2 ] = k >> 16 ;
			codeword[ j + 3 ] = k >> 24 ;
#endif	M68000
#endif	PERQ
			str_decl_list = ( pntr ) str_decl_list->class.s_decl.s_pntr ;
		}
	
		while ( proc_decl_list->skind != NIL )
		{
			i = proc_decl_list->class.p_decl.p_int ;
			j = base + i ;
#ifdef	PERQ
			offset = codeword[ j ] << 16 ;
			offset |= codeword[ j + 1 ] << 24 ;
			offset |= codeword[ j + 2 ] ;
			offset |= codeword[ j + 3 ] << 8 ;
			new_offset = offset - ( code_size + i ) - 4 ;
			codeword[ j ] = new_offset >> 16 ;
			codeword[ j + 1 ] = new_offset >> 24 ;
			codeword[ j + 2 ] = new_offset ;
			codeword[ j + 3 ] = new_offset >> 8 ;
#else
#ifdef	M68000
			offset = codeword[ j + 3 ] ;
			offset |= codeword[ j + 2 ] << 8 ;
			offset |= codeword[ j + 1 ] << 16 ;
			offset |= codeword[ j ] << 24 ;
			new_offset = offset - ( code_size + i ) - ( psint ) 4 ;
			codeword[ j ] = new_offset >> 24 ;
			codeword[ j + 1 ] = new_offset >> 16 ;
			codeword[ j + 2 ] = new_offset >> 8 ;
			codeword[ j + 3 ] = new_offset ;
#else
			offset = codeword[ j ] ;
			offset |= codeword[ j + 1 ] << 8 ;
			offset |= codeword[ j + 2 ] << 16 ;
			offset |= codeword[ j + 3 ] << 24 ;
			new_offset = offset - ( code_size + i ) - ( psint ) 4 ;
			codeword[ j ] = new_offset ;
			codeword[ j + 1 ] = new_offset >> 8 ;
			codeword[ j + 2 ] = new_offset >> 16 ;
			codeword[ j + 3 ] = new_offset >> 24 ;
#endif	M68000
#endif	PERQ
			proc_decl_list = ( pntr ) proc_decl_list->class.p_decl.p_pntr ;
		}
	
		gen16( max_ms ) ; gen16( max_ps ) ; cp -= ( psint ) 4 ;
		for ( i = base ; i <= base + n - ( psint ) 1 ; i++ ) bout( ( psint ) codeword[ i ] ) ;
		add_byte( ( psint ) 45 ) ;
		gen16( add - st_size2 ) ;
		load_32( code_size ) ;
		last_segment = code_size ;
		code_size += n ;
	}
}

iliffe_op( t,bounds )
pntr	t ;
psint	bounds ;
{
	dec_stack( t ) ;
	dec_ms( st_size * bounds * ( psint ) 2 ) ;
	gen_type1( t,( psint ) 75 ) ;
	gen16( bounds ) ;
	push_ps() ;
}

form_structure( ms,ps )
psint	ms,ps ;
{
	gen8( ( psint ) 64 ) ; gen16( ms + st_size ) ;
	ssp = ms ;
	psp = ps + st_size ;
}

psint for_test( n )
psint	n ;
{
	gen8( ( psint ) 133 ) ;
	gen16( n ) ;
	return( cp - ( psint ) 2 ) ;
}

for_step( n )
psint	n ;
{
	try_jump( ( psint ) 6,cp - n ) ;
}

float_op( n )
psint	n ;
{
	push_ms() ;
	gen8( ( psint ) 116 + ( n - ( psint ) 1 ) * ( psint ) 128 ) ;
}

reverse_stack( t )
pntr	t ;
{
	gen8( ( psint ) 123 + ( pointer( t ) ? ( psint ) 128 : ( psint ) 0 ) ) ;
}

concat_op()
{
	pop_ps() ;
	gen8( ( psint ) 85 ) ;
}

substr_op()
{
	dec_ms( st_size2 ) ;
	gen8( ( psint ) 213 ) ;
}

write_op( n )
psint	n ;
{
	pop_ms() ;
	switch ( n )
	{
	case ( psint ) 0	: 
	case ( psint ) 2	: pop_ms() ;
		  break ;
	case ( psint ) 3	: 
	case ( psint ) 4	: pop_ps() ;
		  pop_ms() ;
		  break ;
	default	: pop_ps() ;
		  break ;
	}
	gen8( ( psint ) 255 ) ;
	gen8( n ) ;
}

read_op( n )
psint	n ;
{
	switch( n )
	{
	case ( psint ) 0	:
	case ( psint ) 2	:
	case ( psint ) 3	:
	case ( psint ) 6	: 
	case ( psint ) 9	: 
	case ( psint ) 10	: push_ms() ;
		  pop_ps() ;
		  break ;
	case ( psint ) 7	: pop_ps() ;
		  break ;
	default		: break ;
	}
	gen8( ( psint ) 127 ) ;
	gen8( n ) ;
}

erase_op( t )
pntr	t ;
{
	dec_stack( t ) ;
	gen_type1( t,( psint ) 120 ) ;
}

load_int( n )
psint	n ;
{
	if ( n < ( psint ) -128 || n >= ( psint ) 128 )
	{
		gen8( ( psint ) 218 ) ;
		load_32( n ) ;
	} else
	{
		gen8( ( psint ) 90 ) ;
		gen8( ( n < ( psint ) 0 ) ? ( psint ) 256 + n : n ) ;
	}
}

load_lit( t,n )
pntr	t ;
psint	n ;
{
	if ( eq2( PNTR,t ) ) { push_ps() ; gen8( ( psint ) 222 ) ;  } else
	if ( eq2( FILET,t ) ) { push_ps() ; gen8( ( psint ) 221 ) ; } else
	{
		push_ms() ;
		if ( eq2( BOOL,t ) )
		{
			gen8( ( psint ) 91 ) ;
			gen8( ( n < ( psint ) 0 ) ? ( psint ) 256 + n : n ) ;
		}
		else load_int( n ) ;
	}
}

load_string( t )
pntr	t ;
{
	psint	i,k ;

	k = t->class.str.str_int ;
	if ( k == ( psint ) 0 ) gen8( ( psint ) 94 ) ; else
	if ( k == ( psint ) 1 )
	{
		gen8( ( psint ) 93 ) ;
		gen8( ( ( pntr ) t->class.str.str_pntr )->class.str.str_int );
	} else
	{
		gen8( ( psint ) 92 ) ;
#ifdef	PERQ
		while ( ( cp + ( psint ) 2 ) % ( psint ) 4 != ( psint ) 0  && coding )
			add_byte( ( psint ) 0 ) ;
		gen16( ( ( psint ) 1 << 3 ) | ( ( psint ) 1 << 12 ) ) ;
		gen16( k ) ;
#else
		while ( cp % 4 != 0 && coding ) add_byte( ( psint ) 0 ) ;
#ifdef	M68000
		gen16( ( ( psint ) 1 << 3 ) | ( ( psint ) 1 << 12 ) ) ;
		gen16( k ) ;
#else
		gen16( k ) ;
		gen16( ( ( psint ) 1 << 3 ) | ( ( psint ) 1 << 12 ) ) ;
#endif	M68000
#endif	PERQ
		for ( i = ( psint ) 0 ; i < k ; i ++ )
		{
			t = ( pntr ) t->class.str.str_pntr ;
			gen8( t->class.str.str_int ) ;
		}
	}
	push_ps() ;
}

load_real( n )
double	n ;
{
	psint	*ans ;

	gen8( ( psint ) 219 ) ;
	ans = ( psint * ) ( &n ) ;   /* fiddle_r() */
	load_32( ans[0] ); load_32( ans[1] ) ;
	inc_ms( st_size2 ) ;
}

gen_load( op,add1,add2 )
psint	op,add1,add2 ;
{
	if ( add2 > ( psint ) 255 )
	{
		gen8( op + ( psint ) 128 ) ;
		gen8( add1 ) ; gen16( add2 ) ;
	} else { gen8( op ); gen8( add1 ); gen8( add2 ); }
}

psint mark_stack( p )
pntr	p ;
{
	psint	ll,dd,params ;
	pntr	t ;

	if ( coding )
	{
		ll = p->class.link.addr1 ;
		dd = p->class.link.addr2 ;
		t = ( pntr ) ( ( pntr ) p->class.link.type1 )->class.proc.args ;
		params = ( t->skind != NIL ) ;
		if ( ( ( pntr ) p->class.link.type1 )->class.proc.stand )
		{
			gen8( params ? ( psint ) 187 : ( psint ) 60 ) ;
			gen8( dd ) ;
		} else
		if ( ll == lex_level )
			try_short( ( params ? ( psint ) 15 : ( psint ) 52 ),dd ) ; else
		if ( ll == ( psint ) 0 )
			try_short( ( params ? ( psint ) 19 : ( psint ) 51 ),dd ) ; else
		gen_load( ( params ? ( psint ) 27 : ( psint ) 63 ),lex_level - ll,dd ) ;

		if ( params ) inc_ms( mscw_size ) ;
		return( params ) ;
	} else return( ( ( pntr ) ( ( pntr )p->class.link.type1 )->class.proc.args )->skind != NIL ) ;
}

load( add1,add2,r_value,t )
psint	add1,add2,r_value ;
pntr	t ;
{
	psint	ty ;

	if ( r_value ) inc_stack( t ) ; else dec_stack( t ) ;

	if ( pointer( t ) ) ty = ( psint ) 1 ; else
	if ( t->skind == PROC_CLASS ) ty = ( psint ) 2 ; else
	if ( eq( REAL,t ) ) ty = ( psint ) 2 ; else ty = ( psint ) 0 ;

	ty += ( r_value ? ( psint ) 0 : ( psint ) 16 ) ;

	if ( add1 == lex_level ) try_short( ( psint ) 12 + ty,add2 ) ; else
	if ( add1 == ( psint ) 0 ) try_short( ( psint ) 16 + ty,add2 ) ; else
	gen_load( ( psint ) 24 + ty,lex_level - add1,add2 ) ;
	
}

#ifdef	RASTER
image_op()
{
	dec_ms( st_size3 ) ;
	push_ps() ;
	gen8( ( psint ) 36 ) ;
}

form_pixel_op( n )
psint	n ;
{
	dec_ms( ( n - ( psint ) 1 ) * st_size ) ;
	gen8( ( psint ) 164 ) ;
	gen8( n ) ;
}

subimage_op()
{
	dec_ms( st_size2 ) ;
	gen8( ( psint ) 35 ) ;
}

subpixel_op()
{
	pop_ms() ;
	gen8( ( psint ) 163 ) ;
}

limit_op( to_symb )
psint	to_symb ;
{
	dec_ms( to_symb ? st_size4 : st_size2 ) ;
	if ( to_symb ) gen8( ( psint ) 165 ) ; else gen8( ( psint ) 37 ) ;
}

Raster_op( s )
char	*s ;
{
	psint	opcode ;

	dec_ps( st_size2 ) ;
	opcode = ( psint ) 20 ;
	if ( streq( s,copy_sy ) ) opcode += ( psint ) 0 ; else
	if ( streq( s,rnot_sy ) ) opcode += ( psint ) 128 ; else
	if ( streq( s,rand_sy ) ) opcode += ( psint ) 1 ; else
	if ( streq( s,nand_sy ) ) opcode += ( psint ) 129 ; else
	if ( streq( s,ror_sy ) ) opcode += ( psint ) 2 ; else
	if ( streq( s,nor_sy ) ) opcode += ( psint ) 130 ; else
	if ( streq( s,xor_sy ) ) opcode += ( psint ) 3 ; else
	opcode += ( psint ) 131 ;
	gen8( opcode ) ;
}
#endif	RASTER
newline()
{
	if ( newlin && coding && newlins )
	{
		try_short( ( psint ) 124,line_count + ( psint ) 1 ) ;
		newlin = FALSE ;
	}
}
