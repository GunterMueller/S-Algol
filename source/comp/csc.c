/*************************************************************************/
/*                                                                       */
/*                   S-algol compiler ( C version ) 1.0                  */
/*                              1-MAR-87                                 */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include "defines.h"
#include "stype.h"
#include "externs.h"
#include "functions.h"
#include "mktype.h"

pntr    nil,INT,CINT,REAL,CREAL,BOOL,CBOOL,PNTR,CPNTR,
        STRING,CSTRING,FILET,CFILET,
#ifdef  RASTER
        PIXEL,CPIXEL,IMAGE,CIMAGE,image_type,
#endif  RASTER
#ifdef  OUTLINE
        PIC,CPIC,
#endif  OUTLINE
        VOID,ANY,vector_type;

pntr    proc_decl_list,str_decl_list,ex_decl_list,external_list,
        procedure_type ;

char *nullstring = "",procedure_name[ MAXSIZE ] ;
extern	char	*malloc() ;

/*************** Compiler Symbols ***************/

char *minus_sy =	"-" ;
char *arrow_sy =	"->" ;
char *bar_sy =		"|" ;
char *eq_sy =		"=" ;
char *colon_sy =	":" ;
char *dcolon_sy =	"::" ;
char *assign_sy =	":=" ;
char *lt_sy =		"<" ;
char *le_sy =		"<=" ;
char *gt_sy =		">" ;
char *ge_sy =		">=" ;
char *neq_sy =		"~=" ;
char *comma_sy =	"," ;
char *lp_sy =		"(" ;
char *rp_sy =		")" ;
char *star_sy =		"*" ;
char *semi_sy =		";" ;
char *lsb_sy =		"[" ;
char *rsb_sy =		"]" ;
char *lcb_sy =		"{" ;
char *rcb_sy =		"}" ;
char *plus_sy =		"+" ;
char *dplus_sy =	"++" ;
char *not_sy =		"~" ;
char *squote_sy =	"''" ;
char *dquote_sy =	"\"" ;
char *divide_sy =	"/" ;
char *tab_sy =		"\t" ;
#ifdef	RASTER
char *hash_sy =		"#" ;
char *hat_sy =		"^" ;
char *amper_sy =	"&" ;
#endif	RASTER
char *vec_at_sy =	"@" ;

/****************** Reserved Words ******************/

char *if_sy =		"if" ;
char *do_sy =		"do" ;
char *is_sy =		"is" ;
char *or_sy =		"or" ;
#ifdef	RASTER
char *at_sy =		"at" ;
#endif	RASTER
char *to_sy =		"to" ;
char *of_sy =		"of" ;
char *by_sy =		"by" ;
#ifdef	OUTLINE
char *in_sy =		"in" ;
#endif	OUTLINE
#ifdef	RASTER
char *on_sy =		"on" ;
#endif	RASTER
char *let_sy =		"let" ;
char *end_sy =		"end" ;
char *int_sy =		"int" ;
char *and_sy =		"and" ;
char *for_sy =		"for" ;
char *div_sy =		"div" ;
char *rem_sy =		"rem" ;
char *upb_sy =		"upb" ;
char *lwb_sy =		"lwb" ;
char *eof_sy =		"eof" ;
char *nil_sy =		"nil" ;
#ifdef	OUTLINE
char *pic_sy =		"pic" ;
#endif	OUTLINE
#ifdef	RASTER
char *ror_sy =		"ror" ;
char *xor_sy =		"xor" ;
char *nor_sy =		"nor" ;
char *rnot_sy =		"not" ;
char *off_sy =		"off" ;
#endif	RASTER
char *then_sy =		"then" ;
char *else_sy =		"else" ;
char *case_sy =		"case" ;
char *isnt_sy =		"isnt" ;
char *cint_sy =		"cint" ;
char *pntr_sy =		"pntr" ;
char *bool_sy =		"bool" ;
char *real_sy =		"real" ;
char *file_sy =		"file" ;
char *peek_sy =		"peek" ;
char *read_sy =		"read" ;
#ifdef	OUTLINE
char *cpic_sy =		"cpic" ;
char *from_sy =		"from" ;
char *text_sy =		"text" ;
#endif	OUTLINE
#ifdef	RASTER
char *xnor_sy =		"xnor" ;
char *onto_sy =		"onto" ;
char *rand_sy =		"rand" ;
char *nand_sy =		"nand" ;
char *copy_sy =		"copy" ;
#endif	RASTER
char *true_sy =		"true" ;
char *false_sy =	"false" ;
char *begin_sy =	"begin" ;
char *while_sy =	"while" ;
char *write_sy =	"write" ;
char *creal_sy =	"creal" ;
char *cpntr_sy =	"cpntr" ;
char *cfile_sy =	"cfile" ;
char *cbool_sy =	"cbool" ;
char *readi_sy =	"readi" ;
char *readr_sy =	"readr" ;
char *readb_sy =	"readb" ;
char *reads_sy =	"reads" ;
char *abort_sy =	"abort" ;
char *float_sy =	"float" ;
#ifdef	OUTLINE
char *shift_sy =	"shift" ;
char *scale_sy =	"scale" ;
#endif	OUTLINE
#ifdef	RASTER
char *pixel_sy =	"pixel" ;
char *limit_sy =	"limit" ;
char *image_sy =	"image" ;
#endif	RASTER
char *vector_sy =	"vector" ;
char *string_sy =	"string" ;
char *repeat_sy =	"repeat" ;
char *output_sy =	"output" ;
char *out_16_sy =	"out.16" ;
char *out_32_sy =	"out.32" ;
#ifdef	OUTLINE
char *colour_sy =	"colour" ;
char *rotate_sy =	"rotate" ;
#endif	OUTLINE
#ifdef	RASTER
char *cpixel_sy =	"cpixel" ;
#endif	RASTER
char *forward_sy =	"forward" ;
char *default_sy =	"default" ;
char *cstring_sy =	"cstring" ;
char *eformat_sy =	"eformat" ;
char *fformat_sy =	"fformat" ;
char *gformat_sy =	"gformat" ;
char *read_16_sy =	"read.16" ;
char *read_32_sy =	"read.32" ;
char *nullfile_sy =	"nullfile" ;
char *out_byte_sy =	"out.byte" ;
char *external_sy =	"external" ;
#ifdef	OUTLINE
char *pic_pntr_sy =	"pic.pntr" ;
#endif	OUTLINE
char *structure_sy =	"structure" ;
char *procedure_sy =	"procedure" ;
char *read_name_sy =	"read.name" ;
char *read_byte_sy =	"read.byte" ;
char *read_a_line_sy =	"read.a.line" ;

char *literal_sy =	"literal" ;
char *identifier_sy =	"identifier" ;
char *eot =		"?" ;

#ifdef	RASTER
#define	COLUMN	( psint ) 24
#define	ROW	( psint ) 17
#else
#define	COLUMN	( psint ) 23
#define	ROW	( psint ) 14
#endif	RASTER

char	*rwords[ COLUMN ][ ROW ] ;

