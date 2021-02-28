#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "stype.h"
#include "functions.h"

extern	pntr	nil,INT,CINT,REAL,CREAL,BOOL,CBOOL,PNTR,CPNTR,
        	STRING,CSTRING,FILET,CFILET,
#ifdef	RASTER
		PIXEL,CPIXEL,IMAGE,CIMAGE,image_type,
#endif	RASTER
#ifdef	OUTLINE
        	PIC,CPIC,
#endif	OUTLINE
		VOID,ANY,vector_type;

extern	pntr	proc_decl_list,str_decl_list,ex_decl_list,external_list,
		procedure_type,struct_list ;

extern	char	*malloc() ;
char	*copy_string( s )
char	*s ;
/* get some space and make a copy of s */
{
	register char	*t ;
	t = malloc( ( unsigned )( strlen( s ) + 1 ) ) ;
	( void ) strcpy( t,s ) ;
	return( t ) ;
}

pntr	mk_scalar( s )
char	*s ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = SCALAR_CLASS ;
	( void ) strcpy( p->class.scalar.t_name,s ) ;
	return( p ) ;
}

pntr	mk_vector( t )
pntr	t ;
{
	register pntr	p ;
	p = ( pntr ) malloc( stype_size ) ;
	p->skind = VECTOR_CLASS ;
	( pntr *) p->class.VECTOR.elms = t ;
	return( p ) ;
}

pntr	mk_proc( s,t,u,v )
pntr	s,t ;
psint	u,v ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = PROC_CLASS ;
	( pntr ) p->class.proc.args = s ;
	( pntr ) p->class.proc.result = t ;
	p->class.proc.forwd = u ;
	p->class.proc.stand = v ;
	return( p ) ;
}

pntr	mk_var( t )
pntr	t ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = VAR_CLASS ;
	( pntr ) p->class.var.contents = t ;
	return( p ) ;
}

pntr	mk_structure( f )
pntr	f ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = STRUCTURE_CLASS ;
	( pntr ) p->class.STRUCTURE.fields = f ;
	return( p ) ;
}

pntr	mk_field( t,o )
pntr	t ;
psint	o ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = FIELD_CLASS ;
	( pntr ) p->class.field.fieldt = t ;
	p->class.field.f_offset = o ;
	return( p ) ;
}

pntr	mk_cons( h,t )
pntr	h,t ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = CONS_CLASS ;
	( pntr ) p->class.cons.hd = h ;
	( pntr ) p->class.cons.tl = t ;
	return( p ) ;
}

pntr	mk_struct_table( st,sp,sty )
psint	st,sp ;
pntr	sty ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = STRUCT_TABLE_CLASS ;
	p->class.struct_table.s_tdm = st ;
	p->class.struct_table.s_ptrs = sp ;
	( pntr ) p->class.struct_table.str_next = sty ;
	return( p ) ;
}

pntr	mk_const( t )
pntr	t ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = CONST_CLASS ;
	( pntr ) p->class.CONST.content = t ;
	return( p ) ;
}

pntr	mk_string( i,t )
psint	i ;
pntr	t ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = STR_CLASS ;
	p->class.str.str_int = i ;
	( pntr ) p->class.str.str_pntr = t ;
	return( p ) ;
}

pntr	mk_p_decl( t,i )
pntr	t ;
psint	i ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = P_DECL_CLASS ;
	( pntr ) p->class.p_decl.p_pntr = t ;
	p->class.p_decl.p_int = i ;
	return( p ) ;
}

pntr	mk_s_decl( t,i )
pntr	t ;
psint	i ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = S_DECL_CLASS ;
	( pntr ) p->class.s_decl.s_pntr = t ;
	p->class.s_decl.s_int = i ;
	return( p ) ;
}

pntr	mk_external( n,t,a,nt )
char	*n ;
pntr	t ;
psint	a ;
pntr	nt ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = EXTERNAL_CLASS ;
	p->class.EXTERNAL.e_name = copy_string( n ) ;
	( pntr ) p->class.EXTERNAL.e_type = t ;
	p->class.EXTERNAL.e_addr = a ;
	( pntr ) p->class.EXTERNAL.e_next = nt ;
	return( p ) ;
}

#ifdef	RASTER
pntr	mk_image( t )
pntr	t ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = IMAGE_CLASS ;
	( pntr ) p->class.Image.image_elements = t ;
	return( p ) ;
}
#endif	RASTER

pntr	mk_link( s,a1,a2,t,l,r )
char	*s ;
psint	a1,a2 ;
pntr	t,l,r ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = LINK_CLASS ;
	p->class.link.name = copy_string( s ) ;
	p->class.link.addr1 = a1 ;
	p->class.link.addr2 = a2 ;
	( pntr ) p->class.link.type1 = t ;
	( pntr ) p->class.link.left = l ;
	( pntr ) p->class.link.right = r ;
	return( p ) ;
}

