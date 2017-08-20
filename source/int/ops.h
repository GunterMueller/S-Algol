/* Instruction Definitions */

/* Jump Instructions */

#define NO_OP			0
#define FJUMP_2			0 + 128
#define BJUMP_1			1
#define BJUMP_2			1 + 128

#define JUMPF_2			2 + 128

#define JUMPFF_2		3 + 128

#define JUMPTT_2		4 + 128

#define FOR_TEST_2		5 + 128
#define FOR_ST_1		6
#define FOR_ST_2		6 + 128
#define CJUMP_IB_2		7
#define CJUMP_R_2		7 + 128
#define CJUMP_S_2		8
#define CJUMP_P_2		8 + 128

#define BJUMPT_1		10
#define BJUMPT_2		10 + 128


/* Stack Accessing Instructions */

#define LC_1			12
#define LC_2			12 + 128
#define PLC_1			13
#define PLC_2			13 + 128
#define DLC_1			14
#define DLC_2			14 + 128
#define MST_LC_1		15
#define MST_LC_2		15 + 128
#define GBL_1			16
#define GBL_2			16 + 128
#define PGBL_1			17
#define PGBL_2			17 + 128
#define DGBL_1			18
#define DGBL_2			18 + 128
#define MST_GB_1		19
#define MST_GB_2		19 + 128

#define LD_2			24
#define LD_3			24 + 128
#define PLD_2			25
#define PLD_3			25 + 128
#define DLD_2			26
#define DLD_3			26 + 128
#define MST_LD_2		27
#define MST_LD_3		27 + 128
#define LCA_1			28
#define LCA_2			28 + 128
#define PLCA_1			29
#define PLCA_2			29 + 128
#define DLCA_1			30
#define DLCA_2			30 + 128
#define GBLA_1			32
#define GBLA_2			32 + 128
#define PGBLA_1			33
#define PGBLA_2			33 + 128
#define DGBLA_1			34
#define DGBLA_2			34 + 128

#define LDA_2			40
#define LDA_3			40 + 128
#define PLDA_2			41
#define PLDA_3			41 + 128
#define DLDA_2			42
#define DLDA_3			42 + 128

/* Procedure entry and exit Instructions */

#define APPLY_1			44
#define APPLY_2			44 + 128
#define ST_CL_6			45

#define RTN_IB			47
#define RTN_R			47 + 128
#define RTN_S			48
#define RTN_P			48 + 128
#define RTN_V			49 + 128

/* Here are the new ones for S */

#define GB_APP_1		51
#define GB_APP_2		51 + 128
#define LC_APP_1		52
#define LC_APP_2		52 + 128
#define RET_IB_2		53
#define RET_IB_4		53 + 128
#define RET_R_2			54
#define RET_R_4			54 + 128
#define RET_S_2			55
#define RET_S_4			55 + 128
#define RET_P_2			56
#define RET_P_4			56 + 128

#define RET_V_2			58
#define RET_V_4			58 + 128

#define LOAD_SFN_1		59
#define MST_SFN_1		59 + 128
#define APPLY_SFN_1		60
#define MST_EXT_1		61
#define FORWARD			62
#define LD_APP_2		63
#define LD_APP_3		63 + 128

/* Structure and Vector Instructions */

#define F_STR_2			64
#define IS_OP			65
#define ISNT_OP			65 + 128
#define SUBS_IB			66
#define SUBS_R			66 + 128
#define SUBS_S			67
#define SUBS_P			67 + 128

#define SSA_IB			69
#define SSA_R			69 + 128
#define SSA_S			70
#define SSA_P			70 + 128

#define MAKEV_IB_2		72
#define MAKEV_R_2		72 + 128
#define MAKEV_S_2		73
#define MAKEV_P_2		73 + 128

#define ILF_IB_2		75
#define ILF_R_2			75 + 128
#define ILF_S_2			76
#define ILF_P_2			76 + 128

#define SUBV_IB			78
#define SUBV_R			78 + 128
#define SUBV_S			79
#define SUBV_P			79 + 128

#define SVA_IB			81
#define SVA_R			81 + 128
#define SVA_S			82
#define SVA_P			82 + 128

#define UPB_OP			84
#define LWB_OP			84 + 128
#define CONCAT_OP		85
#define SUBSTR_OP		85 + 128
#define LD_TM_5			86
#define LD_TM_6			86 + 128

/* Load Literal Instructions */

#define LL_INT_1		90
#define LL_INT_4		90 + 128
#define LL_BOOL_1		91
#define LL_REAL_8		91 + 128
#define LL_STR_1		92

#define LL_CHAR_1		93
#define LL_FILE			93 + 128
#define LL_NIL_STRING		94
#define LL_N_P			94 + 128


/* Comparison Instructions */

#define EQ_IB			96
#define EQ_R			96 + 128
#define EQ_S			97
#define EQ_P			97 + 128

#define NEQ_IB			99
#define NEQ_R			99 + 128
#define NEQ_S			100
#define NEQ_P			100 + 128

#define LT_I			102
#define LT_R			102 + 128
#define LT_S			103

#define LE_I			104
#define LE_R			104 + 128
#define LE_S			105

#define GT_I			106
#define GT_R			106 + 128
#define GT_S			107

#define GE_I			108
#define GE_R			108 + 128
#define GE_S			109


/* Arithmetic Instructions */

#define PLUS			110
#define TIMES			110 + 128
#define MINUS			111
#define DIV			111 + 128
#define REM			112
#define NEG			112 + 128
#define FPLUS			113
#define FTIMES			113 + 128
#define FMINUS			114
#define FDIVIDE			114 + 128
#define NOT_OP			115
#define FNEG			115 + 128
#define FLOATING1		116
#define FLOATING2		116 + 128

/* Stack Manipulation, Termination and IO Instructions */

#define ERASE_IB		120
#define ERASE_R			120 + 128
#define ERASE_S			121
#define ERASE_P			121 + 128

#define REV_MS			123
#define REV_PS			123 + 128
#define NLNE_1			124
#define NLNE_2			124 + 128
#define FINISH_OP		125
#define ABORT_OP		125 + 128


#define READ_OP			127
#define WRITE_OP		127 + 128

#ifdef	RASTER

/*	Raster graphics ops	*/

#define R_COPY                  20
#define R_NOT                   20 + 128
#define R_AND                   21
#define R_NAND                  21 + 128
#define R_ROR                   22
#define R_NOR                   22 + 128
#define R_XOR                   23
#define R_XNOR                  23 + 128
#define SUB_IMAGE               35
#define SUB_PIXEL               35 + 128
#define MK_IMAGE                36
#define MK_PIXEL                36 + 128
#define LIMIT_2                 37
#define LIMIT_4                 37 + 128

#endif RASTER
