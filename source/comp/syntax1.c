#include <strings.h>
#include <stdio.h>
#include "defines.h"
#include "stype.h"
#include "functions.h"
#include "symbols.h"
#include "cgen.h"
#include "lex.h"
#include "mktype.h"

extern  pntr    nil,INT,CINT,REAL,CREAL,BOOL,CBOOL,PNTR,CPNTR,
                STRING,CSTRING,FILET,CFILET,
#ifdef  RASTER
                PIXEL,CPIXEL,IMAGE,CIMAGE,image_type,
#endif  RASTER
#ifdef  OUTLINE
                PIC,CPIC,
#endif  OUTLINE
                VOID,ANY,vector_type;

extern	pntr	this, this1, this2 ;
extern	psint	deflt, mixed_mode, xit, xit1, error ;
extern	pntr	env_list,expr_t ;

extern	psint	forwd_count,prelude ;

pntr	unary_op( n )
psint	n ;
{
	register psint	neg,more1 = TRUE,level,plevel,bounds ;
	char	n1[ MAXSIZE ],s[ 6 ] ;
	register pntr	p,t,start,t1;

	if ( Streq( symb,identifier_sy ) )
	{
		( void ) strcpy( n1,the_name ) ;
		p = search_name( n1 ) ;
		if ( p->skind == NIL ) name_err1( n1 ) ;
		next_sy() ;
		if ( p->skind == NIL ) return( ANY ) ; else
		{
			t = ( pntr ) p->class.link.type1 ;
			if ( t->skind == PROC_CLASS ) return( proc_call( p ) ) ; else
			if ( t->skind == STRUCTURE_CLASS ) return( args_list( look_up( p,TRUE ) ) ) ; else
			if ( Streq( symb,assign_sy ) )
			{
				if ( t->skind == VAR_CLASS ) t = ( pntr ) t->class.var.contents; else { ass_err( t ) ; t = ANY; }
				next_sy() ;
				match( t,clause() ) ;
				t = look_up( p,FALSE ) ;
				return( VOID ) ;
			} else return( look_up( p,TRUE ) ) ;
		}
	} else
	if ( Streq( symb,literal_sy ) )
	{
		t = lit_type ;
		if ( scalar_eq( t,REAL ) ) load_real( real_value ) ; else
		if ( scalar_eq( t,STRING ) ) load_string( the_string ) ;
		else load_lit( t,the_lit ) ;
		next_sy() ;
		return( t ) ;
	} else
	if ( Streq( symb,begin_sy ) || Streq( symb,lcb_sy ) ) return( block() ) ; else
	if ( Streq( symb,lp_sy ) )
	{
		next_sy() ;
		t = clause() ;
		mustbe( rp_sy ) ;
		return( t ) ;
	} else
#ifdef	OUTLINE
	if ( Streq( symb,lsb_sy ) )
	{
		next_sy() ;
		level = ssp ;
		plevel = psp ;
		t = lookup( "poin.strc",TRUE ) ;
		match( REAL,clause() ) ;
		reverse_stack( REAL ) ;
		mustbe( comma_sy ) ;
		match( REAL,clause() ) ;
		reverse_stack( REAL ) ;
		mustbe( rsb_sy ) ;
		form_structure( level,plevel ) ;
		return( PIC ) ;
	} else
#endif	OUTLINE
	if ( Streq( symb,vector_sy ) )
	{
		bounds = ( psint ) 0 ;
		t = mk_vector( nil ) ;
		start = t ;
		next_sy() ;
		while ( more1 )
		{
			match( INT,clause() ) ;
			mustbe( dcolon_sy ) ;
			match( INT,clause() ) ;
			bounds ++ ;
			if ( have( comma_sy ) ) t = mk_vector( t ) ; else more1 = FALSE ;
		}
		mustbe( of_sy ) ;
		t1 = clause() ;
		( pntr ) start->class.VECTOR.elms = t1 ;
		if ( scalar_eq( t1,VOID ) ) bad_type( t1 ) ;
		iliffe_op( t1,bounds ) ;
		return( t ) ;
	} else
	if( Streq( symb,vec_at_sy ) )
	{
		level = ssp ;
		plevel = psp ;
		next_sy() ;
		match( INT,clause() ) ;
		mustbe( of_sy ) ;
		t = type() ;
		mustbe( lsb_sy ) ;
		while( more1 )
		{
			match( t,clause() ) ;
			if ( !have( comma_sy ) ) more1 = FALSE ;
		}
		mustbe( rsb_sy ) ;
		make_vector( t,level,plevel ) ;
		return( mk_vector( t ) ) ;
	} else
	if ( Streq( symb,not_sy ) ) { le1( n,( psint ) 3 ) ; match( BOOL,expr( ( psint ) 3 ) ) ; not_op() ; return( BOOL ) ; } else
	if ( Streq( symb,plus_sy ) ) { le1( n,( psint ) 4 ) ; return( int_real( expr( ( psint ) 4 ) ) ) ; } else
	if ( Streq( symb,minus_sy ) )
	{
		unary_minus = TRUE ;
		le1( n,( psint ) 4 ) ;
		neg = unary_minus ;
		unary_minus = FALSE ;
		t1 = int_real( expr( ( psint ) 4 ) ) ;
		if ( neg ) neg_op( t1 ) ;
		return( t1 ) ;
	} else
#ifdef	OUTLINE
	if ( Streq( symb,shift_sy ) || Streq( symb,scale_sy ) ) return( transform_pic() ) ; else
	if ( Streq( symb,rotate_sy ) ) return( rotate_pic() ) ; else
	if ( Streq( symb,colour_sy ) ) return( colour_pic() ) ; else
	if ( Streq( symb,text_sy ) ) return( text_pic() ) ; else
#endif	OUTLINE
#ifdef	RASTER
	if ( Streq( symb,image_sy ) ) return( image_exp() ) ; else
	if ( Streq( symb,limit_sy ) ) return( limit_exp() ) ; else
	if ( Streq( symb,rand_sy ) || Streq( symb,rnot_sy ) || Streq( symb,xor_sy ) || Streq( symb,xnor_sy ) ||
	     Streq( symb,nor_sy ) || Streq( symb,nand_sy ) || Streq( symb,ror_sy ) || Streq( symb,copy_sy ) )
	{
		( void ) strcpy( s,symb ) ;
		next_sy() ;
		match( image_type,clause() ) ;
		mustbe( onto_sy ) ;
		match( IMAGE,clause() ) ;
		Raster_op( s ) ;
		return( VOID ) ;
	} else
#endif	RASTER
	return( standard_func() ) ;
}