init_rwords()
{
	register psint i,j ;

	for ( i = ( psint ) 0; i < COLUMN ; i++ )
	{
		for ( j = ( psint ) 0; j < ROW ; j++ )
			rwords[ i ][ j ] = nullstring ;
	}

#ifdef	RASTER
	rwords[ 0 ][ 0 ] = at_sy ;
	rwords[ 0 ][ 1 ] = and_sy ;
	rwords[ 0 ][ 2 ] = abort_sy ;
#else
	rwords[ 0 ][ 0 ] = and_sy ;
	rwords[ 0 ][ 1 ] = abort_sy ;
#endif	RASTER

	rwords[ 1 ][ 0 ] = begin_sy ;
	rwords[ 1 ][ 1 ] = bool_sy ;
	rwords[ 1 ][ 2 ] = by_sy ;

#ifndef	RASTER
#ifdef	OUTLINE
	rwords[ 2 ][ 0 ] = case_sy ;
	rwords[ 2 ][ 1 ] = cstring_sy ;
	rwords[ 2 ][ 2 ] = cint_sy ;
	rwords[ 2 ][ 3 ] = creal_sy ;
	rwords[ 2 ][ 4 ] = cfile_sy ;
	rwords[ 2 ][ 5 ] = cpntr_sy ;
	rwords[ 2 ][ 6 ] = cbool_sy ;
	rwords[ 2 ][ 7 ] = cpic_sy ;
#else
	rwords[ 2 ][ 0 ] = case_sy ;
	rwords[ 2 ][ 1 ] = cstring_sy ;
	rwords[ 2 ][ 2 ] = cint_sy ;
	rwords[ 2 ][ 3 ] = creal_sy ;
	rwords[ 2 ][ 4 ] = cfile_sy ;
	rwords[ 2 ][ 5 ] = cpntr_sy ;
	rwords[ 2 ][ 6 ] = cbool_sy ;
#endif	OUTLINE
#else
	rwords[ 2 ][ 0 ] = case_sy ;
	rwords[ 2 ][ 1 ] = cstring_sy ;
	rwords[ 2 ][ 2 ] = cint_sy ;
	rwords[ 2 ][ 3 ] = creal_sy ;
	rwords[ 2 ][ 4 ] = cfile_sy ;
	rwords[ 2 ][ 5 ] = cpntr_sy ;
	rwords[ 2 ][ 6 ] = cbool_sy ;
	rwords[ 2 ][ 7 ] = cpic_sy ;
	rwords[ 2 ][ 8 ] = colour_sy ;
	rwords[ 2 ][ 9 ] = cpixel_sy ;
	rwords[ 2 ][ 10 ] = copy_sy ;
#endif	RASTER

	rwords[ 3 ][ 0 ] = do_sy ;
	rwords[ 3 ][ 1 ] = default_sy ;
	rwords[ 3 ][ 2 ] = div_sy ;

	rwords[ 4 ][ 0 ] = end_sy ;
	rwords[ 4 ][ 1 ] = else_sy ;
	rwords[ 4 ][ 2 ] = eformat_sy ;
	rwords[ 4 ][ 3 ] = eof_sy ;
	rwords[ 4 ][ 4 ] = external_sy ;

#ifdef	OUTLINE
	rwords[ 5 ][ 0 ] = file_sy ;
	rwords[ 5 ][ 1 ] = for_sy ;
	rwords[ 5 ][ 2 ] = float_sy ;
	rwords[ 5 ][ 3 ] = fformat_sy ;
	rwords[ 5 ][ 4 ] = forward_sy ;
	rwords[ 5 ][ 5 ] = from_sy ;
	rwords[ 5 ][ 6 ] = false_sy ;
#else
	rwords[ 5 ][ 0 ] = file_sy ;
	rwords[ 5 ][ 1 ] = for_sy ;
	rwords[ 5 ][ 2 ] = float_sy ;
	rwords[ 5 ][ 3 ] = fformat_sy ;
	rwords[ 5 ][ 4 ] = forward_sy ;
	rwords[ 5 ][ 5 ] = false_sy ;
#endif	OUTLINE

	rwords[ 6 ][ 0 ] = gformat_sy ;

#ifndef	RASTER
#ifdef	OUTLINE
	rwords[ 8 ][ 0 ] = if_sy ;
	rwords[ 8 ][ 1 ] = int_sy ;
	rwords[ 8 ][ 2 ] = is_sy ;
	rwords[ 8 ][ 3 ] = isnt_sy ;
	rwords[ 8 ][ 4 ] = in_sy ;
#else
	rwords[ 8 ][ 0 ] = if_sy ;
	rwords[ 8 ][ 1 ] = int_sy ;
	rwords[ 8 ][ 2 ] = is_sy ;
	rwords[ 8 ][ 3 ] = isnt_sy ;
#endif	OUTLINE
#else
	rwords[ 8 ][ 0 ] = if_sy ;
	rwords[ 8 ][ 1 ] = int_sy ;
	rwords[ 8 ][ 2 ] = is_sy ;
	rwords[ 8 ][ 3 ] = isnt_sy ;
	rwords[ 8 ][ 4 ] = in_sy ;
	rwords[ 8 ][ 5 ] = image_sy ;
#endif	RASTER

#ifdef	RASTER
	rwords[ 11 ][ 0 ] = let_sy ;
	rwords[ 11 ][ 1 ] = lwb_sy ;
	rwords[ 11 ][ 2 ] = limit_sy ;

	rwords[ 13 ][ 0 ] = nand_sy ;
	rwords[ 13 ][ 1 ] = nor_sy ;
	rwords[ 13 ][ 2 ] = rnot_sy ;
        rwords[ 13 ][ 3 ] = nil_sy ;
        rwords[ 13 ][ 4 ] = nullfile_sy ;

	rwords[ 14 ][ 0 ] = of_sy ;
	rwords[ 14 ][ 1 ] = or_sy ;
	rwords[ 14 ][ 2 ] = output_sy ;
	rwords[ 14 ][ 3 ] = out_byte_sy ;
	rwords[ 14 ][ 4 ] = out_16_sy ;
	rwords[ 14 ][ 5 ] = out_32_sy ;
	rwords[ 14 ][ 6 ] = onto_sy ;
        rwords[ 14 ][ 7 ] = on_sy ;
        rwords[ 14 ][ 8 ] = off_sy ;
#else

	rwords[ 11 ][ 0 ] = let_sy ;
	rwords[ 11 ][ 1 ] = lwb_sy ;

        rwords[ 13 ][ 0 ] = nil_sy ;
        rwords[ 13 ][ 1 ] = nullfile_sy ;

	rwords[ 14 ][ 0 ] = of_sy ;
	rwords[ 14 ][ 1 ] = or_sy ;
	rwords[ 14 ][ 2 ] = output_sy ;
	rwords[ 14 ][ 3 ] = out_byte_sy ;
	rwords[ 14 ][ 4 ] = out_16_sy ;
	rwords[ 14 ][ 5 ] = out_32_sy ;
#endif	RASTER

#ifndef	RASTER
#ifdef	OUTLINE
	rwords[ 15 ][ 0 ] = procedure_sy ;
	rwords[ 15 ][ 1 ] = pntr_sy ;
	rwords[ 15 ][ 2 ] = peek_sy ;
	rwords[ 15 ][ 3 ] = pic_sy ;
	rwords[ 15 ][ 4 ] = pic_pntr_sy ;

	rwords[ 17 ][ 0 ] = real_sy ;
	rwords[ 17 ][ 1 ] = repeat_sy ;
	rwords[ 17 ][ 2 ] = rem_sy ;
	rwords[ 17 ][ 3 ] = read_sy ;
	rwords[ 17 ][ 4 ] = readi_sy ;
	rwords[ 17 ][ 5 ] = read_a_line_sy ;
	rwords[ 17 ][ 6 ] = reads_sy ;
	rwords[ 17 ][ 7 ] = readr_sy ;
	rwords[ 17 ][ 8 ] = readb_sy ;
	rwords[ 17 ][ 9 ] = read_name_sy ;
	rwords[ 17 ][ 10 ] = read_byte_sy ;
	rwords[ 17 ][ 11 ] = read_16_sy ;
	rwords[ 17 ][ 12 ] = read_32_sy ;
	rwords[ 17 ][ 13 ] = rotate_sy ;

	rwords[ 18 ][ 0 ] = string_sy ;
	rwords[ 18 ][ 1 ] = structure_sy ;
	rwords[ 18 ][ 2 ] = shift_sy ;
	rwords[ 18 ][ 3 ] = scale_sy ;

	rwords[ 19 ][ 0 ] = to_sy ;
	rwords[ 19 ][ 1 ] = then_sy ;
	rwords[ 19 ][ 2 ] = text_sy ;
        rwords[ 19 ][ 3 ] = true_sy ;
#else

	rwords[ 15 ][ 0 ] = procedure_sy ;
	rwords[ 15 ][ 1 ] = pntr_sy ;
	rwords[ 15 ][ 2 ] = peek_sy ;

	rwords[ 17 ][ 0 ] = real_sy ;
	rwords[ 17 ][ 1 ] = repeat_sy ;
	rwords[ 17 ][ 2 ] = rem_sy ;
	rwords[ 17 ][ 3 ] = read_sy ;
	rwords[ 17 ][ 4 ] = readi_sy ;
	rwords[ 17 ][ 5 ] = read_a_line_sy ;
	rwords[ 17 ][ 6 ] = reads_sy ;
	rwords[ 17 ][ 7 ] = readr_sy ;
	rwords[ 17 ][ 8 ] = readb_sy ;
	rwords[ 17 ][ 9 ] = read_name_sy ;
	rwords[ 17 ][ 10 ] = read_byte_sy ;
	rwords[ 17 ][ 11 ] = read_16_sy ;
	rwords[ 17 ][ 12 ] = read_32_sy ;

	rwords[ 18 ][ 0 ] = string_sy ;
	rwords[ 18 ][ 1 ] = structure_sy ;

	rwords[ 19 ][ 0 ] = to_sy ;
	rwords[ 19 ][ 1 ] = then_sy ;
        rwords[ 19 ][ 2 ] = true_sy ;
#endif	OUTLINE
#else
	rwords[ 15 ][ 0 ] = procedure_sy ;
	rwords[ 15 ][ 1 ] = pntr_sy ;
	rwords[ 15 ][ 2 ] = peek_sy ;
	rwords[ 15 ][ 3 ] = pic_sy ;
	rwords[ 15 ][ 4 ] = pic_pntr_sy ;
	rwords[ 15 ][ 5 ] = pixel_sy ;

	rwords[ 17 ][ 0 ] = real_sy ;
	rwords[ 17 ][ 1 ] = repeat_sy ;
	rwords[ 17 ][ 2 ] = rem_sy ;
	rwords[ 17 ][ 3 ] = read_sy ;
	rwords[ 17 ][ 4 ] = readi_sy ;
	rwords[ 17 ][ 5 ] = read_a_line_sy ;
	rwords[ 17 ][ 6 ] = reads_sy ;
	rwords[ 17 ][ 7 ] = readr_sy ;
	rwords[ 17 ][ 8 ] = readb_sy ;
	rwords[ 17 ][ 9 ] = read_name_sy ;
	rwords[ 17 ][ 10 ] = read_byte_sy ;
	rwords[ 17 ][ 11 ] = read_16_sy ;
	rwords[ 17 ][ 12 ] = read_32_sy ;
	rwords[ 17 ][ 13 ] = rotate_sy ;
	rwords[ 17 ][ 14 ] = ror_sy ;
	rwords[ 17 ][ 15 ] = rand_sy ;

	rwords[ 18 ][ 0 ] = string_sy ;
	rwords[ 18 ][ 1 ] = structure_sy ;
	rwords[ 18 ][ 2 ] = shift_sy ;
	rwords[ 18 ][ 3 ] = scale_sy ;

	rwords[ 19 ][ 0 ] = to_sy ;
	rwords[ 19 ][ 1 ] = then_sy ;
	rwords[ 19 ][ 2 ] = text_sy ;
        rwords[ 19 ][ 3 ] = true_sy ;
#endif	RASTER

	rwords[ 20 ][ 0 ] = upb_sy ;

	rwords[ 21 ][ 0 ] = vector_sy ;

	rwords[ 22 ][ 0 ] = while_sy ;
	rwords[ 22 ][ 1 ] = write_sy ;

#ifdef	RASTER
	rwords[ 23 ][ 0 ] = xor_sy ;
	rwords[ 23 ][ 1 ] = xnor_sy ;
#endif	RASTER
}

