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
extern	psint	trademark,max_trademark,forwd_count,last_retract ;
extern	psint	ext ;
extern	pntr	struct_list,proc_decl_list,str_decl_list,ex_decl_list ;
extern	pntr	env_list,expr_t,procedure_type ;
extern	char	procedure_name[],proc_name[] ;

pntr	type()
{
	if ( Streq( symb,int_sy ) ) { next_sy(); return INT; } else
	if ( Streq( symb,pntr_sy ) ) { next_sy(); return PNTR; }  else
	if ( Streq( symb,string_sy ) ) { next_sy(); return STRING; }  else
	if ( Streq( symb,cint_sy ) ) { next_sy(); return CINT; }  else
	if ( Streq( symb,cpntr_sy ) ) { next_sy(); return CPNTR; }  else
	if ( Streq( symb,cstring_sy ) ) { next_sy(); return CSTRING; }  else
	if ( Streq( symb,real_sy ) ) { next_sy(); return REAL; }  else
	if ( Streq( symb,creal_sy ) ) { next_sy(); return CREAL; }  else
	if ( Streq( symb,bool_sy ) ) { next_sy(); return BOOL; }  else
	if ( Streq( symb,cbool_sy ) ) { next_sy(); return CBOOL; }  else
	if ( Streq( symb,file_sy ) ) { next_sy(); return FILET; }  else
	if ( Streq( symb,cfile_sy ) ) { next_sy(); return CFILET; }  else
#ifdef	OUTLINE
	if ( Streq( symb,pic_sy ) ) { next_sy(); return PIC; }  else
	if ( Streq( symb,cpic_sy ) ) { next_sy(); return CPIC; }  else
#endif	OUTLINE
#ifdef	RASTER
	if ( Streq( symb,pixel_sy ) ) { next_sy(); return PIXEL; }  else
	if ( Streq( symb,cpixel_sy ) ) { next_sy(); return CPIXEL; }  else
	if ( Streq( symb,hash_sy ) )
	{
		next_sy() ;
		if ( have( pixel_sy ) ) return IMAGE; else { mustbe( cpixel_sy ); return CIMAGE; }
	} else
#endif	RASTER
	if ( Streq( symb,star_sy ) ) { next_sy(); return ( mk_vector( type() ) ); } else
	if ( Streq( symb,identifier_sy ) && !strcmp( the_name,"c" ) )
	{
		next_sy() ;
#ifdef	RASTER
		if ( have( star_sy ) )
		{
			return( mk_const( mk_vector( type() ) ) ) ;
		} else
		{
			mustbe( hash_sy ) ;
			if ( have( pixel_sy ) ) return( mk_const( IMAGE ) ); else { mustbe( cpixel_sy ); return( mk_const( CIMAGE ) ); }
		}
#else
		mustbe( star_sy ) ;
		return( mk_const( mk_vector( type() ) ) ) ;
#endif	RASTER
	} else
	{
		syntax( "type" ) ;
		return ANY ;
	}
}

let_decl()
{
	register psint	eq ;
	char	n[ MAXSIZE ] ;
	register pntr	t = nil ;
	
	next_sy() ;
	( void ) strcpy( n,the_name ) ;
	check_name( n ) ;
	mustbe( identifier_sy ) ;
	eq = have( eq_sy ) ;
	if ( !eq ) mustbe( assign_sy ) ;
	t = clause() ;
	if ( scalar_eq( t,VOID ) || t->skind == FIELD_CLASS ) { bad_type( t ) ; t = ANY ; }
	if ( eq ) t = mk_const( t ) ; else t = mk_var( t ) ;
	declare( n,t,TRUE ) ;
}

pntr	struct_param()
{
	pntr	t,t1,start ;

	start = mk_namelist( star_sy,nil,nil ) ;
	t = start ;
	do {
		t1 = type() ;
		do {
			( pntr ) t->class.namelist.next = mk_namelist( the_name,t1,nil ) ;
			check_name( the_name ) ;
			mustbe( identifier_sy ) ;
			t = ( pntr ) t->class.namelist.next ;
		} while ( have( comma_sy ) ) ;
	} while ( have( semi_sy ) ) ;

	return( ( pntr ) start->class.namelist.next ) ;
}