pntr	mk_namelist( s,t,n )
char	*s ;
pntr	t,n ;
{
	register pntr	p ;

	p = ( pntr ) malloc( stype_size ) ;
	p->skind = NAMELIST_CLASS ;
	p->class.namelist.names = copy_string( s ) ;
	( pntr ) p->class.namelist.types = t ;
	( pntr ) p->class.namelist.next = n ;
	return( p ) ;
}

pntr	mk_nil()
{
	register pntr	p ;
	p = ( pntr ) malloc( stype_size ) ;
	p->skind = NIL ;
	return( p ) ;
}

init_types()
{
	nil = mk_nil() ;

	INT = mk_scalar( "int" );	CINT = mk_const( INT ) ;
	REAL = mk_scalar( "real" );	CREAL = mk_const( REAL ) ;
	BOOL = mk_scalar( "bool" );	CBOOL = mk_const( BOOL ) ;
	PNTR = mk_scalar( "pntr" );	CPNTR = mk_const( PNTR ) ;
	STRING = mk_scalar( "string" );	CSTRING = mk_const( STRING ) ;
	FILET = mk_scalar( "file" );	CFILET = mk_const( FILET ) ;
#ifdef	RASTER
	PIXEL = mk_scalar( "pixel" );	CPIXEL = mk_const( PIXEL ) ;
	IMAGE = mk_image( PIXEL );	CIMAGE = mk_image( CPIXEL ) ;
#endif	RASTER
#ifdef	OUTLINE
	PIC = mk_scalar( "pic" );	CPIC = mk_const( PIC ) ;
#endif	OUTLINE
	VOID = mk_scalar( "void" );	ANY = mk_scalar( "undefined" ) ;
	vector_type = mk_vector( ANY ) ;
#ifdef	RASTER
	image_type = mk_image( ANY ) ;
#endif	RASTER
	proc_decl_list = nil ;
	str_decl_list = nil ;
	ex_decl_list = nil ;
	external_list = nil ;
	procedure_type = nil ;
	struct_list = mk_struct_table( ( psint ) 0,( psint ) 0,nil ) ;
}

psint eq1( a,b )
register pntr	a,b ;
{
	if ( a == b || a == ANY || b == ANY ||
	     a->skind == VECTOR_CLASS && b->skind == VECTOR_CLASS && eq1( ( pntr )a->class.VECTOR.elms,( pntr )b->class.VECTOR.elms ) ||
#ifdef	RASTER
	     a->skind == IMAGE_CLASS && b->skind == IMAGE_CLASS && eq1( ( pntr )a->class.Image.image_elements,( pntr )b->class.Image.image_elements ) ||
#endif	RASTER
	     a->skind == CONST_CLASS && b->skind == CONST_CLASS && eq1( ( pntr )a->class.CONST.content,( pntr )b->class.CONST.content ) ) return( TRUE ) ;
	else return( FALSE ) ;
}

psint eq( a,b )
register pntr	a,b ;
{
	if ( a == b || a == ANY || b == ANY ||
	     a->skind == CONST_CLASS && eq( ( pntr )a->class.CONST.content,b ) ||
	     b->skind == CONST_CLASS && eq( a,( pntr )b->class.CONST.content ) ||
	     a->skind == VECTOR_CLASS && b->skind == VECTOR_CLASS && eq1( ( pntr )a->class.VECTOR.elms,( pntr )b->class.VECTOR.elms ) ||
#ifdef	RASTER
	     a->skind == IMAGE_CLASS && b->skind == IMAGE_CLASS && eq1( ( pntr )a->class.Image.image_elements,( pntr )b->class.Image.image_elements ) ||
#endif	RASTER
	     a->skind == PROC_CLASS && b->skind == PROC_CLASS && eq( ( pntr )a->class.proc.args,( pntr )b->class.proc.args ) && eq( ( pntr )a->class.proc.result,( pntr )b->class.proc.result ) ||
	     a->skind == CONS_CLASS && b->skind == CONS_CLASS && eq( ( pntr )a->class.cons.hd,( pntr )b->class.cons.hd ) && eq( ( pntr )a->class.cons.tl,( pntr )b->class.cons.tl ) ) return( TRUE ) ;
	else return( FALSE ) ;
}

psint eq2( a,b )
register pntr	a,b ;
{
	if ( a != ANY && b != ANY && eq( a,b ) ) return( TRUE ); else return( FALSE ) ;
}