/*************************** Options ****************************/

psint	flsum = FALSE ;
psint	ext = FALSE ;
char	proc_name[ MAXSIZE ] ;
psint	listing = FALSE ;
psint	newlins = TRUE ;

set_options( argc,argv )
int	argc ;
char	*argv[] ;
{
	register psint	i ;

	for ( i = ( psint ) 0 ; i < argc ; i++ )
	if ( strcmp( "ext",argv[ i ] ) == 0 ) ext = TRUE ; else
	if ( strcmp( "flsum",argv[ i ] ) == 0 ) flsum = TRUE ; else
	if ( strcmp( "list",argv[ i ] ) == 0 ) listing = TRUE ; else
	if ( strcmp( "nonewline",argv[ i ] ) == 0 ) newlins = FALSE ;
}

FILE	*si,*so,*SI ;
FILE	*fl_sum ;
psint	st_size = Stack_element_size, st_size2 = ( psint ) 2 * Stack_element_size ;
psint	st_id_size  ;

/******** CODE GENERATION GLOBALS *******/

psint	codeword_size = Code_v_size ;
unsigned char	*codeword ;
psint	cp = ( psint ) 4, code_base = ( psint ) 0, code_size = ( psint ) 0 ;
psint	coding = FALSE ;

/******************** ERROR ROUTINES **********************/

psint	error = FALSE ;
psint	no_of_errors = ( psint ) 0 ;
psint	errors_on_line = ( psint ) 0 ;
char	lf = 10 ;
psint	err_pos = ( psint ) 10 ;
char	space = ' ' ;
char	nl = '\n' ;
char	*error_line ;
char	errors[5][OUTPUT_LENGTH] ;
FILE	*s_o = stdout ;

err_mess( s )
char	s[] ;
{
	error = TRUE ;
	coding = FALSE ;
	cp = ( psint ) 0 ;
	if ( errors_on_line < ( psint ) 5 ) ( void ) ( void ) strcpy( errors[ errors_on_line ],s ) ;
	errors_on_line++ ;
	error_line[ err_pos - 1 ] = '^' ;
	no_of_errors++ ;
}

/************************** Code Generation ******************************/

psint	max_seg_size = twoto17 ;
psint	externals = ( psint ) 0 ;

/* note the following have been expanded for initialisation in C */

psint	st_size3 = ( psint ) 3 * Stack_element_size ;
psint	st_size4 = ( psint ) 4 * Stack_element_size ;
psint	mscw_size = ( psint ) 6 * Stack_element_size ;
psint	lex_level = ( psint ) 0, newlab = ( psint ) 0 ;
psint	ssp = ( psint ) 6 * Stack_element_size ;
psint	psp = ( psint ) 0 ;
psint	max_ms = ( psint ) 6 * Stack_element_size ;
psint	max_ps = ( psint ) 0 ;

psint	last_segment = ( psint ) 0 ;
psint	last_retract = ( psint ) ( psint ) -1 ;

psint	newlin = TRUE ;
psint	line_count = ( psint ) 0 ;

/******************* LEXICAL ANALYSIS *********************/

char	peep ;
psint	at_new_line ;
char	*symb,the_name[MAXSIZE] ;
pntr	the_string ;
psint	the_string_length = MAXSIZE ;
psint	lines_per_page = ( psint ) 44 ;
psint	lines_on_page = ( psint ) 44 + ( psint ) 1 ;
psint	block_count = ( psint ) 0 ;
psint	last_block_count = ( psint ) 0 ;
psint	option = TRUE ;
psint	line_pos = ( psint ) 0 ;
psint	page_count = ( psint ) 0 ;
char	*line_image ;
psint	output_length = OUTPUT_LENGTH ;
pntr	lit_type ;
char	heading[MAXSIZE] = "S-algol System" ;

init_lines()
{
	line_image = malloc( ( unsigned ) output_length ) ;
	error_line = malloc( ( unsigned ) output_length ) ;
}

extend_lines()
{
        register psint  i,new_output_length ;
        register char   *new_error_line,*new_line_image ;
        
        new_output_length = output_length + 32 ;
        new_error_line = malloc( ( unsigned ) new_output_length ) ;
        new_line_image = malloc( ( unsigned ) new_output_length ) ;
        i = output_length ;
        while( i-- >= 0 )
        {
                new_error_line[ i ] = error_line[ i ] ;
                new_line_image[ i ] = line_image[ i ] ;
        }
        free( error_line ) ;
        free( line_image ) ;
        output_length = new_output_length ;
        error_line = new_error_line ;
        line_image = new_line_image ;
}

Shutdown()
{
	if ( error )
	( void ) ( void ) printf( "\n Compilation Fails\nNumber of Error Messages = %d\n",no_of_errors ) ;
	else
	if( listing ) ( void ) ( void ) printf( "\n**** Program Compiles ****\n" ) ;
}

