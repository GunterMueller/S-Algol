


























 

















                                                                   































 


































                                                                   
























































































typedef unsigned long   size_t;




typedef long    fpos_t;




















































































typedef struct {
	int	_cnt;
	unsigned char	*_ptr;
	unsigned char	*_base;
	int	_bufsiz;
	short	_flag;
	short	_file;


	char	*__newbase;
	void	*_lock;			
	unsigned char	*_bufendp;
} FILE;

extern FILE	_iob[];









extern int     fread();
extern int     fwrite();



extern int	_flsbuf ();
extern int	_filbuf ();
extern int 	ferror ();
extern int 	feof ();
extern void 	clearerr ();
extern int 	putchar ();
extern int 	getchar ();
extern int 	putc ();
extern int 	getc ();
extern int	remove ();
extern int	rename ();
extern FILE 	*tmpfile ();
extern char 	*tmpnam ();
extern int 	fclose ();
extern int 	fflush ();
extern FILE	*fopen ();
extern FILE 	*freopen ();
extern void 	setbuf ();
extern int 	setvbuf ();
extern int	fprintf ();
extern int	fscanf ();
extern int	printf ();
extern int	scanf ();
extern int	sprintf ();
extern int	sscanf ();







































































typedef struct {
	char	*_a0;		
	int	_offset;		
} va_list;


extern int  vfprintf ();
extern int  vprintf ();
extern int  vsprintf ();



extern int 	fgetc ();
extern char 	*fgets ();
extern int 	fputc ();
extern int 	fputs ();
extern char 	*gets ();
extern int 	puts ();
extern int	ungetc ();
extern int	fgetpos ();
extern int 	fseek ();
extern int	fsetpos ();
extern long	ftell ();
extern void	rewind ();
extern void 	perror ();






































































 











































































 


































                                                                   











typedef signed long     ptrdiff_t;










    typedef unsigned int  wchar_t;






typedef unsigned int wctype_t;






typedef int            time_t;




typedef int             clock_t;











typedef long                    ssize_t; 





typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef unsigned long	ulong_t;
typedef	volatile unsigned char	vuchar_t;
typedef	volatile unsigned short	vushort_t;
typedef	volatile unsigned int	vuint_t;
typedef volatile unsigned long	vulong_t;















typedef	struct	{ long r[1]; } *physadr_t;




typedef	struct	label_t	{
	long	val[10];
} label_t;




typedef int		level_t;
typedef	int		daddr_t;	
typedef	char *		caddr_t;	
typedef long *		qaddr_t;        
typedef char *          addr_t;
typedef	uint_t		ino_t;		
typedef short		cnt_t;
typedef int		dev_t;		
typedef	int		chan_t;		


typedef long    off_t;			


typedef unsigned long	rlim_t;		
typedef	int		paddr_t;
typedef	ushort_t	nlink_t;



typedef int    		key_t;		




typedef	uint_t		mode_t;		




typedef uint_t		uid_t;		




typedef uint_t		gid_t;		


typedef	void *		mid_t;		



typedef	int		pid_t;		


typedef char		slab_t[12];	








typedef pid_t		id_t;		
					
					
					
					





typedef ulong_t		shmatt_t;	
typedef ulong_t		msgqnum_t;	
typedef ulong_t		msglen_t;	



        typedef unsigned int wint_t;         





typedef unsigned long	sigset_t;







typedef long            timer_t;        















typedef void (*sig_t)();




typedef uint_t	major_t;      
typedef uint_t	minor_t;      
typedef uint_t	devs_t;       
typedef uint_t	unit_t;       














































 











































typedef	unsigned long	vm_offset_t;
typedef	unsigned long	vm_size_t;







typedef	uchar_t		uchar;
typedef	ushort_t	ushort;
typedef	uint_t		uint;
typedef ulong_t		ulong;

typedef	physadr_t	physadr;



typedef	uchar_t		u_char;
typedef	ushort_t 	u_short;
typedef	uint_t		u_int;
typedef	ulong_t		u_long;
typedef	vuchar_t	vu_char;
typedef	vushort_t 	vu_short;
typedef	vuint_t		vu_int;
typedef	vulong_t	vu_long;


typedef struct  _quad { int val[2]; } quad;


typedef	long	swblk_t;
typedef u_long	fixpt_t;



