pntr	struct_decl()
{
	char	n[ MAXSIZE ] ;
	register pntr	ans,ans1,t,t2,p,ptypes,sptypes,start,list ;
	psint	pointers,total,offset ;

	next_sy() ;
	( void ) strcpy( n,the_name ) ;
	ans = mk_structure( nil ) ;
	declare( n,ans,FALSE ) ;
	mustbe( identifier_sy ) ;
	if ( have( lp_sy ) )
	{
		t2 = struct_param() ;
		pointers = ( psint ) 0 ; total = st_size ; offset = st_size ;
		ptypes = mk_cons( nil,nil ) ; sptypes = ptypes ;
		start = mk_namelist( star_sy,nil,nil ) ; list = start ;
		t = t2 ;
		while ( t->skind != NIL )
		{
			( pntr ) ptypes->class.cons.tl = mk_cons( ( pntr )t->class.namelist.types,nil ) ;
			ptypes = ( pntr ) ptypes->class.cons.tl ;
			if ( pointer( ( pntr ) t->class.namelist.types ) )
			{
				pointers += ( psint ) 1 ;
				declare( t->class.namelist.names,mk_field( ( pntr ) t->class.namelist.types,offset ),FALSE ) ;
				offset += st_size ;
			} else
			{
				( pntr ) list->class.namelist.next = mk_namelist( t->class.namelist.names,( pntr )t->class.namelist.types,nil ) ;
				list = ( pntr ) list->class.namelist.next ;
			}
			total += type_size( ( pntr ) t->class.namelist.types ) ;
			t = ( pntr ) t->class.namelist.next ;
		}
		t = ( pntr ) start->class.namelist.next ;
		while ( t->skind != NIL )
		{
			declare( t->class.namelist.names,mk_field( ( pntr ) t->class.namelist.types,offset ),FALSE ) ;
			offset += type_size( ( pntr ) t->class.namelist.types ) ;
			t = ( pntr ) t->class.namelist.next ;
		}
		ans1 = mk_structure( ( pntr )sptypes->class.cons.tl ) ;
		p = search( n,( pntr ) env_list->class.cons.hd ) ;
		( pntr ) p->class.link.type1 = ans1 ;
		mustbe( rp_sy ) ;
		return( mk_struct_table( total,pointers,ans1 ) ) ;
	} else return( mk_struct_table( st_size2,( psint ) 0,ans ) ) ;
}

structure_decl()
{
	pntr	a ;

	load_trademark( trademark ) ;
	if ( trademark >= max_trademark )
	{
		trademark = ( psint ) 0 ;
		err_mess( "Too many structures" ) ;
	}
	a = struct_decl() ;
	( pntr ) struct_list->class.struct_table.str_next = mk_struct_table( a->class.struct_table.s_tdm,a->class.struct_table.s_ptrs,nil ) ;
	struct_list = ( pntr ) struct_list->class.struct_table.str_next ;
	trademark += ( psint ) 4 ;
}

pntr	get_types( function )
psint	function ;
{
	register psint	res ;
	register pntr	arg_types = nil,restype = VOID,t = nil ;
	if ( !have( rp_sy ) )
	{
		res = ( function && have( arrow_sy ) ) ;
		if ( !res )
		{
			if ( function ) arg_types = mk_cons( param_type( FALSE ),nil ); else arg_types = mk_cons( type(),nil ) ;
			t = arg_types ;
			while ( have( comma_sy ) )
			{
				if ( function ) ( pntr ) t->class.cons.tl = mk_cons( param_type( FALSE ),nil ); else ( pntr ) t->class.cons.tl = mk_cons( type(),nil ) ;
				t = ( pntr ) t->class.cons.tl ;
			}
			if ( function ) res = have( arrow_sy ); else res = FALSE ;
		}
		if ( res )
		{
			restype = type() ;
			if ( restype->skind == CONST_CLASS ) bad_type( restype ) ;
		}
		mustbe( rp_sy ) ;
	}
	if ( function ) return( mk_proc( arg_types,restype,FALSE,FALSE ) ) ;
	else return( mk_structure( arg_types ) ) ;
}

external_decl()
{
	char	n[ MAXSIZE ] ;
	pntr	t ;

	next_sy() ;
	( void ) strcpy( n,the_name ) ;
	check_name( n ) ;
	mustbe( identifier_sy ) ;
	if ( have( lp_sy ) ) t = get_types( TRUE ) ; else
	t = mk_proc( nil,VOID,FALSE,FALSE ) ;
	external_op( n,t ) ;
	declare( n,t,TRUE ) ;
}
     