title()
{
	psint	i ;

	page_count++ ;
	if ( listing )
	{
		if ( page_count != ( psint ) 1 )
		for( i = lines_on_page ; i <= lines_per_page + ( psint ) 3 ; i++ ) ( void ) ( void ) printf( "\n" ) ;
		( void ) printf( "\n%30s    page %4d\n\n",heading,page_count ) ;
	}
	lines_on_page = ( psint ) 3 ;
}

psint	zero_sy = '0' ;
psint	nine_sy = '9' ;
psint	one_sy = '1' ;
psint	tabs = FALSE ;

out()
{
	if ( lines_on_page > lines_per_page ) title() ;
	lines_on_page++ ;
}

write_line( F )
FILE	*F ;
{
	char	c1 = '-' ;
	char	c2 = '-' ;

	if ( block_count > last_block_count ) c1 = block_count + zero_sy ;
	if ( block_count < last_block_count ) c2 = block_count + one_sy ;
	( void ) fprintf( F,"%5d %c%c  ",line_count,c1,c2 ) ;
	/* for ( i = 0; i < line_pos; i++ )
	   ( void ) fprintf( F,"%c",line_image[i] ) ;
	   a little optimisation because we're using C */
	line_image[ line_pos ] = NUL ;
	( void ) fprintf( F,"%s",line_image ) ;
}

clear_line( l,n,c )
char	*l ;
psint	n ;
char	c ;
{
	register psint	i ;
	for ( i=( psint ) 0; i < n; l[i++] = c ) ;
	l[n-( psint ) 1] = '\0' ;
}

output_line()
{
	psint	i ;

	line_count++ ;
	if ( flsum )
	{
		if ( line_count == ( psint ) 4093 ) err_mess( "Too many lines for flow summary\n" ) ;
		write_line( fl_sum ) ;
	}
	if ( !option )
	{
		newlin = TRUE ;
		if  ( listing || errors_on_line != ( psint ) 0 )
		{
			out() ;
			write_line( stdout ) ;
			last_block_count = block_count ;
		}
		if ( errors_on_line != ( psint ) 0 )
		{
			out() ;
			for ( i = ( psint ) 0; i < err_pos; i++ ) ( void ) printf( "%c",error_line[i] ) ;
			( void ) printf( "\n" ) ;
			for ( i = ( psint ) 0; i < err_pos; i++ ) error_line[i] = ' ' ;
			if ( errors_on_line > ( psint ) 3 ) errors_on_line = ( psint ) 3 ;
			if ( no_of_errors < ( psint ) 30 )
			for ( i = ( psint ) 0; i < errors_on_line; i++ )
			{
				out() ;
				( void ) printf( "%s",errors[i] );  /* optimisation for */
				/* for ( j = 0; j < strlen( errors[i] ); j++ ) ( void ) printf( "%c",errors[i][j] ); */
			}
			for ( i = ( psint ) 0; i < ( psint ) 5; i++ ) clear_line( errors[i],OUTPUT_LENGTH,NUL ) ;
			errors_on_line = ( psint ) 0 ;
		}
		else
		if ( tabs )
		{
			clear_line( error_line,err_pos,' ' ) ;
			tabs = FALSE ;
		}
	}
	line_pos = ( psint ) 0 ;
	err_pos = ( psint ) 10 ;
}

inc_pos( n )
psint	n ;
{
	err_pos = err_pos + n ;
/*
	if ( err_pos > output_length + ( psint ) 1 ) err_pos = output_length + ( psint ) 1 ;
	line_pos++ ;
	if ( line_pos >= output_length )
	{
		line_image[ output_length ] = nl ;
		line_pos++ ;
		output_line() ;
	} 
*/
        while ( err_pos > OUTPUT_LENGTH ) extend_lines() ;
        line_pos++ ;
        while ( line_pos >= OUTPUT_LENGTH ) extend_lines() ;
}

char	read_ch()
{
	/* c:= read( s.i ) */
	register int	c ;

	if ( ( c = getc( si ) ) == EOF ) c = ( int ) '?' ;
	return( ( char ) c ) ;
}

char	next_ch()
{
	register char	c ;

	c = peep ;
	line_image[ line_pos ] = c ;
	if ( c == '\t' )
	{
		error_line[ err_pos ] = '\t' ;
		tabs = TRUE ;
	}
	inc_pos( ( psint ) 1 ) ;
	if ( c == '\n' ) output_line() ;
	peep = read_ch() ;
	return( c ) ;
}

char	read_char()
{
	register char	c ;

	c = next_ch() ;
	if ( c == '\'' )
	{
		switch ( peep ) {
		case 'n'	: c = next_ch() ;
		                  c = NL ;
		                  break ;
		case 'p'	: c = next_ch() ;
		                  c = FF ;
		                  break ;
		case 'o'	: c = next_ch() ;
		                  c = CR;
		                  break ;
		case 'b'	: c = next_ch() ;
		                  c = BS ;
		                  break ;
		case 't'	: c = next_ch() ;
		                  c = TAB ;
		                  break ;
		case '"'	: c = next_ch() ;
		                  c = DQUOTE ;
		                  break ;
		case '\''	: c = next_ch() ;
		                  break ;
		default		: break ;
		}
	}
	return( c ) ;
}
	
try( symbol )
char	symbol[] ;
{
	if ( symbol[ 1 ] == peep )
	{
		( void ) next_ch() ;
		symb = symbol ;
	}
}

read_string()     /* read a string terminated by " into the_string */
{
	register psint	i = ( psint ) 0 ;
	pntr	t,start ;
	char	c ;

	t = mk_string( ( psint ) 0,nil ) ; start = t ;
	while ( peep != '"' && !feof( si ) )
	{
		( pntr ) t->class.str.str_pntr = mk_string( read_char(),nil ) ;
		t = ( pntr ) t->class.str.str_pntr ; i++ ;
	}
	c = next_ch() ;      /* discard closing quote */
	if ( c == '?' ) err_mess( "End of File\n" ) ;
	start->class.str.str_int = i ;
	the_string = start ;
}

psint reserved_word( s )
char	s[] ;
{
	register psint	index,i,found ;

	index = s[ 0 ] - 'a' ;
	if ( index >= ( psint ) 0 && index < COLUMN )
	{
		i = ( psint ) 0 ;
		found = FALSE ;
		while ( i < ROW && ( rwords[ index ][ i ] != nullstring ) && !( found = streq( rwords[ index ][ i ],s ) ) ) i++ ;
		if ( !found ) return FALSE; else
		{
			symb = rwords[ index ][ i ] ;
			return TRUE ;
		}
	}
	else return FALSE ;
}

psint ok( c )
char	c ;
{
	return( ( psint ) ( isalpha( c ) || isdigit( c ) || c == '.' ) ) ;
}

psint	underline = FALSE ;

ul( s )
char	s[] ;
{
	register psint	i,l ;
	
	if ( underline )
	{
		l = strlen( s ) ;
		for ( i = ( psint ) 0; i < l; i++ )
		{
			line_image[ line_pos + i ] = '\b' ;
			line_image[ line_pos + i + l ] = '_' ;
		}
		line_pos += l * ( psint ) 2 ;
	}
}

psint	the_lit = ( psint ) 0 ;
psint	needsRaster = ( psint ) 0 ;

read_name( s )
char	s[] ;
{
	register psint	i = ( psint ) 0 ;
	s[ i++ ] = next_ch() ;
	while ( ok( peep ) )
	s[ i++ ] = next_ch() ;
	s[ i ] = NUL ;
}

try_name()
{
	char	s[MAXSIZE] ;
	
	read_name( s );		/* reads a name into string s */

	if ( reserved_word( s ) )
	{
		ul( s ) ;
		if ( Streq( symb,true_sy ) || Streq( symb,false_sy ) )
		{
			lit_type = BOOL ;
			the_lit = ( psint ) 0 ;
			if ( *s == 'f' ) the_lit = ( psint ) -1 ;
			symb = literal_sy ;
		}
		else
		if ( Streq( symb,nil_sy ) )
		{
			lit_type = PNTR ;
			symb = literal_sy ;
		}
		else
		if ( Streq( symb,on_sy ) || Streq( symb,off_sy ) )
		{
			lit_type = PIXEL ;
			the_lit = ON_VALUE ;
			if ( s[1] == 'f' ) the_lit = OFF_VALUE ;
			symb = literal_sy ;
		}
		else
		if ( Streq( symb,nullfile_sy ) )
		{
			lit_type = FILET ;
			symb = literal_sy ;
		}
		else
		( void ) strcpy( the_name,symb ) ;
	}
	else
	{
		( void ) strcpy( the_name,s ) ;
		if ( !strcmp( the_name,"screen" ) || !strcmp( the_name,"cursor" ) )
		needsRaster = ( psint ) 1 ;
		symb = identifier_sy ;
	}
}