pntr	subscript( t1,t2 )
pntr	t1,t2 ;
{
	if ( eq2( PNTR,t1 ) && t2->skind == FIELD_CLASS ) return( ( pntr ) t2->class.field.fieldt ) ; else
	if ( eq2( vector_type,t1 ) && eq2( INT,t2 ) )
	{
		if ( t1->skind == CONST_CLASS ) t1 = ( pntr ) t1->class.CONST.content ;
		return( ( pntr ) t1->class.VECTOR.elms ) ;
	}
	else
	{
		bad_types( t1,t2 ) ;
		return( ANY ) ;
	}
}

pntr	exp_list( t )
pntr	t ;
{
	register pntr	t1 = nil,t2 = nil,a_clause=nil ;
	register psint	more1 = TRUE ;
	t1 = t; t2 = t ;
	mustbe( lp_sy ) ;
	while( more1 )
	{
		a_clause = clause() ;
		t1 = subscript( t2,a_clause ) ;
		if ( have( comma_sy ) ) { sub_op( t1,t2 ) ; t2 = t1 ; } else more1 = FALSE ;
	}
	mustbe( rp_sy ) ;
	if ( Streq( symb,assign_sy ) )
	{
		if ( t1->skind == CONST_CLASS ) ass_err( t1 ) ;
		next_sy() ;
		match( t1,clause() ) ;
		sub_ass( t1,t2 ) ;
		return( VOID ) ;
	} else
	{
		sub_op( t1,t2 ) ;
		if ( t1->skind == CONST_CLASS ) return( ( pntr ) t1->class.CONST.content ) ; else return( t1 ) ;
	}
}

static	psint	expr_more = TRUE ;
static	char	expr_s[MAXSIZE] ;

psint expr_le2( n,p )
psint	n,p ;
{
	if ( n <= p ) { ( void ) strcpy( expr_s,symb ) ; next_sy() ; return TRUE; }
	else { expr_more = FALSE; return FALSE; }
}

expr_and_or_op( n,n1 )
psint	n,n1 ;
{
	register psint	l ;
	if ( expr_le2( n,n1 ) )
	{
		match( BOOL,expr_t ) ;
		l = ( n1 == ( psint ) 0 ) ? jumptt( newlab ) : jumpff( newlab ) ;
		match( BOOL,expr( n1 + ( psint ) 1 ) ) ;
		setlab( l ) ;
		expr_t = BOOL ;
	}
}