forward_decl()
{
	char	n[ MAXSIZE ] ;
	pntr	t ;

	next_sy() ;
	( void ) strcpy( n,the_name ) ;
	check_name( n ) ;
	mustbe( identifier_sy ) ;
	if ( have( lp_sy ) ) t = get_types( TRUE ) ; else
	t = mk_proc( nil,VOID,TRUE,FALSE ) ;
	t->class.proc.forwd = TRUE ;
	forwd_count += ( psint ) 1 ;
	forward_op() ;
	declare( n,t,TRUE ) ;
}
 
pntr	param_type( names )
psint	names ;
{
	pntr	p ;

	if ( Streq( symb,structure_sy ) )
	{
		if ( names )
		{
			push_ms() ;
			p = struct_decl() ;
			return( ( pntr ) p->class.struct_table.str_next ) ;
		}
		else
		{
			next_sy() ;
			if ( have( lp_sy ) ) return( get_types( FALSE ) ) ; else
			return( mk_structure( nil ) ) ;
		}
	} else
	if ( Streq( symb,lp_sy ) )
	{
		next_sy() ;
		return( get_types( TRUE ) ) ;
	} else return( type() ) ;
}

pntr	param_decl()
{
	register psint more1 = TRUE,more2 ;
	register pntr	t = nil,t1 = nil,start = nil,this = nil ;
	char	n[ MAXSIZE ] ;
	start = mk_cons( nil,nil ) ;
	t = start ;
	while( more1 )
	{
		more2 = TRUE ;
		t1 = param_type( TRUE ) ;
		( pntr ) t->class.cons.tl = mk_cons( t1,nil ) ;
		t = ( pntr ) t->class.cons.tl ;
		if ( t1->skind != STRUCTURE_CLASS )
		while( more2 )
		{
			( void ) strcpy( n,the_name ) ;
			inc_stack( t1 ) ;
			if ( t1->skind == PROC_CLASS || t1->skind == CONST_CLASS ) this = t1 ;
			else this = mk_var( t1 ) ;
			declare( n,this,FALSE ) ;
			mustbe( identifier_sy ) ;
			if ( have( comma_sy ) )
			{
				( pntr ) t->class.cons.tl = mk_cons( t1,nil ) ;
				t = ( pntr ) t->class.cons.tl ;
			} else more2 = FALSE ;
		}
		if ( !have( semi_sy ) ) more1 = FALSE ;
	}
	return( ( pntr ) start->class.cons.tl ) ;
}

pntr	proc_decl()
{
	char	n[ MAXSIZE ],nn[ MAXSIZE ] ;
	pntr	the_type,save_pd,save_struct,save_ex,arg_types,restype ;
	pntr	pentry,ptype ;
	psint	level,plevel,max_m,max_p,save_ext,cb,code_p,n1 ;

	next_sy() ;
	( void ) strcpy( n,the_name ) ;
	inc_ms( st_size2 ) ;
	the_type = mk_proc( nil,VOID,FALSE,FALSE ) ;
	declare( n,the_type,FALSE ) ;
	level = ssp ; plevel = psp ;
	max_m = max_ms ; max_p = max_ps ;
	psp = ( psint ) 0 ; max_ps = psp ;
	cb = code_base ; code_p = cp ;
	code_base += cp ; cp = ( psint ) 4 ;
	save_pd = mk_p_decl( proc_decl_list,code_p + ( psint ) 3 ) ;
	last_retract = ( psint ) -1 ; proc_decl_list = nil ;
	save_struct = str_decl_list ; str_decl_list = nil ;
	save_ex = ex_decl_list ; ex_decl_list = nil ;
	lex_level += ( psint ) 1 ;
	enter_scope() ;
	ssp = st_size2 ;
	declare( n,the_type,FALSE ) ;
	ssp = mscw_size ; max_ms = ssp ;
	enter_scope() ;
	( void ) strcpy( nn,proc_name ) ; ( void ) strcpy( proc_name,n ) ;
	mustbe( identifier_sy ) ;
	arg_types = nil ; restype = VOID ;
	if ( have( lp_sy ) )
	{
		if ( !Streq( symb,arrow_sy ) && !Streq( symb,rp_sy ) ) arg_types = param_decl() ;
		if ( have( arrow_sy ) )	
		{
			restype = type() ;
			if ( restype->skind == CONST_CLASS ) bad_type( restype ) ;
		}
		mustbe( rp_sy ) ;
	}
	( pntr ) the_type->class.proc.args = arg_types ; ( pntr ) the_type->class.proc.result = restype ;
	pentry = search( n,( pntr ) ( ( pntr ) ( ( pntr ) env_list->class.cons.tl )->class.cons.tl )->class.cons.hd ) ;
	ptype = ( pntr ) pentry->class.link.type1 ;
	if ( ptype->skind == PROC_CLASS && ptype->class.proc.forwd )
	{
		if ( eq( ( pntr ) ptype->class.proc.args,arg_types ) && eq( ( pntr ) ptype->class.proc.result,restype ) )
		{
			forwd_count -= ( psint ) 1 ;
			ptype->class.proc.forwd = FALSE ;
		} else name_err( n ) ;
	}
	separator() ;
	if ( ext ) load_struct_base() ;
	load_proc_name( n ) ;
	save_ext = ext ; ext = FALSE ;
	match( restype,clause() ) ;
	ext = save_ext ;
	exit_scope() ; exit_scope() ;
	return_op( restype ) ;
	n1 = cp ; lex_level -= ( psint ) 1 ;
	code_base = cb ; cp = code_p ;
	ssp = level ; psp = plevel ;
	( void ) strcpy( proc_name,nn ) ;
	endsegment( n1,pentry->class.link.addr2 ) ;
	proc_decl_list = save_pd ;
	str_decl_list = save_struct ; ex_decl_list = save_ex ;
	max_ms = max_m ; max_ps = max_p ;
	if ( ext ) ( void ) strcpy( procedure_name,n ) ; procedure_type = the_type ;
}