char	*maxintstr = "2147483647" ;
char	*minintstr = "2147483648" ;
psreal	real_value = 0.0;
psint	unary_minus = FALSE ;

psint	int_conv( s,minus )
char	s[] ;
psint	minus ;
{
	register psint	n = ( psint ) 0,i,k ;

	for ( i = ( psint ) 0 ; i < strlen( s ) ; i ++ )
	{
		k = s[ i ] - zero_sy ;
		n = n * ( psint ) 10 + ( minus ? -k : k ) ;
	}
	return( n ) ;
}

int_str( s )
char	s[] ;
{
	register psint	i = ( psint ) 0 ;

	while ( isdigit( peep ) )
	{
		s[ i++ ] = peep ;
		( void ) next_ch() ;
	}
	s[ i ] = '\0' ;
}

psint le( s,s1 )
char	s[],s1[] ;
{
	psint	result ;

	if ( strlen( s ) < strlen( s1 ) ) result = TRUE ; else
	if ( strlen( s ) == strlen( s1 ) ) 
		if ( strcmp( s,s1 ) <= 0 ) result = TRUE ; else result = FALSE ;
	else result = FALSE ;

	return result ;
}

psreal	ex( the_scale )
psint	the_scale ;
{
	register psreal	r = 1.0 ;
	register psreal	fac = 10.0 ;

	while ( the_scale != ( psint ) 0 )
	{
		if ( the_scale % ( psint ) 2 ) r *= fac ;
		fac *= fac ;
		the_scale = the_scale / ( psint ) 2 ;
	}
	return( r ) ;
}

psreal	real_conv( s,the_scale )
char	s[] ;
psint	the_scale ;
{
	register psreal	n = 0.0 ;
	register psint	no = strlen( s ) - ( psint ) 1 ;
	register psint	more = TRUE ;
	register psint	i ;

	while ( no >= ( psint ) 0 && more )
	if ( s[ no ] == zero_sy ) no-- ; else more = FALSE ;
	for ( i = ( psint ) 0 ; i <= no ; i++ )
	n = n * 10.0 + ( s[ i ] - zero_sy ) ;
	the_scale = the_scale + strlen( s ) - no - ( psint ) 1 ;
	if ( the_scale != ( psint ) 0 ) n = ( the_scale < ( psint ) 0 ) ? n / ex( -the_scale ) : n * ex( the_scale ) ;
	return( n ) ;
}

number()
{
	register psint	sign = FALSE ;
	register psint	the_scale ;
	char	ipart[ MAXSIZE ],dpart[ MAXSIZE ],epart[ MAXSIZE ] ;

	symb = literal_sy ;
	lit_type = INT ;
	while ( peep == '0' ) ( void ) next_ch() ;
	int_str( ipart ) ;
	( void ) strcpy( dpart,"" ) ;
	if ( peep == '.' )
	{ ( void ) next_ch() ; lit_type = REAL ; int_str( dpart ) ; }
	( void ) strcpy( epart,"" ) ;
	if ( peep == 'e' )
	{
		( void ) next_ch() ;
		lit_type = REAL ;
		if ( peep == '-' )
		{
			( void ) next_ch() ;
			sign = TRUE ;
		}
		else
		if ( peep == '+' )
		{
			( void ) next_ch() ;
			sign = FALSE ;
		}
		int_str( epart ) ;
	}
	if ( lit_type == REAL )
	{
		the_scale = int_conv( epart,sign ) ;
		real_value = real_conv( ipart,the_scale ) ;
		if ( !streq( dpart,"" ) ) real_value = real_value + real_conv( dpart,the_scale - strlen( dpart ) ) ;
		if ( unary_minus ) real_value = - real_value ;
	}
	else
	if ( unary_minus && le( ipart,minintstr ) || !unary_minus && le( ipart,maxintstr ) )
	{
		the_lit = ( psint ) 0 ;
		if ( strlen( ipart ) > ( psint ) 0 ) the_lit = int_conv( ipart,unary_minus ) ;
	}
	else err_mess( "Integer literal out of range\n" ) ;
	unary_minus = FALSE ;
}

psint	onecharstring = sizeof( " " ) ;

char	*char_to_string( c )
char	c ;
{
	register char	*s ;

	s = malloc( ( unsigned ) onecharstring ) ;
	s[ 0 ] = c ;
	s[ 1 ] = '\0' ;
	return( s ) ;
}

layout()
{
	register psint	more ;

	at_new_line = FALSE ;
	more = TRUE ;
	while ( more ) switch( peep ){
	case ' '	:
	case '\t'	: ( void ) next_ch() ;
			  break ;
	case '%'	: directive() ;
			  at_new_line = TRUE ;
			  break ;
	case '\n'	: ( void ) next_ch() ;
			  at_new_line = TRUE ;
			  break ;
	case '!'	: while( next_ch() != '\n' && !feof( si ) ) {} ;
	                  at_new_line = TRUE ;
	                  break ;
	default		: more = FALSE ;
	}
}

psint	recovered = TRUE ;

next_sy()
{
	the_name[ 0 ] = NUL ;

	layout() ;
	if ( isalpha( peep ) ) try_name() ;
	else
	if ( isdigit( peep ) ) number() ;
	else
	switch ( peep ){
	case ':'	: ( void ) next_ch() ;
			  symb = colon_sy ;
			  if ( peep == ':' ){
			       ( void ) next_ch() ;
			       symb = dcolon_sy ;
			  }
			  else
			  	try( assign_sy ) ;
			  break ;
	case '<'	: ( void ) next_ch() ;
			  symb = lt_sy ;
			  try( le_sy ) ;
			  break ;
	case '>'	: ( void ) next_ch() ;
			  symb = gt_sy ;
			  try( ge_sy ) ;
			  break ;
	case '~'	: ( void ) next_ch() ;
			  symb = not_sy ;
			  try( neq_sy ) ;
			  break ;
	case '"'	: ( void ) next_ch() ;     /* discard the opening quote */
	                  symb = literal_sy ;
	                  lit_type = STRING ;
	                  read_string() ;
	                  break ;
	case '-'	: ( void ) next_ch() ;
			  symb = minus_sy ;
			  try( arrow_sy ) ;
			  break ;
	case '+'	: ( void ) next_ch() ;
			  symb = plus_sy ;
			  try( dplus_sy ) ;
			  break ;
	case '|'	: ( void ) next_ch() ; symb = bar_sy ; break ;
	case '='	: ( void ) next_ch() ; symb = eq_sy ; break ;
	case '('	: ( void ) next_ch() ; symb = lp_sy ; break ;
	case ')'	: ( void ) next_ch() ; symb = rp_sy ; break ;
	case '{'	: ( void ) next_ch() ; symb = lcb_sy ; break ;
	case '}'	: ( void ) next_ch() ; symb = rcb_sy ; break ;
	case '['	: ( void ) next_ch() ; symb = lsb_sy ; break ;
	case ']'	: ( void ) next_ch() ; symb = rsb_sy ; break ;
	case ','	: ( void ) next_ch() ; symb = comma_sy ; break ;
	case '*'	: ( void ) next_ch() ; symb = star_sy ; break ;
	case ';'	: ( void ) next_ch() ; symb = semi_sy ; break ;
	case '/'	: ( void ) next_ch() ; symb = divide_sy ; break ;
	case '#'	: ( void ) next_ch() ; symb = hash_sy ; break ;
	case '^'	: ( void ) next_ch() ; symb = hat_sy ; break ;
	case '&'	: ( void ) next_ch() ; symb = amper_sy ; break ;
	case '@'	: ( void ) next_ch() ; symb = vec_at_sy ; break ;
	case '?'	: ( void ) next_ch() ; symb = eot ; break ;
	default		: symb = char_to_string( next_ch() ) ;
	                  break ;
	}
}