#ifdef	OUTLINE
expr_join( m )
psint	m ;
{
	register psint	level,plevel ;

	if ( eq( PIC,expr_t ) )
	{
		next_sy() ;
		level = ssp ;
		plevel = psp - st_size ;
		match( PIC,expr_t ) ;
		( void ) lookup( "oprtn.strc",TRUE ) ;
		match( PIC,expr( ( psint ) 5 ) ) ;
		load_lit( INT,m ) ;
		form_structure( level,plevel ) ;
		expr_t = PIC ;
	}
	else expr_more = FALSE ;
}
#endif	OUTLINE

/*
pntr	rel_type( t,t1 )
pntr	t,t1 ;
{
	if ( eq2( INT,t ) ) return( coerce( t,t1 ) ) ; else
	if ( eq( REAL,t ) || eq( STRING,t ) ) { match( t,t1 ) ; return( t ) ; }
	else { bad_types( t,t1 ) ; return( ANY ) ; }
}
*/

pntr	rel_type( t,s )
pntr	t ;
char	*s ;
{
	if ( eq2( INT,t ) ) return( coerce( t,expr( ( psint ) 3 ) ) ) ; else
	if ( eq( REAL,t ) || eq( STRING,t ) ) { match( t,expr( ( psint ) 3 ) ) ; return( t ) ; }
	else { bad_types2( t,s ) ; return( ANY ) ; }
}

