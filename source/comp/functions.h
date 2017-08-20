
/* cgen.c */

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


/* csc.c */

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


/* io.c */

extern	FILE	*Create_code() ;
extern	FILE	*Open_source() ;
extern	FILE	*Open_prelude() ;
extern	FILE	*Open_std_decls() ;
extern	FILE	*Create_flsum() ;


/* stype.c */

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


/* syntax1.c */

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


/* syntax2.c */

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
