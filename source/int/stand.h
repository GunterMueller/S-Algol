#define SIN		0
#define COS		1
#define EXP		2
#define LN		3
#define SQRT		4
#define ATAN		5
#define TRUNCATE	6
#define ABS		7
#define RABS		8
#define FIDDLE_R	9
#define OPEN		10
#define CLOSE		11
#define SEEK		12
#define CREATE		13
#define FLUSH		14
#define OPTIONS		15
#define CODE		16
#define DECODE		17
#define LENGTH		18
#define IFORMAT		19
#define LETTER		20
#define DIGIT		21
#define LINE_NUMBER	22
#define SHIFT_R		23
#define SHIFT_L		24
#define B_AND		25
#define B_OR		26
#define EXEC		27
#define FORK		28
#define WAIT		29
#define TRACE		30
#define FIND_SUBSTR	31
#define ENVIRONMENT	32
#define SYSTEM		33
#define RANDOM		34
#define B_NOT		35
#define B_XOR		36
#define DATE		37
#define TIME		38
#define INTRPT		39
#define INTRPTON	40
#define INTRPTOFF	41
#define RDVEC		42
#define WRTVEC		43
#ifdef	PERQ
#define CRTBMAP		44
#define CRTWIND		45
#define CCURS		46
#define CDRAWLN		47
#define COLOUROF	48	
#define ISAWIND		49
#define LNEND		50
#define SETLOCAT	51
#define SETCURS		52
#define RASTEROP	53
#define LOCATOR		54
#define SIZEOF		55
#define HIDEUPS		56
#define SHOWUPS		57
#define CURS		58
#define MKBMAP		59
#define MKWIND		60
#define DRAWPIC		61
#define LIMIT		62
#define ERASETO		63
#define CHKCOL		64
#define DRAWLIN		65
#define STDWIN		66
#define COLOF		67
#define FILL		68
#define MNKMEN		69
#define PICMEN		70
#define FRST		71
#define SCND		72
#endif	PERQ
#ifdef	RASTER
#define	XDMN		44
#define	YDMN		45
#define	LOCAT		46
#define	CURSTIP		47
#define	PIXEL		48
#define	CONST_IM	49
#define	VAR_IM		50
#define	DEPTH		51
#define	PDEPTH		52
#define	COL_MAP		53
#define	COL_OF		54
#define	LNEND		55
#define	PLANE		56
#define	PNXLINE		57
#define	CURSON		58
#define	CURSOFF		59
#define	INPPEND		60
#endif	RASTER