typedef int	fd_mask;






typedef	struct fd_set {
	fd_mask	fds_bits[(((4096)+(( (sizeof(fd_mask) * 8		)	)-1))/( (sizeof(fd_mask) * 8		)	))];
} fd_set;


















extern int 	fileno ();
extern FILE 	*fdopen ();


extern char *cuserid ();












































 


































 


































                                                                   



extern int getopt ();


extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;







extern char	*ctermid ();
extern int 	getw ();
extern int 	pclose ();
extern int 	putw ();
extern FILE 	*popen ();
extern char 	*tempnam ();


































 


































                                                                   






























 


































                                                                   








































 












































					



					



































































































 







































































					




































































 

















































                       





























































































extern void 	setbuffer ();
extern void 	setlinebuf ();































   typedef double	psreal ;
   typedef int		psint ;

































struct scalar{ char t_name[ 10 ] ; } ;
struct VECTOR{ void *elms ; } ;
struct proc{ void *args,*result ; psint forwd,stand ; } ;
struct var{ void *contents ; } ;
struct STRUCTURE{ void *fields ; } ;
struct field{ void *fieldt ; psint f_offset ; } ;
struct cons{ void *hd,*tl ; } ;
struct struct_table{ psint s_tdm,s_ptrs ; void *str_next ; } ;
struct CONST{ void *content ; } ;
struct str{ psint str_int ; void *str_pntr ; } ;
struct p_decl{ void *p_pntr ; psint p_int ; } ;
struct s_decl{ void *s_pntr ; psint s_int ; } ;
struct EXTERNAL{ char *e_name ; void *e_type ; psint e_addr ; void *e_next ; } ;

struct link{ char *name ; psint addr1,addr2 ; void *type1,*left,*right ; } ;
struct namelist{ char *names ; void *types,*next ; } ;



struct stype { psint skind ;
		union { struct scalar scalar ;
			struct VECTOR VECTOR ;
			struct proc proc ;
			struct var var ;
			struct STRUCTURE STRUCTURE ;
			struct field field ;
			struct cons cons ;
			struct struct_table struct_table ;
			struct CONST CONST ;
			struct str str ;
			struct p_decl p_decl ;
			struct s_decl s_decl ;
			struct EXTERNAL EXTERNAL ;

			struct link link ;
			struct namelist namelist ;
		      }	class ;
	      } ;

typedef	struct	stype *pntr ;





extern	psint	out_list() ;
extern	psint	gen_type0() ;
extern	psint	pointer() ;
extern	psint	type_size() ;
extern	psint	get_opcode() ;
extern	psint	get_arith_opcode() ;
extern	psint	cjump() ;
extern	psint	fjump() ;
extern	psint	jumpf() ;
extern	psint	jumpff() ;
extern	psint	jumptt() ;
extern	psint	get_retract_type() ;
extern	psint	for_test() ;
extern	psint	mark_stack() ;




extern	char	read_ch() ;
extern	char	next_ch() ;
extern	char	read_char() ;
extern	psint	reserved_word() ;
extern	psint	ok() ;
extern	psint	int_conv() ;
extern	psint	le() ;
extern	psreal	ex() ;
extern	psreal	real_conv() ;
extern	char	*char_to_string() ;
extern	psint	have() ;
extern	char	*display() ;
extern	pntr	int_real() ;
extern	pntr	coerce() ;
extern	pntr	enter() ;
extern	pntr	search() ;
extern	pntr	search_name() ;
extern	pntr	look_up() ;
extern	pntr	lookup() ;
extern	psint	output_name_type() ;
extern	psint	output_externals() ;




extern	FILE	*Create_code() ;
extern	FILE	*Open_source() ;
extern	FILE	*Open_prelude() ;
extern	FILE	*Open_std_decls() ;
extern	FILE	*Create_flsum() ;




extern	char	*copy_string() ;
extern	pntr	mk_scalar() ;
extern	pntr	mk_vector() ;
extern	pntr	mk_proc() ;
extern	pntr	mk_var() ;
extern	pntr	mk_structure() ;
extern	pntr	mk_field() ;
extern	pntr	mk_cons() ;
extern	pntr	mk_struct_table() ;
extern	pntr	mk_const() ;
extern	pntr	mk_string() ;
extern	pntr	mk_p_decl() ;
extern	pntr	mk_s_decl() ;
extern	pntr	mk_external() ;
extern	pntr	mk_image() ;
extern	pntr	mk_link() ;
extern	pntr	mk_namelist() ;
extern	pntr	mk_nil() ;
extern	psint	eq1() ;
extern	psint	eq() ;
extern	psint	eq2() ;