pntr	block()
{
	char	*last ;
	pntr	t,t1 ;

	block_count += ( psint ) 1 ;
	if ( Streq( symb,lcb_sy ) ) last = rcb_sy ; else last = end_sy ;
	next_sy() ;
	if( Streq( symb,last ) ) t = VOID ; else
	{
		enter_scope() ;
		t1 = sequence() ;
		exit_scope() ;
		t = t1 ;
	}
	block_count -= ( psint ) 1 ;
	mustbe( last ) ;
	return( t ) ;
}

#ifdef	OUTLINE
pntr	rotate_pic()
{
	register psint	level,plevel ;

	level = ssp ;
	plevel = psp ;
	( void ) lookup( "trnsfrm.strc",TRUE ) ;
	next_sy() ;
	load_lit( INT,( psint ) 3 ) ;
	match( PIC,clause() ) ;
	mustbe( by_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	load_real( ( psreal ) 0.0 ) ;
	reverse_stack( REAL ) ;
	form_structure( level,plevel ) ;
	return( PIC ) ;
}

pntr	colour_pic()
{
	register psint	level,plevel ;

	level = ssp ;
	plevel = psp ;
	( void ) lookup( "culr.strc",TRUE ) ;
	next_sy() ;
	match( PIC,clause() ) ;
	mustbe( in_sy ) ;
	match( PIXEL,clause() ) ;
	form_structure( level,plevel ) ;
	return( PIC ) ;
}

pntr	transform_pic()
{
	register psint	level,plevel,Scale ;

	level = ssp ;
	plevel = psp ;
	Scale = ( Streq( symb,scale_sy ) ) ;
	next_sy() ;
	( void ) lookup( "trnsfrm.strc",TRUE ) ;
	load_lit( INT,Scale ? ( psint ) 1 : ( psint ) 2 ) ;
	match( PIC,clause() ) ;
	mustbe( by_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	mustbe( comma_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	form_structure( level,plevel ) ;
	return( PIC ) ;
}

pntr	text_pic()
{
	register psint	level,plevel ;

	level = ssp ;
	plevel = psp ;
	( void ) lookup( "scrbl.strc",TRUE ) ;
	next_sy();	
	match( STRING,clause() ) ;
	mustbe( from_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	mustbe( comma_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	mustbe( to_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	mustbe( comma_sy ) ;
	match( REAL,clause() ) ;
	reverse_stack( REAL ) ;
	form_structure( level,plevel ) ;
	return( PIC ) ;
}
#endif	OUTLINE

#ifdef	RASTER
pntr	image_exp()
{
	next_sy() ;
	match( INT,clause() ) ;
	mustbe( by_sy ) ;
	match( INT,clause() );	
	mustbe( of_sy ) ;
	match( PIXEL,clause() ) ;
	image_op() ;
	return( IMAGE ) ;
}

pntr	limit_exp()
{
	register pntr	t ;
	register psint	to_symb ;

	next_sy() ;
	t = clause() ;
	match( image_type,t ) ;
	to_symb = have( to_sy ) ;
	if ( to_symb )
	{
		match( INT,clause() ) ;
		mustbe( by_sy ) ;
		match( INT,clause() ) ;
	}
	if ( have( at_sy ) )
	{
		match( INT,clause() ) ;
		mustbe( comma_sy ) ;
		match( INT,clause() ) ;
	} else { load_lit( INT,( psint ) 0 ) ; load_lit( INT,( psint ) 0 ) ; }
	limit_op( to_symb ) ;
	return( t ) ;
}
#endif	RASTER

std_arg1( t )
pntr	t ;
{
	next_sy() ;
	mustbe( lp_sy ) ;
	match( t,clause() ) ;
	mustbe( rp_sy ) ;
}

pntr	format( n,ms )
psint	n,ms ;
{
	psint	i;

	mst_external( n ) ;
	next_sy() ;
	mustbe( lp_sy ) ;
	match( REAL,clause() ) ;
	if ( n != ( psint ) 2 ) 
		for ( i = ( psint ) 1 ; i <= ( psint ) 2 ; i++ )
		{
			mustbe( comma_sy ) ;
			match( INT,clause() ) ;
		}
	mustbe( rp_sy ) ;
	apply_op( ms,( psint ) 0 ) ;
	push_ps() ;
	return( STRING ) ;
}

std_do_io()
{
	next_sy() ;
	if ( Streq( symb,lp_sy ) ) 
	{
		next_sy() ;
		if ( Streq( symb,rp_sy ) )
		{
			match( FILET,lookup( "s.i",TRUE ) ) ;
			next_sy() ;
		} else
		{
			match( FILET,clause() ) ;
			mustbe( rp_sy ) ;
		}
	} else match( FILET,lookup( "s.i",TRUE ) ) ;
}

pntr	vec_bound( n )
psint	n ;
{
	register pntr	t = nil ;
	next_sy() ;
	mustbe( lp_sy ) ;
	t = clause() ;
	if ( t->skind == CONST_CLASS ) t = ( pntr ) t->class.CONST.content ;
	match( vector_type,t ) ;
/*
	if ( t->skind == VECTOR_CLASS ) bound( n,t->class.VECTOR.elms ) ;
*/
	if ( t->skind == VECTOR_CLASS ) bound( n ) ;
	mustbe( rp_sy ) ;
	return( INT ) ;
}

pntr	standard_func()
{
	register pntr	t ;

	if ( Streq( symb,abort_sy ) ) { next_sy() ; finish_op() ; return( VOID ) ; } else
	if ( Streq( symb,upb_sy ) ) return( vec_bound( ( psint ) 0 ) ) ; else
	if ( Streq( symb,lwb_sy ) ) return( vec_bound( ( psint ) 1 ) ) ; else
	if ( Streq( symb,float_sy ) ) { std_arg1( INT ) ; float_op( ( psint ) 1 ) ; return( REAL ) ; } else
	if ( Streq( symb,eof_sy ) ) { std_do_io() ; read_op( ( psint ) 6 ) ; return( BOOL ) ; } else
	if ( Streq( symb,read_sy ) ) { std_do_io() ; read_op( ( psint ) 5 ); return( STRING ); } else
	if ( Streq( symb,peek_sy ) ) { std_do_io(); read_op( ( psint ) 4 ); return( STRING ); } else
	if ( Streq( symb,reads_sy ) ) { std_do_io(); read_op( ( psint ) 1 ); return( STRING ); } else
	if ( Streq( symb,readi_sy ) ) { std_do_io(); read_op( ( psint ) 0 ); return( INT ); } else
	if ( Streq( symb,readr_sy ) )
	{
		mst_external( ( psint ) 4 ) ;
		std_do_io() ;
		apply_op( ( psint ) 0,st_size ) ;
		inc_ms( st_size2 ) ;
		return( REAL ) ;
	} else
	if ( Streq( symb,readb_sy ) ) { std_do_io(); read_op( ( psint ) 2 ); return( BOOL ); } else
	if ( Streq( symb,read_16_sy ) ) { std_do_io(); read_op( ( psint ) 9 ); return( INT ); } else
	if ( Streq( symb,read_32_sy ) ) { std_do_io(); read_op( ( psint ) 10 ) ; return( INT ); } else
	if ( Streq( symb,eformat_sy ) ) return ( format( ( psint ) 1,st_size2 + ( psint ) 2 * st_size ) ) ; else
	if ( Streq( symb,fformat_sy ) ) return ( format( ( psint ) 0,st_size2 + ( psint ) 2 * st_size ) ) ; else
	if ( Streq( symb,gformat_sy ) ) return ( format( ( psint ) 2,st_size2 ) ) ; else
	if ( Streq( symb,read_a_line_sy ) ) { std_do_io(); read_op( ( psint ) 8 ); return( STRING ); } else
	if ( Streq( symb,read_byte_sy ) ) { std_do_io(); read_op( ( psint ) 3 ); return( INT ); } else
	if ( Streq( symb,read_name_sy ) )
	{
		next_sy() ;
		mustbe( lp_sy ) ;
		t = clause() ;
		if ( eq2( FILET,t ) ) { mustbe( comma_sy ); match( STRING,clause() ); } else
		{ match( STRING,t ); match( FILET,lookup( "s.i",TRUE ) ); reverse_stack( STRING ); }
		read_op( ( psint ) 7 ) ;
		mustbe( rp_sy ) ;
		return( STRING ) ;
	} else
#ifdef	OUTLINE
	if ( Streq( symb,pic_pntr_sy ) ) { std_arg1( PIC ); return( PNTR ); } else
#endif	OUTLINE
	{
		syntax( "expression" ) ;
		next_sy() ;
		return( ANY ) ;
	}
}

pntr	args_list( t )
pntr	t ;
{
	register pntr	t1 ;
	register psint	more1 = TRUE,level,plevel ;

	level = ssp - st_size ;
	plevel = psp ;
	t1 = ( pntr ) t->class.STRUCTURE.fields ;
	if ( t1->skind == NIL ) load_lit( INT,( psint ) 0 ) ; else
	{
		mustbe( lp_sy ) ;
		while ( more1 )
		{
			match( ( pntr )t1->class.cons.hd,clause() ) ;
			if ( eq2( REAL,( pntr )t1->class.cons.hd ) ) reverse_stack( REAL ) ;
			t1 = ( pntr ) t1->class.cons.tl ;
			if ( t1->skind == NIL || !have( comma_sy ) ) more1 = FALSE ;
		}
		if ( t1->skind != NIL ) syntax( "field" ) ;
		mustbe( rp_sy ) ;
	}
	form_structure( level,plevel ) ;
	return( PNTR ) ;
}

pntr	proc_call( p )
pntr	p ;
{
	pntr	t,t1,t2,t3 ;
	psint	level,plevel ;

	t = ( pntr ) p->class.link.type1 ;
	t1 = ( pntr ) t->class.proc.args ;
	if ( mark_stack( p ) )
	{
		level = ssp ; plevel = psp ;
		if ( t1->skind != NIL )
		{
			mustbe( lp_sy ) ;
			do {
				t2 = ( pntr ) t1->class.cons.hd ;
				switch ( t2->skind )
				{
				case PROC_CLASS	: match( t2,lookup( the_name,TRUE ) ) ;
						  next_sy() ;
						  break ;
				case STRUCTURE_CLASS :
						  t3 = lookup( the_name,TRUE ) ;
						  if ( t3->skind == STRUCTURE_CLASS && eq( ( pntr )t2->class.STRUCTURE.fields,( pntr )t3->class.STRUCTURE.fields ) )
						  bad_types( t2,t3 ) ;
						  next_sy() ;
						  break ;
				default		: match( t2,clause() ) ;
						  break ;
				}
				t1 = ( pntr ) t1->class.cons.tl ;
			} while ( t1->skind != NIL && have( comma_sy ) ) ;
			if ( t1->skind != NIL ) syntax( "parameter" ) ;
			mustbe( rp_sy ) ;
/*
		} else if ( have( lp_sy ) ) mustbe( rp_sy ) ;
*/
		} 
		apply_op( ssp - level,psp - plevel ) ;
/*
	} else if ( have( lp_sy ) ) mustbe( rp_sy ) ;
*/
	} 
	t = ( pntr ) t->class.proc.result ;
	inc_stack( t ) ;
	return( t ) ;
}

le1( n,p )
psint	n,p ;
{
	register psint	err, code1, cp1 ;

	if ( n > p )
	{
		err = error ;
		code1 = coding ;
		cp1 = cp ;
		err_mess( "** Warning higher precedence expression\n" ) ;
		error = err ;
		coding = code1 ;
		cp = cp1 ;
	}
	next_sy() ;
}

ass_err( t )
pntr	t ;
{
	char	s[ MAXSIZE ] ;
	( void ) strcpy( s,"Type " ) ;
	( void ) strcat( s,display( t ) ) ;
	( void ) strcat( s," may not be assigned to\n" ) ;
	err_mess( s ) ;
}