pntr	expr( n )
psint	n ;
{
	register psint	save_more ;
	register pntr	save_t,restype = nil ;
	char	save_s[ MAXSIZE ] ;
	
	newline() ;
	save_more = expr_more ;
	expr_more = TRUE ;
	( void ) strcpy( save_s,expr_s ) ;
	( void ) strcpy( expr_s,"" ) ;
	save_t = expr_t ;
	expr_t = unary_op( n ) ;
	
	while( expr_more && !at_new_line )
	if ( Streq( symb,or_sy ) ) expr_and_or_op( n,( psint ) 0 ) ; else
	if ( Streq( symb,and_sy ) ) expr_and_or_op( n,( psint ) 1 ) ; else
	if ( Streq( symb,eq_sy ) || Streq( symb,neq_sy ) )
	{
		if ( expr_le2( n,( psint ) 2 ) )
		{
			if ( eq2( INT,expr_t ) ) expr_t = coerce( expr_t,expr( ( psint ) 3 ) ) ; else match( expr_t,expr( ( psint ) 3 ) ) ;
			comp( expr_s,expr_t ) ;
			expr_t = BOOL ;
		}
	} else
	if ( Streq( symb,le_sy ) || Streq( symb,lt_sy ) || Streq( symb,ge_sy ) || Streq( symb,gt_sy ) )
	{
		if ( expr_le2( n,( psint ) 2 ) )
		{
/*
			expr_t = rel_type( expr_t,expr( ( psint ) 3 ) ) ;
*/
			expr_t = rel_type( expr_t,expr_s ) ;
			comp( expr_s,expr_t ) ;
			expr_t = BOOL ;
		}
	} else
	if ( Streq( symb,is_sy ) || Streq( symb,isnt_sy ) )
	{
		if ( expr_le2( n,( psint ) 2 ) )
		{
			match( PNTR,expr_t ) ;
			expr_t = lookup( the_name,TRUE ) ;
			if ( expr_t->skind != STRUCTURE_CLASS ) bad_type( expr_t ) ;
			mustbe( identifier_sy ) ;
			is_op( !streq( expr_s,isnt_sy ) ) ;
			expr_t = BOOL ;
		}
	} else
	if ( Streq( symb,dplus_sy ) )
	{
		if ( expr_le2( n,( psint ) 3 ) )
		{
			match( STRING,expr_t ) ;
			match( STRING,expr( ( psint ) 4 ) ) ;
			concat_op() ;
			expr_t = STRING ;
		}
	} else
	if ( Streq( symb,plus_sy ) || Streq( symb,minus_sy ) )
	{
		if ( expr_le2( n,( psint ) 3 ) )
		{
			if ( !eq2( INT,expr_t ) && !eq2( REAL,expr_t ) )
			{
				bad_types2( expr_t,expr_s ) ;
				return( ANY ) ;
			} else
			{
				expr_t = coerce( expr_t,expr( ( psint ) 4 ) ) ;
				arith_op( expr_t,expr_s ) ;
			}
		}
	} else
#ifndef	RASTER
#ifdef	OUTLINE
	if ( Streq( symb,amper_sy ) ) join( ( psint ) 1 ) ; else
	if ( Streq( symb,hat_sy ) ) join( ( psint ) 2 ) ; else
#endif	OUTLINE
#else
	if ( Streq( symb,amper_sy ) )
	{
		if ( eq( PIXEL,expr_t ) )
		{
			if ( expr_le2( n,( psint ) 2 ) )
			{
				n = ( psint ) 1 ;
				match( PIXEL,expr( ( psint ) 3 ) ) ;
				n++ ;
				while ( have( amper_sy ) ) { match( PIXEL,expr( ( psint ) 3 ) ) ; n++ ; }
				form_pixel_op( n ) ;
			}
		} else expr_join( ( psint ) 1 ) ;
	} else
	if ( Streq( symb,hat_sy ) ) expr_join( ( psint ) 2 ) ; else
#endif	RASTER
	if ( Streq( symb,star_sy ) ) { if ( expr_le2( n,( psint ) 4 ) ) { expr_t = coerce( expr_t,expr( ( psint ) 5 ) ) ; arith_op( expr_t,expr_s ) ; }; } else
	if ( Streq( symb,divide_sy ) )
	{
		if ( expr_le2( n,( psint ) 4 ) )
		{
			match( REAL,expr_t ) ;
			match( REAL,expr( ( psint ) 5 ) ) ;
			arith_op( REAL,expr_s ) ;
			expr_t = REAL ;
		}
	} else
	if ( Streq( symb,rem_sy ) || Streq( symb,div_sy ) )
	{
		if ( expr_le2( n,( psint ) 4 ) )
		{
			match( INT,expr_t ) ;
			match( INT,expr( ( psint ) 5 ) ) ;
			arith_op( expr_t,expr_s ) ;
			expr_t = INT ;
		}
	} else
	if ( Streq( symb,lp_sy ) )
	{
		if ( eq2( STRING,expr_t ) ) /* substring */
		{
			next_sy() ;
			match( INT,clause() ) ;
			mustbe( bar_sy ) ;
			match( INT,clause() ) ;
			mustbe( rp_sy ) ;
			substr_op() ;
			expr_t = STRING ;
		} else
#ifdef	RASTER
		if ( eq( image_type,expr_t ) )   /* sub image */
		{
			next_sy() ;
			match( INT,clause() ) ;
			mustbe( bar_sy ) ;
			match( INT,clause() ) ;
			mustbe( rp_sy ) ;
			subimage_op() ;
		} else
		if ( eq( PIXEL,expr_t ) ) /* sub pixel */
		{
			next_sy() ;
			match( INT,clause() ) ;
			mustbe( rp_sy ) ;
			subpixel_op() ;
			expr_t = PIXEL ;
		} else
#endif	RASTER
		expr_t = exp_list( expr_t )  ;
	} else
	expr_more = FALSE ;
	expr_more = save_more ;
	restype = expr_t ;
	expr_t = save_t ;
	( void ) strcpy( expr_s,save_s ) ;
	return( restype ) ;
}

pntr	if_clause()
{
	register pntr	t,t1 ; 
	register psint	l,m,n ;

	next_sy() ;
	match( BOOL,clause() ) ;
	l = jumpf( newlab ) ;
	if ( have( do_sy ) )
	{
		match( VOID,clause() ) ;
		setlab( l ) ;
		return( VOID ) ;
	}
	else
	{
		mustbe( then_sy ) ;
		t = clause() ;
		m = fjump( newlab ) ;
		dec_stack( t ) ;
		setlab( l ) ;
		mustbe( else_sy ) ;
		t1 = clause() ;
		if ( eq2( INT,t ) && eq2( REAL,t1 ) )
		{
			n = fjump( newlab ) ;
			setlab( m ) ;
			float_op( ( psint ) 1 ) ;
			dec_stack( INT ) ;
			setlab( n ) ;
			return( REAL ) ;
		}
		else
		{
			match( t,t1 ) ;
			setlab( m ) ;
			return( t ) ;
		}
	}
}