extern	pntr	unary_op() ;
extern	pntr	subscript() ;
extern	pntr	exp_list() ;
extern	psint	expr_le2() ;
extern	pntr	rel_type() ;
extern	pntr	expr() ;
extern	pntr	if_clause() ;
extern	pntr	for_clause() ;
extern	pntr	while_clause() ;
extern	pntr	repeat_clause() ;
extern	pntr	case_clause() ;
extern	pntr	write_clause() ;
extern	pntr	clause() ;
extern	psint	sequence_more() ;
extern	pntr	sequence() ;




extern	pntr	type() ;
extern	pntr	struct_param() ;
extern	pntr	struct_decl() ;
extern	pntr	get_types() ;
extern	pntr	param_type() ;
extern	pntr	param_decl() ;
extern	pntr	proc_decl() ;
extern	pntr	block() ;
extern	pntr	rotate_pic() ;
extern	pntr	colour_pic() ;
extern	pntr	transform_pic() ;
extern	pntr	text_pic() ;
extern	pntr	image_exp() ;
extern	pntr	limit_exp() ;
extern	pntr	format() ;
extern	pntr	vec_bound() ;
extern	pntr	standard_func() ;
extern	pntr	args_list() ;
extern	pntr	proc_call() ;

 
extern char *minus_sy ;
extern char *arrow_sy ;
extern char *bar_sy ;
extern char *eq_sy ;
extern char *colon_sy ;
extern char *dcolon_sy ;
extern char *assign_sy ;
extern char *lt_sy ;
extern char *le_sy ;
extern char *gt_sy ;
extern char *ge_sy ;
extern char *neq_sy ;
extern char *comma_sy ;
extern char *lp_sy ;
extern char *rp_sy ;
extern char *star_sy ;
extern char *semi_sy ;
extern char *lsb_sy ;
extern char *rsb_sy ;
extern char *lcb_sy ;
extern char *rcb_sy ;
extern char *plus_sy ;
extern char *dplus_sy ;
extern char *not_sy ;
extern char *squote_sy ;
extern char *dquote_sy ;
extern char *divide_sy ;
extern char *tab_sy ;

extern char *vec_at_sy ;



extern char *if_sy ;
extern char *do_sy ;
extern char *is_sy ;
extern char *or_sy ;

extern char *to_sy ;
extern char *of_sy ;
extern char *by_sy ;


extern char *let_sy ;
extern char *end_sy ;
extern char *int_sy ;
extern char *and_sy ;
extern char *for_sy ;
extern char *div_sy ;
extern char *rem_sy ;
extern char *upb_sy ;
extern char *lwb_sy ;
extern char *eof_sy ;
extern char *nil_sy ;


extern char *then_sy ;
extern char *else_sy ;
extern char *case_sy ;
extern char *isnt_sy ;
extern char *cint_sy ;
extern char *pntr_sy ;
extern char *bool_sy ;
extern char *real_sy ;
extern char *file_sy ;
extern char *peek_sy ;
extern char *read_sy ;


extern char *true_sy ;
extern char *false_sy ;
extern char *begin_sy ;
extern char *while_sy ;
extern char *write_sy ;
extern char *creal_sy ;
extern char *cpntr_sy ;
extern char *cfile_sy ;
extern char *cbool_sy ;
extern char *readi_sy ;
extern char *readr_sy ;
extern char *readb_sy ;
extern char *reads_sy ;
extern char *abort_sy ;
extern char *float_sy ;


extern char *vector_sy ;
extern char *string_sy ;
extern char *repeat_sy ;
extern char *output_sy ;
extern char *out_16_sy ;
extern char *out_32_sy ;


extern char *forward_sy ;
extern char *default_sy ;
extern char *cstring_sy ;
extern char *eformat_sy ;
extern char *fformat_sy ;
extern char *gformat_sy ;
extern char *read_16_sy ;
extern char *read_32_sy ;
extern char *nullfile_sy ;
extern char *out_byte_sy ;
extern char *external_sy ;