psint have( s )
char	s[] ;
{
	if ( streq( s,symb ) ) { next_sy() ; return TRUE ; }
	else return FALSE ;
}

syntax( s )
char	s[] ;
{
	char	mess[ MAXSIZE ] ;

	if ( recovered )
	{
	( void ) strcpy( mess,"***** Syntax Error ***** " ) ;
	( void ) strcat( mess,symb ) ;
	( void ) strcat( mess," found where " ) ;
	( void ) strcat( mess,s ) ;
	( void ) strcat( mess," expected\n" ) ;
	err_mess( mess ) ;
	recovered = FALSE ;
	}
}

mustbe( s )
char	*s ;
{
	if ( streq( s,symb ) ) { recovered = TRUE; next_sy(); }
	else if ( recovered ) syntax( s ) ;
	else
	{
		while( !streq( s,symb ) && !streq( symb,eot ) ) next_sy() ;
		if ( streq( s,symb ) ) { recovered = TRUE; next_sy(); }
	}
}

separator()
{
	if( !have( semi_sy ) && !at_new_line )
	if( recovered ) syntax( semi_sy ) ;
	else
	{
		while ( !have( semi_sy ) && !at_new_line && !streq( symb,eot ) ) next_sy() ;
		if ( !streq( symb,eot ) ) recovered = TRUE ;
	}
}

/******************************* Type Matching *********************************/

char	*display( p )
pntr	p ;
{
	char	s[ 1000 ] ;

	( void ) strcpy( s,"" ) ;
	switch( p->skind ){
	case SCALAR_CLASS	: ( void ) strcpy( s,p->class.scalar.t_name ); break ;
	case VAR_CLASS		: ( void ) strcpy( s,display( ( pntr ) p->class.var.contents ) ); break ;
	case CONST_CLASS	: ( void ) strcpy( s,"c" ) ;
	                  	( void ) strcat( s,display( ( pntr ) p->class.CONST.content ) ) ;
	                  	break ;
	case VECTOR_CLASS	: ( void ) strcpy( s,"*" ) ;
	                  	( void ) strcat( s,display( ( pntr ) p->class.VECTOR.elms ) ) ;
	                  	break ;
#ifdef	RASTER
	case IMAGE_CLASS	: ( void ) strcpy( s,"#" ) ;
	                  	( void ) strcat( s,display( ( pntr ) p->class.Image.image_elements ) ) ;
	                  	break ;
#endif	RASTER
	case STRUCTURE_CLASS	: ( void ) strcpy( s,"structure( " ) ;
	                  	( void ) strcat( s,display( ( pntr ) p->class.STRUCTURE.fields ) ) ;
	                  	( void ) strcat( s," )" ) ;
	                  	break ;
	case PROC_CLASS		: ( void ) strcpy( s,"procedure(" ) ;
	                  	if ( ( ( pntr ) p->class.proc.args )->skind != NIL )
	                  	{
	                  		( void ) strcat( s," " ) ;
	                  		( void ) strcat( s,display( ( pntr ) p->class.proc.args ) ) ;
	                  	}
	                  	if ( !( ( ( pntr ) p->class.proc.result )->skind == SCALAR_CLASS && ( strcmp( ( ( pntr ) p->class.proc.result )->class.scalar.t_name,"void" ) == 0 ) ) )
	                  	{
	                  		( void ) strcat( s," -> " ) ;
	                  		( void ) strcat( s,display( ( pntr ) p->class.proc.result ) ) ;
	                  	}
	                  	( void ) strcat( s," )" ) ;
	                  	break ;
	case CONS_CLASS		: ( void ) strcpy( s,display( ( pntr ) p->class.cons.hd ) ) ;
	                  	if ( ( ( pntr ) p->class.cons.tl )->skind != NIL )
	                  	{
	                  		( void ) strcat( s,"," ) ;
	                  		( void ) strcat( s,display( ( pntr ) p->class.cons.tl ) ) ;
	                  	}
	                  	break ;
	case FIELD_CLASS	: ( void ) strcpy( s,"field( " ) ;
	                  	( void ) strcat( s,display( ( pntr ) p->class.field.fieldt ) ) ;
	                  	( void ) strcat( s," )" ) ;
	                  	break ;
	default			: ( void ) strcpy( s,"unknown" ) ;
	                  	break ;
	}
	return( s ) ;
}

bad_type( a )
pntr	a ;
{
	char	s[ MAXSIZE ] ;

	if ( !scalar_eq( a,ANY ) )
	{
		( void ) strcpy( s,"**** Type error **** " ) ;
		( void ) strcat( s,display( a ) ) ;
		( void ) strcat( s," may not be used in this context\n" ) ;
		err_mess( s ) ;
	}
}

bad_types( t,t1 )
pntr	t,t1 ;
{
	char	s[ MAXSIZE ] ;

	if ( !scalar_eq( t,ANY ) && !scalar_eq( t1,ANY ) )
	{
		( void ) strcpy( s,"**** Type error **** " ) ;
		( void ) strcat( s,display( t ) ) ;
		( void ) strcat( s," and " ) ;
		( void ) strcat( s,display( t1 ) ) ;
		( void ) strcat( s," are not compatible in this context\n" ) ;
		err_mess( s ) ;
	}
}

bad_types2( t,op )
pntr	t ;
char	*op ;
{
	char	s[ MAXSIZE ] ;

	if ( !scalar_eq( t,ANY )  )
	{
		( void ) strcpy( s,"**** Type error **** " ) ;
		( void ) strcat( s,display( t ) ) ;
		( void ) strcat( s," and operator "  ) ;
		( void ) strcat( s,op ) ;
		( void ) strcat( s," are not compatible in this context\n" ) ;
		err_mess( s ) ;
	}
}

pntr	int_real( t )
pntr	t ;
{
	if ( eq( INT,t ) ) return INT; else
	if ( eq( REAL,t ) ) return REAL; else
	{ bad_types( INT,t ); return ANY; }
}

match( a,b )
pntr	a,b ;
{
	if ( !eq( a,b ) )
	if ( eq( REAL,a ) && eq( INT,b ) ) float_op( ( psint ) 1 ); else bad_types( a,b ) ;
}

pntr	coerce( a,b )
pntr	a,b ;
{
	if ( scalar_eq( a,ANY ) || scalar_eq( b,ANY ) ) return ANY; else
	if ( eq( REAL,a ) ) { match( REAL,b ) ; return REAL ; } else
	if ( eq( INT,a ) )
	{
		if ( eq( REAL,b ) ) { float_op( ( psint ) 2 ) ; return REAL; }
                else { match( INT,b ) ; return INT ; }
        }
        else { bad_types( a,b ) ; return ANY ; }
}

directive()
{
	char	dis,s[ MAXSIZE ] ;
	int	i ;
	pntr	t ;

	dis = next_ch() ;
	option = TRUE ;
	while ( peep == ' ' ) dis = next_ch() ;
	read_name( s ) ;
	if ( streq( s,"list" ) ) listing = TRUE ; else
	if ( streq( s,"nolist" ) ) listing = FALSE ; else
	if ( streq( s,"ul" ) ) underline = TRUE ; else
	if ( streq( s,"noul" ) ) underline = FALSE ; else
	if ( streq( s,"title" ) )
	{
		while ( peep == ' ' ) dis = next_ch() ;
		if ( peep == '"' )
		{
			dis = next_ch() ;
			read_string() ;
			heading[ 0 ] = '\0' ;
			t = ( pntr ) the_string->class.str.str_pntr ;
			for( i = ( psint ) 0 ; i < the_string->class.str.str_int ; i++ )
			{
				heading[ i ] = t->class.str.str_int ;
				t = ( pntr ) t->class.str.str_pntr ;
			}
			heading[ i + 1 ] = '\0' ;
			title() ;
		} else syntax( dquote_sy ) ;
	} else
	if ( streq( s,"lines" ) )
	{
		next_sy() ;
		mustbe( eq_sy ) ;
		match( INT,lit_type ) ;
		lines_per_page = the_lit ;
	} else
	if ( streq( s,"newline" ) ) newlins = TRUE; else
	if ( streq( s,"nonewline" ) ) newlins = FALSE; else
	syntax( "option" ) ;
	dis = next_ch() ;
	while ( dis != '\n' && dis != '?' ) dis = next_ch() ;
	option = FALSE ;
}