pntr	for_clause()
{
	char	n[ MAXSIZE ] ;
	register psint	level,plevel,sp,l,m ;

	next_sy() ;
	level = ssp ; plevel = psp ;
	( void ) strcpy( n,the_name ) ;
	mustbe( identifier_sy ) ;
	mustbe( eq_sy ) ;
	match( INT,clause() ) ;
	enter_scope() ;
	sp = ssp ;
	mustbe( to_sy ) ;
	match( INT,clause() ) ;
	if ( have( by_sy ) ) match( INT,clause() ) ; else load_lit( INT,( psint ) 1 ) ;
	( pntr ) env_list->class.cons.hd = enter( ( pntr ) env_list->class.cons.hd,mk_link( n,lex_level,sp,CINT,nil,nil ),TRUE ) ;
	mustbe( do_sy ) ;
	l = cp ;
	m = for_test( newlab ) ;
	match( VOID,clause() ) ;
	for_step( l ) ;
	setlab( m ) ;
	exit_scope() ;
	ssp = level ;
	psp = plevel ;
	return( VOID ) ;
}

pntr	while_clause()
{
	register psint	l,m ;

	next_sy() ;
	l = cp ;
	match( BOOL,clause() ) ;
	m = jumpf( newlab ) ;
	mustbe( do_sy ) ;
	match( VOID,clause() ) ;
	bjump( l ) ;
	setlab( m ) ;
	return( VOID ) ;
}

pntr	repeat_clause()
{
	register psint	l,m ;

	next_sy() ;
	l = cp ;
	match( VOID,clause() ) ;
	mustbe( while_sy ) ;
	match( BOOL,clause() ) ;
	if ( have( do_sy ) )
	{
		m = jumpf( newlab ) ;
		match( VOID,clause() ) ;
		bjump( l ) ;
		setlab( m ) ;
	} else bjumpt( l ) ;
	return( VOID ) ;
}

match_it()
{
	if ( deflt ) erase_op( this ) ; else dec_stack( this ) ;
	this1 = clause() ;
	if ( eq2( this2,INT ) && eq2( this1,REAL ) ) mixed_mode = TRUE; else
	if ( eq2( REAL,this2 ) ) this1 = coerce( this2,this1 ) ; else match( this2,this1 ) ;
	if ( mixed_mode ) xit1 = fjump( xit1 ) ; else
	if ( !deflt ) xit = fjump( xit ) ;
	if ( !deflt ) dec_stack( this1 ) ;
}

pntr	case_clause()
{
	register psint	l,m ;
	register psint	more1,save_mixed_mode,save_xit,save_xit1,save_deflt ;
	register pntr	save_this = this,save_this1 = this1,save_this2 = this2,restype = nil ;

	save_mixed_mode = mixed_mode ; mixed_mode = FALSE ;
	save_xit = xit ; save_xit1 = xit1 ;
	save_deflt = deflt ;			     /* remember old value of mixed_mode etc */
	next_sy() ;
	this = clause() ; this2 = ANY ; this1 = ANY ;
	if ( scalar_eq( this,VOID ) ) bad_type( this ) ;

	xit = newlab ;
	xit1 = newlab ;
	mixed_mode = FALSE ;
	deflt = FALSE ;

	mustbe( of_sy ) ;
	while ( !have( default_sy ) && !Streq( symb,eot ) )
	{
		more1 = TRUE ;
		l = newlab ;
		while ( more1 )
		{
			if ( eq2( INT,this ) ) this = coerce( this,clause() ) ; else match( this,clause() ) ;
			l = cjump( this,l ) ;
			if ( !have( comma_sy ) ) more1 = FALSE ;
		}
		m = fjump( newlab ) ;
		mustbe( colon_sy ) ;
		setlab( l ) ;
		match_it() ;
		inc_stack( this ) ;
		setlab( m ) ;
		this2 = this1 ;
		separator() ;
	}
	mustbe( colon_sy ) ;
	deflt = TRUE ;
	match_it() ;
	setlab( xit ) ;
	if ( mixed_mode ) { float_op( ( psint ) 1 ) ; dec_stack( INT ) ; setlab( xit1 ) ; }
	mixed_mode = save_mixed_mode ;
	deflt = save_deflt ;
	xit = save_xit ;
	xit1 = save_xit1 ;
	restype = this1 ;
	this = save_this ; this1 = save_this1 ; this2 = save_this2 ;
	return( restype ) ;
}