extern char *structure_sy ;
extern char *procedure_sy ;
extern char *read_name_sy ;
extern char *read_byte_sy ;
extern char *read_a_line_sy ;
extern char *literal_sy ;
extern char *identifier_sy ;
extern char *eot ;

extern  psint   codeword_size ;
extern  unsigned char   *codeword ;
extern  psint   cp,code_base,code_size ;
extern  psint   coding,externals ;
extern  psint   max_seg_size ;
extern  FILE    *so ;
extern  pntr	nil,INT,CINT,REAL,CREAL,BOOL,CBOOL,PNTR,CPNTR,
		STRING,CSTRING,FILET,CFILET,


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

	char emess[ ( psint ) 2000     ] ;

	if ( n > m )
	{
		( void ) sprintf( emess,"instruction operand too big\n%d %d\n",n,m ) ;
		err_mess( emess ) ;




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

		n = ( codeword[ a + 1 ] << 8 ) + codeword[ a ] ;

		spot = link ;
		link = n ;
		n = cp - spot - ( psint ) 2 ;
		check( n,( ( psint ) 1 << 16 ) ) ;

		codeword[ a ] = n & ( psint ) 255 ;
		codeword[ a + 1 ] = n >> 8 ;

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


	bout( n ) ;
	bout( n >> 8 ) ;
	bout( n >> 16 ) ;
	bout( n >> 24 ) ;


}