/************************* Scope and Name Handler ***************************/


#ifdef	OUTLINE
psint	prelude	= TRUE ;
#endif	OUTLINE
psint	trademark = ( psint ) 0 ;
psint	max_trademark = ( psint ) 4088 ;
psint	max_stack = ( psint ) 1023 ;
psint	forwd_count = ( psint ) 0 ;
pntr	env_list ;
pntr	struct_list,s_str_list ;

enter_scope()
{
	env_list = mk_cons( nil,env_list ) ;
}

exit_scope()
{
	env_list = ( pntr ) env_list->class.cons.tl ;
}

forwds( head )
pntr	head ;
{
	pntr	t1 ;
	char	s[ MAXSIZE ] ;

	if ( head->skind != NIL )
	{
		forwds( ( pntr ) head->class.link.left ) ;
		t1 = ( pntr ) head->class.link.type1 ;
		if ( ( t1->skind == PROC_CLASS ) &&  t1->class.proc.forwd )
		{
			( void ) strcpy( s,"Declaration of forward procedure <" ) ;
			( void ) strcat( s,head->class.link.name ) ;
			( void ) strcat( s,"> required\n" ) ;
			err_mess( s ) ;
		}
		forwds( ( pntr ) head->class.link.right ) ;
	}
}

name_err( n )
char	*n ;
{
	char	s[ MAXSIZE ] ;

	( void ) strcpy( s,"The name " ) ;
	( void ) strcat( s,n ) ;
	( void ) strcat( s," has already been used\n" ) ;
	err_mess( s ) ;
}

pntr	enter( head,temp,checked )
pntr	head,temp ;
psint	checked ;
{
	if ( head->skind == NIL ) return( temp ) ; else
	if ( strcmp( temp->class.link.name,head->class.link.name ) < 0 )
	{
		( pntr ) head->class.link.left = enter( ( pntr ) head->class.link.left,temp,checked ) ;
		return( head ) ;
	} else
	if ( strcmp( temp->class.link.name,head->class.link.name ) > 0 )
	{
		( pntr ) head->class.link.right = enter( ( pntr ) head->class.link.right,temp,checked ) ;
		return( head ) ;
	} else
	if ( ( ( pntr ) head->class.link.type1 )->skind == PROC_CLASS && ( ( pntr ) head->class.link.type1 )->class.proc.forwd && ( ( pntr ) temp->class.link.type1 )->skind == PROC_CLASS )
	{
		dec_ms( st_size2 ) ;
		return( head ) ;
	} else
	{
		if ( !checked ) name_err( head->class.link.name ) ;
		return( head ) ;
	}
}

pntr	search( n,head )
char	*n ;
pntr	head ;
{
	register pntr	p ;
	
	p = head ;
	while ( p->skind != NIL && !streq( n,p->class.link.name ) )
	{
		if ( strcmp( n,p->class.link.name ) < 0 ) p = ( pntr ) p->class.link.left ; else p = ( pntr ) p->class.link.right ;
	}
	return( p ) ;
}

declare( n,t,checked )
char	*n ;
pntr	t ;
psint	checked ;
{
	register pntr	t1 ;
	register psint	add2 ;

	if ( t->skind == VAR_CLASS ) t1 = ( pntr ) t->class.var.contents ; else
	if ( t->skind == CONST_CLASS ) t1 = ( pntr ) t->class.CONST.content ; else t1 = t ;

	if ( pointer( t1 ) ) add2 = psp; else add2 = ssp ;
	if ( add2 > max_stack )
	{
		err_mess( "Stack declaration limit\n" ) ;
		ssp = ( psint ) 0 ; psp = ( psint ) 0 ;
	}
	( pntr ) env_list->class.cons.hd = enter( ( pntr ) env_list->class.cons.hd,mk_link( n,lex_level,add2,t,nil,nil ),checked ) ;
}

name_op( t,n,n1,r_value )
pntr	t ;
psint	n,n1,r_value ;
{
	if ( t->skind == FIELD_CLASS ) load_lit( INT,t->class.field.f_offset ) ;
	if ( t->skind == PROC_CLASS && t->class.proc.stand ) load_st_func( n1 / st_size2 ) ;
	else load( n,n1,r_value,t ) ;
}

pntr	search_name( n )
char	*n ;
{
	register pntr	p2,p = nil ;

	p2 = env_list ;
	while ( p2->skind != NIL && p->skind == NIL )
	{
		p = search( n,( pntr ) p2->class.cons.hd ) ;
		p2 = ( pntr ) p2->class.cons.tl ;
	}
	return( p ) ;
}

name_err1( n )
char	*n ;
{
	char	s[ MAXSIZE ] ;

	( void ) strcpy( s,"**** Undeclared name **** <" ) ;
	( void ) strcat( s,n ) ;
	( void ) strcat( s,"> has been used\n" ) ;
	err_mess( s ) ;
	declare( n,ANY,TRUE ) ;
}

pntr	look_up( p,r_value )
pntr	p ;
psint	r_value ;
{
	register pntr	p1 ;

	p1 = ( pntr ) p->class.link.type1 ;
	if ( p1->skind == VAR_CLASS ) p1 = ( pntr ) p1->class.var.contents ;
	if ( p1->skind == CONST_CLASS ) p1 = ( pntr ) p1->class.CONST.content ;
	name_op( p1,p->class.link.addr1,p->class.link.addr2,r_value ) ;
	p = ( pntr ) p->class.link.type1 ;
	if ( r_value )
	{
		if ( p->skind == VAR_CLASS ) return( ( pntr ) p->class.var.contents ) ; else
		if ( p->skind == CONST_CLASS ) return( ( pntr ) p->class.CONST.content ) ; else return( p ) ;
	} else
	return( p ) ;
}

pntr	lookup( n,r_value )
char	*n ;
psint	r_value ;
{
	register pntr p ;

	p = search_name( n ) ;
	if ( p->skind == NIL ) { name_err1( n ); return( ANY ); } else return( look_up( p,r_value ) ) ;
}

check_name( s )
char	*s ;
{
	register pntr	t ;

	t = search( s,( pntr ) env_list->class.cons.hd ) ;
	if ( t->skind != NIL ) name_err( s ) ;
}

declare1( s,t )
char	*s ;
pntr	t ;
{
	pntr	t1 ;

	if ( t->skind == VAR_CLASS ) t1 = ( pntr ) t->class.var.contents ; else t1 = ( pntr ) t->class.CONST.content ;
	inc_stack( t1 ) ;
	declare( s,t,TRUE ) ;
}

/*************************** Syntax Analysis ********************************/

pntr    expr_t ;
pntr    this,this1,this2 ;
psint   deflt,mixed_mode,xit,xit1 ;

extern  pntr    sequence(),get_types() ;


/********************* Reading Prelude and Standard functions **********************/

#ifdef	OUTLINE
read_prelude()
{
	if ( !ext ) load_struct_base() ;
	coding = TRUE ;
	load_proc_name( proc_name ) ;
	si = Open_prelude() ;
	peep = read_ch() ;
	next_sy() ;
	( void ) sequence() ;
	prelude = FALSE ;
	( void ) fclose( si ) ;
}
#endif	OUTLINE

read_stand_F()
{
	pntr	t  ;
	psint	flsave,save ;
	char	n[ MAXSIZE ] ;
#ifdef	OUTLINE
	psint	lis ;

	lis = listing ;
#endif	OUTLINE

	flsave = flsum ; flsum = FALSE ;

	inc_ms( ( psint ) 5 * st_size2 ) ;
	save = ssp ; ssp = ( psint ) -2 ;
	next_sy() ;
	while ( !feof( si ) || peep != '?' )
	{
		( void ) strcpy( n,the_name ) ;
		mustbe( identifier_sy ) ;
		if ( have( lp_sy ) ) t = get_types( TRUE ) ;
		else t = mk_proc( nil,VOID,FALSE,TRUE ) ;
		t->class.proc.stand = TRUE ;
		inc_ms( st_size2 ) ;
		if ( strcmp( n,"DUMMY" ) ) declare( n,t,TRUE ) ;
	}
	ssp = save ;
	( void ) fclose( si ) ;
#ifdef	OUTLINE
	read_prelude() ;
#endif	OUTLINE
	si = SI ;
	peep = read_ch() ;
	option = FALSE ; coding = TRUE ;
	line_count = ( psint ) 0 ; line_pos = ( psint ) 0 ;
	err_pos = ( psint ) 10 ; flsum = flsave ;
/*
	clear_line( error_line,OUTPUT_LENGTH,' ' ) ;
	for ( i = ( psint ) 0; i < ( psint ) 5; i++ ) clear_line( errors[i],OUTPUT_LENGTH,NUL ) ;
*/
#ifdef	OUTLINE
	listing = lis ;
#endif	OUTLINE
}