pntr	write_clause( wrt )
psint	wrt ;
{
	register psint	op,more1 = TRUE ;
	register pntr	t = nil ;

	newline() ;
	next_sy() ;
	if ( wrt ) match( FILET,lookup( "s.o",TRUE ) ) ;
	else { match( FILET,clause() ) ; mustbe( comma_sy ) ; }
	while ( more1 )
	{
		t = clause() ;
		if ( have( colon_sy ) ) match( INT,clause() ) ; else
		load_lit( INT,( psint ) -1 ) ;
		if( eq( REAL,t ) )
		{
			mst_external( ( psint ) 3 ) ;
			apply_op( st_size2 + st_size,( psint ) 0 ) ;
		} else
		{
		if ( scalar_eq( t,STRING ) || scalar_eq( t,CSTRING ) ) op = ( psint ) 1 ; else
		if ( scalar_eq( t,INT ) || scalar_eq( t,CINT ) ) op = ( psint ) 0 ; else
		if ( scalar_eq( t,BOOL ) || scalar_eq( t,CBOOL ) ) op = ( psint ) 2 ; else
		{
			bad_type( t ) ;
			op = ( psint ) 0 ;
		}
		write_op( op ) ;
		}
		if( !have( comma_sy ) ) more1 = FALSE ;
	}
	erase_op( FILET ) ;
	return( VOID ) ;
}

pntr	clause()
{
	register psint	i,tmp ;
	char	s[ MAXSIZE ] ;

	if ( Streq( symb,identifier_sy ) || Streq( symb,literal_sy ) ) return( expr( ( psint ) -1 ) ) ; else
	if ( Streq( symb,if_sy ) ) return( if_clause() ) ; else
	if ( Streq( symb,repeat_sy ) ) return( repeat_clause() ) ; else
	if ( Streq( symb,while_sy ) ) return( while_clause() ) ; else
	if ( Streq( symb,for_sy ) ) return( for_clause() ) ; else
	if ( Streq( symb,case_sy ) ) return( case_clause() ) ; else
	if ( Streq( symb,write_sy ) ) return( write_clause( TRUE ) ) ; else
	if ( Streq( symb,output_sy ) ) return( write_clause( FALSE ) ) ; else
	if ( Streq( symb,out_byte_sy ) || Streq( symb,out_16_sy ) || Streq( symb,out_32_sy ) )
	{
		( void ) strcpy( s,symb ) ;
		newline() ;
		next_sy() ;
		match( FILET,clause() ) ;
		if ( streq( s,out_32_sy ) ) tmp = ( psint ) 1 ; else tmp = ( psint ) 2 ;
		for ( i = ( psint ) 1 ; i <= tmp ; i++ ) { mustbe( comma_sy ) ; match( INT,clause() ) ; }
		if( streq( s,out_byte_sy ) ) tmp = ( psint ) 3 ; else
		if( streq( s,out_16_sy ) ) tmp = ( psint ) 4 ; else
		tmp = ( psint ) 5 ;
		write_op( tmp ) ;
		return( VOID ) ;
	} else
	return( expr( ( psint ) -1 ) ) ;
}

psint sequence_more()
{
	if ( !Streq( symb,rcb_sy ) && !Streq( symb,end_sy ) && !Streq( symb,eot ) ) return( TRUE ) ;
	else return FALSE ;
}

pntr	sequence()
{
	register pntr	t = VOID ;
	psint	save,level,plevel ;

	level = ssp ; plevel = psp ;
	save = forwd_count ; forwd_count = ( psint ) 0 ;

	while( sequence_more() )
	{
		match( VOID,t ) ;
		if ( Streq( symb,procedure_sy ) ) proc_decl() ; else
		if ( Streq( symb,structure_sy ) ) structure_decl() ; else
		if ( Streq( symb,forward_sy ) ) forward_decl() ; else
		if ( Streq( symb,external_sy ) ) external_decl() ; else
		{
			if ( forwd_count )
			{
				forwd_count = ( psint ) 0 ;
				forwds( ( pntr ) env_list->class.cons.hd ) ;
			}
			if ( Streq( symb,let_sy ) ) let_decl() ; else
			t = clause() ;
		}
		if ( sequence_more() ) separator() ;
	}

	if ( forwd_count ) forwds( ( pntr ) env_list->class.cons.hd ) ;
	forwd_count = save ;
#ifdef	OUTLINE
	if ( !prelude ) retract( level,plevel,t ) ;
#else
	retract( level,plevel,t ) ;
#endif	OUTLINE
	return( t ) ;
}