psint out_list( t )
pntr t ;
{
	psint	size = ( psint ) 0 ;

	if ( coding )
	{
		while ( t->skind != ( psint ) -1 )
		{

			bout( t->class.struct_table.s_tdm ) ;
			bout( t->class.struct_table.s_tdm >> 8 ) ;
			bout( t->class.struct_table.s_ptrs ) ;
			bout( t->class.struct_table.s_ptrs >> 8 ) ;

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

	add_byte( n & ( psint ) 255 ) ;
	add_byte( n >> 8 ) ;

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
	check( n,( ( psint ) 1 << 15 ) - ( psint ) 1 ) ;
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
	if ( (  CINT == t  )  || (  INT == t  ) ||

	     (  BOOL == t  )|| (  CBOOL == t  ) ) return( ( psint ) 0 ) ; else
	if ( (  CREAL == t  ) || (  REAL == t  ) ) return( ( psint ) 128 ) ; else
	if ( (  CSTRING == t  ) || (  STRING == t  ) ) return( ( psint ) 1 ) ; else
	if ( (  VOID == t  ) ) return( ( psint ) 130 ) ;
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
	if ( (  t == CSTRING  ) || (  t == STRING  ) ||
	     (  t == CPNTR  )   || (  t == PNTR  )   ||
	     (  t == CFILET  )  || (  t == FILET  )  ||


	     eq( vector_type,t ) ) return( ( psint ) 1 ) ; else return( ( psint ) 0 ) ;
}

psint type_size( t )
pntr	t ;
{
	if ( t->skind == ( psint ) 2 || eq2( REAL,t ) ) return( st_size2 ); else return( st_size ) ;
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
	if ( !(  t == VOID  ) )
	{
		if ( pointer( t ) ) dec_ps( type_size( t ) ) ;
		else dec_ms( type_size( t ) ) ;
	}
}

inc_stack( t )
register pntr	t ;
{
	if ( !(  t == VOID  ) )
	{
		if ( pointer( t ) ) inc_ps( type_size( t ) ) ;
		else inc_ms( type_size( t ) ) ;
	}
}

psint get_opcode( s )
register char	*s ;
{
	if ( *s == '=' ) return( ( psint ) 96 ) ; else				
	if ( *s == '~' ) return( ( psint ) 99 ) ; else				
	if ( *s == '<' ) return( s[1] == '\0' ? ( psint ) 102 : ( psint ) 104 ) ; else	
	return( s[1] == '\0' ? ( psint ) 106 : ( psint ) 108 );				
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


	add16( n & ( ( ( psint ) 1 << 16 ) - ( psint ) 1 ) ) ;
	add16( n >> 16 ) ;


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
	if ( (  CINT == t  )  || (  INT == t  ) ||

	     (  BOOL == t  )|| (  CBOOL == t  ) ) return( ( psint ) 0 ) ; else
	if ( (  CREAL == t  ) || (  REAL == t  ) ) return( ( psint ) 1 ) ; else
	if ( (  CSTRING == t  ) || (  STRING == t  ) ) return( ( psint ) 2 ) ; else
	if ( (  VOID == t  ) ) return( ( psint ) 5 ) ; else
	return( ( psint ) 3 ) ;
}

retract( ms,ps,t )
psint	ms,ps ;
pntr	t ;
{
	psint	ms1,ps1 ;

	ms1 = ms ; ps1 = ps ;
	if ( pointer( t ) ) ps1 += st_size ;
	else if ( !(  VOID == t  ) ) ms1 += type_size( t ) ;

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

	bout( ( ( psint ) 1 << 15 ) ) ;
	bout( ( ( psint ) 1 << 15 ) >> 8 ) ;

	code_size = ( psint ) 2 ;
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


	gen16( k ) ;
	gen16( ( psint ) 0 ) ;



	for ( i = ( psint ) 0 ; i < k - ( psint ) 1 ; i += ( psint ) 2 )

	gen16( ( psint ) s[ i + 1 ] * ( psint ) 256 + ( psint ) s[ i  ] ) ;

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
	
		while ( ex_decl_list->skind != ( psint ) -1 )
		{
			ex_decl_list->class.EXTERNAL.e_addr += code_size ;
			ex_decl_list = ( pntr ) ex_decl_list->class.EXTERNAL.e_next ;
		}
	
		if ( start->skind != ( psint ) -1 ) external_list = mk_cons( start,external_list ) ;
	
		while ( str_decl_list->skind != ( psint ) -1 )
		{
			i = str_decl_list->class.s_decl.s_int ;
			j = base + i - ( psint ) 4 ;
			k = -( code_size + i ) ;


			codeword[ j ] = k ;
			codeword[ j + 1 ] = k >> 8 ;
			codeword[ j + 2 ] = k >> 16 ;
			codeword[ j + 3 ] = k >> 24 ;


			str_decl_list = ( pntr ) str_decl_list->class.s_decl.s_pntr ;
		}
	
		while ( proc_decl_list->skind != ( psint ) -1 )
		{
			i = proc_decl_list->class.p_decl.p_int ;
			j = base + i ;


			offset = codeword[ j ] ;
			offset |= codeword[ j + 1 ] << 8 ;
			offset |= codeword[ j + 2 ] << 16 ;
			offset |= codeword[ j + 3 ] << 24 ;
			new_offset = offset - ( code_size + i ) - ( psint ) 4 ;
			codeword[ j ] = new_offset ;
			codeword[ j + 1 ] = new_offset >> 8 ;
			codeword[ j + 2 ] = new_offset >> 16 ;
			codeword[ j + 3 ] = new_offset >> 24 ;


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


		gen16( k ) ;
		gen16( ( ( psint ) 1 << 3 ) | ( ( psint ) 1 << 12 ) ) ;


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
	ans = ( psint * ) ( &n ) ;   
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
		params = ( t->skind != ( psint ) -1 ) ;
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
	} else return( ( ( pntr ) ( ( pntr )p->class.link.type1 )->class.proc.args )->skind != ( psint ) -1 ) ;
}

load( add1,add2,r_value,t )
psint	add1,add2,r_value ;
pntr	t ;
{
	psint	ty ;

	if ( r_value ) inc_stack( t ) ; else dec_stack( t ) ;

	if ( pointer( t ) ) ty = ( psint ) 1 ; else
	if ( t->skind == ( psint ) 2 ) ty = ( psint ) 2 ; else
	if ( eq( REAL,t ) ) ty = ( psint ) 2 ; else ty = ( psint ) 0 ;

	ty += ( r_value ? ( psint ) 0 : ( psint ) 16 ) ;

	if ( add1 == lex_level ) try_short( ( psint ) 12 + ty,add2 ) ; else
	if ( add1 == ( psint ) 0 ) try_short( ( psint ) 16 + ty,add2 ) ; else
	gen_load( ( psint ) 24 + ty,lex_level - add1,add2 ) ;
	
}



newline()
{
	if ( newlin && coding && newlins )
	{
		try_short( ( psint ) 124,line_count + ( psint ) 1 ) ;
		newlin = ( psint ) 0 ;
	}
}