init()
{
/*	s_w = 0 ; */
	declare1( "i.w",mk_var( INT ) ) ;
	declare1( "s.w",mk_var( INT ) ) ;
	declare1( "r.w",mk_var( INT ) ) ;
	declare1( "maxint",mk_const( INT ) ) ;
	declare1( "epsilon",mk_const( REAL ) ) ;
	declare1( "pi",mk_const( REAL ) ) ;
	declare1( "maxreal",mk_const( REAL ) ) ;
	declare1( "s.o",mk_var( FILET ) ) ;
	declare1( "s.i",mk_var( FILET ) ) ;
#ifdef	RASTER
	inc_stack( image_type ) ; declare( "screen",mk_const( IMAGE ),TRUE ) ;
	inc_stack( image_type ) ; declare( "cursor",mk_const( IMAGE ),TRUE ) ;
#endif	RASTER
	read_stand_F() ;
}

psint	version = Version_no ;
psint	block_no = ( psint ) 0 ;

psint output_name_type( s,s1 ) 
char	*s ;
pntr	s1 ;
{
	char	*s2 ;
	psint	length ;

	( void ) fprintf( so,"%c%s ",'"',s ) ;
	s2 = display( s1 ) ;
	( void ) fprintf( so,"%s%c ",s2,'"' ) ;
	length = ( psint ) strlen( s ) + ( psint ) strlen( s2 ) + ( psint ) 3 ;
	return( length ) ;
}

psint output_externals()
{
	psint	size  = ( psint ) 0 ;
	pntr	t ;

	while ( external_list->skind != NIL )
	{
		t = ( pntr ) external_list->class.cons.hd ;
		while ( t->skind != NIL )
		{
			size += output_name_type( t->class.EXTERNAL.e_name,( pntr )t->class.EXTERNAL.e_type ) + ( psint ) 2 ;
#ifdef	M68000
			bout( ( ( psint ) t->class.EXTERNAL.e_addr + ( psint ) 1 ) >> 8 ) ;
			bout( ( psint ) t->class.EXTERNAL.e_addr + ( psint ) 1 ) ;
#else
			bout( ( psint ) t->class.EXTERNAL.e_addr + ( psint ) 1 ) ;
			bout( ( ( psint ) t->class.EXTERNAL.e_addr + ( psint ) 1 ) >> 8 ) ;
#endif	M68000
			t = ( pntr ) t->class.EXTERNAL.e_next ;
		}
		external_list = ( pntr ) external_list->class.cons.tl ;
	}
	( void ) fprintf( so,"%c%c",'"','"' ) ;
	return( size + ( psint ) 2 ) ;
}

output_st_info( total,code_total)
psint	total,code_total ;
{
	register psint	i,j,trailer_size = ( psint ) 24 ;

	i = ( psint ) 128 - ( total + trailer_size ) % ( psint ) 128 ;
	for ( j = ( psint ) 1 ; j <= i ; j++ ) ( void ) fprintf( so,"%c",' ' ) ;
	boutl( code_total ) ;
	boutl( last_segment ) ;
	boutl( code_size ) ;
#ifdef	M68000
	bout( externals >> 8 ) ; bout( externals ) ;
	bout( flsum ? ( line_count + ( psint ) 1 ) >> 8 : ( psint ) 0 ) ;
	bout( flsum ? line_count + ( psint ) 1 : ( psint ) 0 ) ;
	bout( version >> 8 ) ; bout( version ) ;
#else
	bout( externals ) ; bout( externals >> 8 ) ;
	bout( flsum ? line_count + ( psint ) 1 : ( psint ) 0 ) ;
	bout( flsum ? ( line_count + ( psint ) 1 ) >> 8 : ( psint ) 0 ) ;
	bout( version ) ; bout( version >> 8 ) ;
#endif  M68000
	boutl( total + i + trailer_size ) ;
#ifdef  M68000
	bout( block_no >> 8 ) ; bout( block_no ) ;
#else
	bout( block_no ) ; bout( block_no >> 8 ) ;
#endif  M68000
}

main( argc,argv )
psint	argc ;
char	*argv[] ;
{
	register char	*filename ;
	register psint	temp_cp,code_total,total ;

	set_options( argc,argv ) ;

	if ( argc == ( psint ) 1 )   /* make sure a filename has been given */
	{
		( void ) printf( "\nNo source filename given\n" ) ;
		exit( ( psint ) -1 ) ;
	}

	filename = argv[1] ;
	if ( flsum ) fl_sum = Create_flsum() ; else fl_sum = NULL ;
	si = Open_std_decls() ;
	SI = Open_source( filename ) ;
	so = Create_code( filename ) ;
	( void ) strcpy( proc_name,"Main" ) ;
	peep = read_ch() ;
	
	init_rwords() ;
	init_codeword() ;
	init_lines() ;
	init_types();   /* initialise INT,REAL etc */
	s_str_list = struct_list ;
	env_list = nil ;
	lit_type = nil ;
	expr_t = nil ;
	this = nil; this1 = nil; this2 = nil ;
	st_id_size = ( psint ) 4 * st_size + ( psint ) 3 * st_size2 ;

	/********************* MAIN PROGRAM *********************/

	enter_scope() ;
	init() ;
	next_sy() ;
	if ( page_count == ( psint ) 0 ) title() ;
	if ( ext )
	{
		while ( strcmp( symb,procedure_sy ) == 0 )
		{
			enter_scope() ;
			proc_decl() ;
			exit_scope() ;
			code_total = code_size + out_list( ( pntr ) s_str_list->class.struct_table.str_next ) ;
			s_str_list = mk_struct_table( ( psint ) 0,( psint ) 0,nil ) ;
			struct_list = s_str_list ;
			total = code_total +
				output_name_type( procedure_name,procedure_type ) + 
				output_externals() ;
	
			block_no += ( psint ) 1 ;
			output_st_info( total,code_total ) ;
			externals = ( psint ) 0 ; line_count = ( psint ) 0 ; trademark = ( psint ) 0 ;
		}
	} else
	{
#ifndef	OUTLINE
		load_struct_base() ;
		load_proc_name( proc_name ) ;
#endif	OUTLINE
		match( VOID,sequence() ) ;
	 	exit_scope() ;
	 	finish_op() ;
#ifdef	PERQ
		while( ( cp + ( psint ) 2 ) % ( psint ) 4 != ( psint ) 0  && coding ) add_byte( ( psint ) 0 ) ;
#else	
		while( cp % ( psint ) 4 != ( psint ) 0  && coding ) add_byte( ( psint ) 0 ) ;
#endif
		temp_cp = cp; cp = ( psint ) 0 ;
		endsegment( temp_cp,( psint ) 0 ) ;
		code_total = code_size + out_list( ( pntr ) s_str_list->class.struct_table.str_next ) ;
		( void ) fprintf( so,"%c%c",'"','"' ) ;
		total = code_total + output_externals() + ( psint ) 2 ;
		if ( total == code_total + ( psint ) 4 ) block_no = ( psint ) -1 ;
		output_st_info( total,code_total ) ;
	}
	if ( !streq( symb,eot ) )
	{
		syntax( eot ) ;
		while ( !feof( si ) ) next_sy() ;
	}
	line_image[ line_pos++ ] = '\n' ;
	output_line() ;
	( void ) fclose( so ) ;
	Shutdown() ;
	if ( flsum ) ( void ) fclose( fl_sum ) ;
	if ( error ) Delete_code_file() ;
	exit( 0 ) ;
}
