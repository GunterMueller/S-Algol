#include "int.h"
#include "ops.h"
#ifdef PERQ
#include "wuser.h"
#include "sgwin.h"
#include "sys/wdesc.h"
#include "sys/wmctl.h"
#include "types.h"
#include "sys/inode.h"
#endif PERQ
/* #define DEBUG */

#define VM1	"Vector indexing\nindex = "
#define VM2	"\nlwb = "
#define VM3	"\nupb = "
#define SM1	"Substring indexing\nstring size = "
#define SM2	"\nsubstring start = "
#define SM3	"\nsubstring length = "

int inited = SFALSE ;

char *STRUCTERR	 = "Illegal structure field" ;
int nil_trade = -1 ;
extern int *c_string(),*c_v_r(),*c_v_ib(),*c_v_p(),*c_structure(),*chain_down(),*iliffe() ;
#ifdef	RASTER
extern int *c_image() ;
#endif	RASTER

main( argc,argv,envp )
int argc ; 
char **argv,**envp ;
{
	register int tmp,tmp1,tmp2,tmp3,*ptr,*ptr1,*ptr2,*ptr3 ;
	psreal	 f1,f2 ;
	int index,size ;
	extern unsigned char *go_sub() ;

	NIL_PTR = &nil_trade ;
	envv = envp ;
	initialise( argc,argv ) ;
	inited = STRUE ;
#ifdef	RASTOP
	start_itimer() ;
#endif	RASTOP
call:
	pc = ( unsigned char * ) storeBase + sf[ PC ] ; 
	psf = ( psptr * ) mkptr( sf[ PDL ] ) ;
	if ( sf[ SL ] )
	{
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
/*
		NEXTPARAM( tmp ) ;
*/
#endif	M68000
		if ( tmp & B_15 )	 /* struc off */
		{
#ifdef	M68000
			tmp = ( int )( *pc++ ) << 8 ;
			tmp |= ( int )( *pc++ ) ;
#else
			tmp = ( int )( *pc++ ) ;
			tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		}
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
/*
		NEXTPARAM( tmp1 ) ;
*/
#endif	M68000
		if ( sp - tmp < m_top ) error( "Main stack overflow" ) ;
		if ( psp - tmp1 < p_top ) error( "Pointer stack overflow" ) ;
		pc += 4 - ( ( long ) pc & B_0_1 ) ; /* get alignment right */
		/* note perq strings and segments padded to 4 byte boundery */
		pc += *( ( int * ) pc ) + 4 ;
	}
	else pc = go_sub( sf[ PC ] ) ;
decode:
#ifdef DEBUG
	debug( pc ) ;
#endif
	switch( ( int )( *pc++ ) )
	{

		/*********** JUMP INSTRUCTIONS***********/

	case NO_OP:
		goto decode ;
	case FJUMP_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		pc += tmp ;
		goto decode ;
	case BJUMP_1: 
		tmp = ( int )( *pc++ ) ;
		pc -= tmp ;
		goto decode ;
	case BJUMP_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		pc -= tmp ;
		goto decode ;
	case JUMPF_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		if ( !( *sp++ ) ) pc += tmp ;
		goto decode ;
	case JUMPFF_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		if ( *sp ) sp++  ; 
		else pc += tmp ;
		goto decode ;
	case JUMPTT_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		if ( *sp ) pc += tmp ;
		else sp++ ; 
		goto decode ;
	case FOR_TEST_2:
		{
			register int *ptr ;

			ptr = sp ;
#ifdef	M68000
			tmp = ( int )( *pc++ ) << 8 ;
			tmp |= ( int )( *pc++ ) ;
#else
			tmp = ( int )( *pc++ ) ;
			tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
			if ( ( ( *ptr >= 0 ) && ( ptr[ 2 ] > ptr[ 1 ] ) ) ||
			    ( ( *ptr < 0 ) && ( ptr[ 2 ] < ptr[ 1 ] ) ) )
			{ 
				pc += tmp ; 
				sp += 3 ; 
			}
		}
		goto decode ;
	case FOR_ST_1: 
		sp[ 2 ] += *sp ;
		tmp = ( int )( *pc++ ) ;
		pc -= tmp ;
		goto decode ;
	case FOR_ST_2: 
		sp[ 2 ] += *sp ; 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		pc -= tmp ;
		goto decode ;
	case CJUMP_IB_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		tmp1 = *sp++ ;
		if ( *sp == tmp1 ) { 
			sp++ ; 
			pc += tmp ; 
		}
		goto decode ;
	case CJUMP_R_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
/*
		ptr = sp + 2 ; 
		ptr1 = sp ;
		if ( *( ( double * ) ptr ) == *( ( double * ) ptr1 ) )
		{ 
			sp += 2 ; 
			pc += tmp ; 
		} else sp += 2 ;
*/
		ptr = ( psint * )( &f2 ) ;
		ptr[ FLOAT1 ] = *sp++ ;
		ptr[ FLOAT0 ] = *sp++ ;
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = sp[ 0 ] ;
		ptr[ FLOAT0 ] = sp[ 1 ] ;

		if ( f1 == f2 )
		{
			sp += 2 ;
			pc += tmp ;
		}
		goto decode ;
	case CJUMP_S_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr = mkptr( *psp++ ) ;
		ptr1 = mkptr( *psp ) ;
		if ( eq_string( ptr,ptr1 ) ) { 
			psp++ ; 
			pc += tmp ; 
		}
		goto decode ;
	case CJUMP_P_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		tmp1 = *psp++ ;
		if ( *psp == tmp1 ) { 
			psp++ ; 
			pc += tmp ; 
		}
		goto decode ;
	case BJUMPT_1:
		tmp = ( int )( *pc++ ) ;
		if ( *sp++ ) pc -= tmp ;
		goto decode ;
	case BJUMPT_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		if ( *sp++ ) pc -= tmp ;
		goto decode ;

		/*********** STACK ACCESSING INSTRUCTIONS ***********/

	case LC_1:
		tmp = ( int )( *pc++ ) ;
		*--sp = sf[ -tmp ] ;
		goto decode ;
	case LC_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sf[ -tmp ] ;
		goto decode ;
	case PLC_1:
		tmp = ( int )( *pc++ ) ;
		*--psp = psf[ -tmp ] ;
		goto decode ;
	case PLC_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--psp = psf[ -tmp ] ;
		goto decode ;
	case DLC_1:
		tmp = ( int )( *pc++ ) ;
		*--sp = sf[ 1 - tmp ] ; 
		*--sp = sf[ -tmp ] ;
		goto decode ;
	case DLC_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sf[ 1 - tmp ] ; 
		*--sp = sf[ -tmp ] ;
		goto decode ;
	case MST_LC_1: 
		tmp = ( int )( *pc++ ) ;
		*--sp = sf[ 1 - tmp ] ; 
		*--sp = sf[ -tmp ] ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case MST_LC_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sf[ 1 - tmp ] ; 
		*--sp = sf[ -tmp ] ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case GBL_1:
		tmp = ( int )( *pc++ ) ;
		*--sp = sb[ -tmp ] ;
		goto decode ;
	case GBL_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sb[ -tmp ] ;
		goto decode ;
	case PGBL_1:
		tmp = ( int )( *pc++ ) ;
		*--psp = psb[ -tmp ] ;
		goto decode ;
	case PGBL_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--psp = psb[ -tmp ] ;
		goto decode ;
	case DGBL_1:
		tmp = ( int )( *pc++ ) ;
		*--sp = sb[ 1 - tmp ] ; 
		*--sp = sb[ -tmp ] ;
		goto decode ;
	case DGBL_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sb[ 1 - tmp ] ; 
		*--sp = sb[ -tmp ] ;
		goto decode ;
	case MST_GB_1: 
		tmp = ( int )( *pc++ ) ;
		*--sp = sb[ 1 - tmp ] ; 
		*--sp = sb[ -tmp ] ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case MST_GB_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sb[ 1 - tmp ] ; 
		*--sp = sb[ -tmp ] ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case LD_2:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		*--sp = ptr[ -(( int )( *pc++ )) ] ;
		goto decode ;
	case LD_3:
		ptr = chain_down( ( int )( *pc++ ) ) ;
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = ptr[ -tmp ] ;
		goto decode ;
	case PLD_2:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		ptr = mkptr( ptr[ PDL ] ) ;
		*--psp = ptr[ -(( int )( *pc++ )) ] ;
		goto decode ;
	case PLD_3:
		ptr = chain_down( ( int )( *pc++ ) ) ;
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr = mkptr( ptr[ PDL ] ) ;
		*--psp = ptr[ -tmp ] ;
		goto decode ;
	case DLD_2:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		tmp = ( int )( *pc++ ) ;
		*--sp = ptr[ 1 - tmp ] ; 
		*--sp = ptr[ -tmp ] ; 
		goto decode ;
	case DLD_3:
		ptr = chain_down( ( int )( *pc++ ) ) ;
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = ptr[ 1 - tmp ] ; 
		*--sp = ptr[ -tmp ] ; 
		goto decode ;
	case MST_LD_2: 
		tmp1 = ( int )( *pc++ ) ;
		tmp2 = ( int )( *pc++ ) ;
		ptr = chain_down( tmp1 ) - tmp2 ;
		*--sp = ptr[ 1 ] ; 
		*--sp = *ptr ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case MST_LD_3: 
		tmp1 = ( int )( *pc++ ) ;
#ifdef	M68000
		tmp2 = ( int )( *pc++ ) << 8 ;
		tmp2 |= ( int )( *pc++ ) ;
#else
		tmp2 = ( int )( *pc++ ) ;
		tmp2 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr = chain_down( tmp1 ) - tmp2 ;
		*--sp = ptr[ 1 ] ; 
		*--sp = *ptr ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case LCA_1:
		sf[ -( int )( *pc++ ) ] = *sp++ ;
		goto decode ;
	case LCA_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sf[ -tmp ] = *sp++ ;
		goto decode ;
	case PLCA_1:
		psf[ -( int )( *pc++ ) ] = *psp++ ;
		goto decode ;
	case PLCA_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		psf[ -tmp ] = *psp++ ;
		goto decode ;
	case DLCA_1:
		tmp = ( int )( *pc++ ) ;
		sf[ -tmp ] = *sp++ ;
		sf[ 1 - tmp ] = *sp++ ;
		goto decode ;
	case DLCA_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sf[ -tmp ] = *sp++ ;
		sf[ 1 - tmp ] = *sp++ ;
		goto decode ;
	case GBLA_1:
		sb[ -( int )( *pc++ ) ] = *sp++ ;
		goto decode ;
	case GBLA_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sb[ -tmp ] = *sp++ ;
		goto decode ;
	case PGBLA_1:
		psb[ -( int )( *pc++ ) ] = *psp++ ;
		goto decode ;
	case PGBLA_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		psb[ -tmp ] = *psp++ ;
		goto decode ;
	case DGBLA_1:
		tmp = ( int )( *pc++ ) ;
		sb[ -tmp ] = *sp++ ;
		sb[ 1 - tmp ] = *sp++ ;
		goto decode ;
	case DGBLA_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sb[ -tmp ] = *sp++ ;
		sb[ 1 - tmp ] = *sp++ ;
		goto decode ;
	case LDA_2:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		tmp = ( int )( *pc++ ) ;
		ptr[ -tmp ] = *sp++ ;
		goto decode ;
	case LDA_3:
		tmp = ( int )( *pc++ ) ;
		ptr = chain_down( tmp ) ;
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr[ -tmp ] = *sp++ ;
		goto decode ;
	case PLDA_2:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		ptr = mkptr( ptr[ PDL ] ) ;
		tmp = ( int )( *pc++ ) ;
		ptr[ -tmp ] = *psp++ ;
		goto decode ;
	case PLDA_3:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		ptr = mkptr( ptr[ PDL ] ) ;
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr[ -tmp ] = *psp++ ;
		goto decode ;
	case DLDA_2:
		ptr = chain_down( ( int )( *pc++ ) ) ;
		tmp = ( int )( *pc++ ) ;
		ptr[ -tmp ] = *sp++ ;
		ptr[ 1 - tmp ] = *sp++ ;
		goto decode ;
	case DLDA_3:
		ptr = chain_down( ( int )( *pc++ ) ) ;
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr[ -tmp ] = *sp++ ;
		ptr[ 1 - tmp ] = *sp++ ;
		goto decode ;

		/*********** PROCEDURE ENTRY AND EXIT ***********/

	case APPLY_1: 
		sf = sp + ( int )( *pc++ ) + 6 ;
		sf[ RA ] = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		sf[ LINE_NO ] = line_no ; 
		goto call ;
	case APPLY_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sf = sp + tmp + 6 ;
		sf[ RA ] = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		sf[ LINE_NO ] = line_no ; 
		goto call ;
	case ST_CL_6: 
		{
#ifdef	M68000
			tmp = ( int )( *pc++ ) << 8 ;
			tmp |= ( int )( *pc++ ) ;
#else
			tmp = ( int )( *pc++ ) ;
			tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
#ifdef PERQ
			tmp1 = ( int )( *pc++ ) << 16 ;
			tmp1 |= ( int )( *pc++ ) << 24 ;
			tmp1 |= ( int )( *pc++ ) ;
			tmp1 |= ( int )( *pc++ ) << 8 ;
#else
#ifdef	M68000
			tmp1 = ( int )( *pc++ ) << 24 ;
			tmp1 |= ( int )( *pc++ ) << 16 ;
			tmp1 |= ( int )( *pc++ ) << 8 ;
			tmp1 |= ( int )( *pc++ ) ;
#else
			tmp1 = ( int )( *pc++ ) ;
			tmp1 |= ( int )( *pc++ ) << 8 ;
			tmp1 |= ( int )( *pc++ ) << 16 ;
			tmp1 |= ( int )( *pc++ ) << 24 ;
#endif	M68000
#endif	PERQ
			tmp1 += ( int ) ( pc - ( unsigned char * ) storeBase ) ;
			if ( ( sf - tmp ) == sp )
			{ 
				*--sp = tmp1 ; 
				*--sp = mkpsptr( sf ) ; 
			}
			else
			{ 
				sf[ -tmp - 1 ] = tmp1 ; 
				sf[ -tmp - 2 ] = mkpsptr( sf ) ; 
			}
		}
		goto decode ;
	case RTN_IB: 
		tmp = *sp ; 
		psp = psf ; 
		sp = sf ; 
		line_no = sf[ LINE_NO ] ;
		pc = ( unsigned char * ) storeBase + sf[ RA ] ;
		sf = mkptr( sf[ DL ] ) ;
		psf = mkptr( sf[ PDL ] ) ;
		*--sp = tmp ;
		goto decode ;
	case RTN_R:
		{
/*
			register double *f1,*f2 ;

			f1 = ( double * ) sp ;
			psp = psf ; 
			sp = sf - 2 ; 
			line_no = sf[ LINE_NO ] ;
			pc = ( unsigned char * ) storeBase + sf[ RA ] ;
			sf = mkptr( sf[ DL ] ) ;
			psf = mkptr( sf[ PDL ] ) ;
			f2 = ( double * ) sp ; 
			*f2 = *f1 ;
*/
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr[ FLOAT1 ] = *sp ;
			psp = psf ; 
			sp = sf ; 
			line_no = sf[ LINE_NO ] ;
			pc = ( unsigned char * ) storeBase + sf[ RA ] ;
			sf = mkptr( sf[ DL ] ) ;
			psf = mkptr( sf[ PDL ] ) ;
			*--sp = ptr[ FLOAT0 ] ;
			*--sp = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case RTN_S:
	case RTN_P: 
		tmp = *psp ; 
		psp = psf ; 
		sp = sf ; 
		line_no = sf[ LINE_NO ] ;
		pc = ( unsigned char * ) storeBase + sf[ RA ] ;
		sf = mkptr( sf[ DL ] ) ; 
		psf = mkptr( sf[ PDL ] ) ;
		*--psp = tmp ;
		goto decode ;
	case RTN_V: 
		psp = psf ; 
		sp = sf ; 
		line_no = sf[ LINE_NO ] ;
		pc = ( unsigned char * ) storeBase + sf[ RA ] ;
		sf = mkptr( sf[ DL ] ) ; 
		psf = mkptr( sf[ PDL ] ) ;
		goto decode ;

		/*********** PARTICULAR S INSTRUCTIONS ***********/

	case GB_APP_1: 
		tmp = ( int )( *pc++ ) ; 
		*--sp = sb[ 1 - tmp ] ;
		*--sp = sb[ -tmp ] ;
		*--sp = mkpsptr( sf ) ;
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ;
		sf = sp + 6 ; 
		goto call ;
	case GB_APP_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sb[ 1 - tmp ] ;
		*--sp = sb[ -tmp ] ;
		*--sp = mkpsptr( sf ) ;
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ;
		sf = sp + 6 ; 
		goto call ;
	case LC_APP_1: 
		tmp = ( int )( *pc++ ) ; 
		*--sp = sf[ 1 - tmp ] ;
		*--sp = sf[ -tmp ] ;
		*--sp = mkpsptr( sf ) ;
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ;
		sf = sp + 6 ; 
		goto call ;
	case LC_APP_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = sf[ 1 - tmp ] ;
		*--sp = sf[ -tmp ] ;
		*--sp = mkpsptr( sf ) ;
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ;
		sf = sp + 6 ; 
		goto call ;
	case RET_IB_2: 
		tmp = *sp ;
		sp = sf - ( int )( *pc++ ) ;
		psp = psf - ( int )( *pc++ ) ;
		*--sp = tmp ;
		goto decode ;
	case RET_IB_4: 
		tmp = *sp ;
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sp = sf - tmp1 ;
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		psp = psf - tmp1 ;
		*--sp = tmp ;
		goto decode ;
	case RET_R_2:
		{
/*
			register double *f1,*f2 ;

			f1 = ( double * ) sp ;
			sp = sf - 2 - ( int )( *pc++ ) ;
			psp = psf - ( int )( *pc++ ) ;
			f2 = ( double * ) sp ; 
			*f2 = *f1 ;
*/
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr[ FLOAT1 ] = *sp ;
			sp = sf - ( int )( *pc++ ) ; 
			psp = psf - ( int )( *pc++ ) ;
			*--sp = ptr[ FLOAT0 ] ;
			*--sp = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case RET_R_4:
		{
/*
			register double *f1,*f2 ;

			f1 = ( double * ) sp ;
#ifdef	M68000
			tmp = ( int )( *pc++ ) << 8 ;
			tmp |= ( int )( *pc++ ) ;
#else
			tmp = ( int )( *pc++ ) ;
			tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
			sp = sf - 2 - tmp ;
#ifdef	M68000
			tmp1 = ( int )( *pc++ ) << 8 ;
			tmp1 |= ( int )( *pc++ ) ;
#else
			tmp1 = ( int )( *pc++ ) ;
			tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
			psp = psf - tmp1 ;
			f2 = ( double * ) sp ; 
			*f2 = *f1 ;
*/
#ifdef	M68000
			tmp = ( int )( *pc++ ) << 8 ;
			tmp |= ( int )( *pc++ ) ;
#else
			tmp = ( int )( *pc++ ) ;
			tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
#ifdef	M68000
			tmp1 = ( int )( *pc++ ) << 8 ;
			tmp1 |= ( int )( *pc++ ) ;
#else
			tmp1 = ( int )( *pc++ ) ;
			tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr[ FLOAT1 ] = *sp ;
			sp = sf - tmp ;
			psp = psf - tmp1 ;
			*--sp = ptr[ FLOAT0 ] ;
			*--sp = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case RET_S_2:
	case RET_P_2: 
		tmp = *psp ;
		sp = sf - ( int )( *pc++ ) ;
		psp = psf - ( int )( *pc++ ) ;
		*--psp = tmp ;
		goto decode ;
	case RET_S_4:
	case RET_P_4: 
		tmp = *psp ;
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sp = sf - tmp1 ;
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		psp = psf - tmp1 ;
		*--psp = tmp ;
		goto decode ;
	case RET_V_2: 
		sp = sf - ( int )( *pc++ ) ;
		psp = psf - ( int )( *pc++ ) ;
		goto decode ;
	case RET_V_4: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		sp = sf - tmp ;
		psp = psf - tmp1 ;
		goto decode ;
	case LOAD_SFN_1: 
		ptr = sb + ( int )( *pc++ ) ;
		*--sp = ptr[1] ; 
		*--sp = *ptr ;
		goto decode ;
	case MST_SFN_1: 
		ptr = sb + ( int )( *pc++ ) ;
		*--sp = ptr[ 1 ] ; 
		*--sp = *ptr ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		sp -= 2 ;
		goto decode ;
	case APPLY_SFN_1: 
		ptr = sb + ( int )( *pc++ ) ;
		*--sp = ptr[ 1 ] ; 
		*--sp = *ptr ;
		tmp = mkpsptr( sf ) ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ; 
		sf = sp + 6 ; 
		goto call ;
	case MST_EXT_1:
		{
			register int *s1,*s2 ;

			s2 = sb - ( int )( *pc++ )  ;
			s1 = sp ; 
			sp -= 6 ;
			*sp = *s1 ; 
			sp[ 1 ] = s1[ 1 ] ; 
			sp[ 2 ] = s1[ 2 ] ;
			sp[ 5 ] = mkpsptr( psp ) ; 
			sp[ 6 ] = mkpsptr( sf ) ; 
			sp[ 7 ] = *s2 ; 
			sp[ 8 ] = s2[ 1 ] ;
			psp[ -1 ] = *psp ; 
			psp-- ;
		}
		goto decode ;
	case FORWARD: 
		sp -= 2 ;
		goto decode ;
	case LD_APP_2:
		tmp1 = ( int )( *pc++ ) ;
		tmp2 = ( int )( *pc++ ) ;
		ptr = chain_down( tmp1 ) - tmp2 ;
		*--sp = ptr[ 1 ] ; 
		*--sp = *ptr ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ; 
		sf = sp + 6 ; 
		goto call ;
	case LD_APP_3: 
		tmp = ( int )( *pc++ ) ;
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 8 ;
		tmp1 |= ( int )( *pc++ ) ;
#else
		tmp1 = ( int )( *pc++ ) ;
		tmp1 |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr = chain_down( tmp ) - tmp1 ;
		*--sp = ptr[ 1 ] ; 
		*--sp = *ptr ;
		*--sp = mkpsptr( sf ) ; 
		*--sp = mkpsptr( psp ) ; 
		*--sp = ( psint )( pc - ( unsigned char * ) storeBase ) ;
		*--sp = line_no ; 
		sf = sp + 6 ; 
		goto call ;

		/*********** STRUCTURE AND VECTOR INSTRUCTIONS ***********/

	case F_STR_2: 
		{
			register int tm,*p,i ;
			int size,nptrs ;

#ifdef	M68000
			tmp = ( int )( *pc++ ) << 8 ;
			tmp |= ( int )( *pc++ ) ;
#else
			tmp = ( int )( *pc++ ) ;
			tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
			tm = sf[ -tmp ] ;
			size = s_base[ tm / 2 ] ;
			nptrs = s_base[ tm / 2 + 1 ] ;
			p = c_structure( tm ) ; 
			p += size - 1 ;
			size -= nptrs ;
			for ( i = 2 ; i <= size ; i++ ) *p-- = *sp++ ;
			for ( i = 1 ; i <= nptrs ; i++ ) *p-- = *psp++ ;
			*--psp = mkpsptr( p ) ; 
			sp++ ; 
		}
		goto decode ;
	case IS_OP: 
		ptr = mkptr( *psp++ ) ; 
		tmp = *sp ;
		*sp = ( ( *ptr & B_0_15 ) == tmp ) ;
		goto decode ;
	case ISNT_OP: 
		ptr = mkptr( *psp++ ) ; 
		tmp = *sp ;
		*sp = ( ( *ptr & B_0_15 ) != tmp ) ;
		goto decode ;
	case SUBS_IB: 
		ptr = mkptr( *psp++ ) ;
		if ( *sp++ != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
		*sp = ptr[ *sp ] ;
		goto decode ;
	case SUBS_R:
		{
/*
			register double *f1,*f2 ;

			ptr = mkptr( *psp++ ) ;
			if ( *sp != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
			f1 = ( double * )( ptr + sp[ 1 ] ) ; 
			f2 = ( double * ) sp ; 
			*f2 = *f1 ;
*/
			ptr = mkptr( *psp++ ) ;
			if ( *sp != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
			ptr1 = ( psint * )( &f1 ) ;
			ptr1[ FLOAT1 ] = *( ptr + sp[ 1 ] ) ;
			ptr1[ FLOAT0 ] = *( ptr + sp[ 1 ] + 1 ) ;
			*sp = ptr1[ FLOAT1 ] ;
			sp[ 1 ] = ptr1[ FLOAT0 ] ;
		}
		goto decode ;
	case SUBS_S:
	case SUBS_P: 
		ptr = mkptr( *psp ) ;
		if ( *sp++ != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
		*psp = ptr[ *sp++ ] ;
		goto decode ;
	case SSA_IB: 
		ptr = mkptr( *psp++ ) ; 
		tmp = *sp++ ;
		if ( *sp++ != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
		else ptr[ *sp++ ] = tmp ;
		goto decode ;
	case SSA_R:
		{
/*
			register double *f1,*f2 ;

			ptr = mkptr( *psp++ ) ;
			if ( sp[ 2 ] != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
			f1 = ( double * )( ptr + sp[ 3 ] ) ; 
			f2 = ( double * ) sp ; 
			sp += 4 ; 
			*f1 = *f2 ;
*/
			ptr = mkptr( *psp++ ) ;
			ptr1 = ( psint * )( &f2 ) ;
			ptr1[ FLOAT1 ] = *sp++ ;
			ptr1[ FLOAT0 ] = *sp++ ;
			if ( *sp++ != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
			tmp = *sp++ ;
			ptr[ tmp ] = ptr1[ FLOAT1 ] ;
			ptr[ tmp + 1 ] = ptr1[ FLOAT0 ] ;
		}
		goto decode ;
	case SSA_S:
	case SSA_P: 
		tmp = *psp++ ; 
		ptr = mkptr( *psp++ ) ;
		if ( *sp++ != ( *ptr & B_0_15 ) ) error( STRUCTERR ) ;
		ptr[ *sp++ ] = tmp ;
		goto decode ;
	case MAKEV_IB_2: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr = sf - tmp - 1 ; 
		size = ptr - sp ;
		ptr2 = c_v_ib( *ptr,size + *ptr - 1 ) ;
		for ( ptr3 = ptr2 + size + 2 ; ptr3 != ptr2 + 2 ; )
			*ptr3-- = *sp++ ; 
		*--psp = mkpsptr( ptr2 ) ; 
		sp++ ;
		goto decode ;
	case MAKEV_R_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		ptr = sf - tmp - 1 ;
		size = ptr - sp ;
		ptr2 = c_v_r( *ptr,size / 2 + *ptr - 1 ) ;
		for ( ptr3 = ptr2 + size + 2  ; ptr3!= ptr2 + 2 ; )
		{ 
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp ;
			ptr[ FLOAT0 ] = sp[1] ;
			ptr3[ -1 ] = *sp++ ; 
			*ptr3 = *sp++ ; 
			ptr3 -= 2 ; 
		}
		*--psp = mkpsptr( ptr2 ) ; 
		sp++ ;
		goto decode ;
	case MAKEV_S_2:
	case MAKEV_P_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		size = psf - psp - tmp ;
		tmp = *sp++ ;
		ptr1 = c_v_p( tmp,size + tmp - 1 ) ;
		for ( ptr2 = ptr1 + size + 2 ; ptr2 != ptr1 + 2 ; )
			*ptr2-- = *psp++ ;
		*--psp = mkpsptr( ptr1 ) ;
		goto decode ;
	case ILF_IB_2:		 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		res1 = *sp++ ;
		ptr = ( tmp - 1 ) * 2 + sp ;
		type = INT_BOOL ;
		*--psp = mkpsptr( iliffe( ptr ) ) ;
		sp = ptr + 2 ;
		goto decode ;
	case ILF_R_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		res1 = *sp++ ; 
		res2 = *sp++ ;
		ptr = ( tmp - 1 ) * 2 + sp ;
		type = REAL ;
		*--psp = mkpsptr( iliffe( ptr ) ) ;
		sp = ptr + 2 ;
		goto decode ;
	case ILF_S_2:
	case ILF_P_2:
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |= ( int )( *pc++ ) ;
#else
		tmp = ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		res1 = psb - psp ;
		type = PNTR ;
		ptr = ( tmp - 1 ) * 2 + sp ;
		ptr2 = iliffe( ptr ) ;
		*psp = mkpsptr( ptr2 ) ;
		sp = ptr + 2 ;
		goto decode ;
	case SUBV_IB: 
		ptr = mkptr( *psp++ ) ;
		if ( *sp < ptr[ 1 ] || *sp > ptr[ 2 ] )
			err( VM1,VM2,VM3,*sp,ptr[ 1 ],ptr[ 2 ] ) ;
		*sp = ptr[  *sp - ptr[ 1 ] + 3 ] ;
		goto decode ;
	case SUBV_R:
		{
/*
			register int *v,index ;
			register double *f1,*f2 ;

			index = *sp-- ;
			v = mkptr( *psp++ ) ;
			if ( index < v[ 1 ] || index > v[ 2 ] )
				err( VM1,VM2,VM3,index,v[ 1 ],v[ 2 ] ) ;
			f2 = ( double * ) sp ;
			f1 = ( double * ) ( v + 3 ) ;
			*f2 = f1[ index - v[ 1 ] ] ;
*/
			register int *v,index ;

			index = *sp-- ;
			v = mkptr( *psp++ ) ;
			if ( index < v[ 1 ] || index > v[ 2 ] )
				err( VM1,VM2,VM3,index,v[ 1 ],v[ 2 ] ) ;
			ptr1 = v + 3 ;
			ptr = ( psint *)( &f1 ) ;
			index -= v[ 1 ] ; index <<= 1 ;
			ptr[ FLOAT1 ] = ptr1[ index ] ;
			ptr[ FLOAT0 ] = ptr1[ index + 1 ] ;
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			*sp = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case SUBV_S:
	case SUBV_P: 
		ptr = mkptr( *psp ) ;
		index = *sp++ ;
		if ( index < ptr[ 1 ] || index > ptr[ 2 ] )
			err( VM1,VM2,VM3,index,ptr[ 1 ],ptr[ 2 ] ) ;
		*psp = ptr[ index - ptr[ 1 ] + 3 ] ;
		goto decode;
	case SVA_IB: 
		ptr = mkptr( *psp++ ) ; 
		tmp = *sp++ ;
		index = *sp++ ;
		if ( index < ptr[ 1 ] || index > ptr[ 2 ] )
			err( VM1,VM2,VM3,index,ptr[ 1 ],ptr[ 2 ] ) ;
		ptr[ index - ptr[ 1 ] + 3 ] = tmp ;
		goto decode ;
	case SVA_R:
		{
/*
			register int *v,index ;
			register double *f1,*f2 ;

			v =  mkptr( *psp++ ) ; 
			index = sp[ 2 ] ;
			if ( index < v[ 1 ] || index > v[ 2 ] )
				err( VM1,VM2,VM3,index,v[ 1 ],v[ 2 ] ) ;
			f2 = ( double * ) sp ; 
			sp += 3 ;
			f1 = ( double * ) ( v + 3 ) ;
			f1[ index - v[ 1 ] ] = *f2 ;
*/
			register int *v,index ;

			v =  mkptr( *psp++ ) ; 
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			index = *sp++ ;
			if ( index < v[ 1 ] || index > v[ 2 ] )
				err( VM1,VM2,VM3,index,v[ 1 ],v[ 2 ] ) ;
			ptr1 = v + 3 ;
			index -= v[ 1 ] ; index <<= 1 ;
			ptr1[ index ] = ptr[ FLOAT1 ] ;
			ptr1[ index + 1 ] = ptr[ FLOAT0 ] ;
		}
		goto decode ;
	case SVA_S:
	case SVA_P: 
		tmp = *psp++ ; 
		ptr = mkptr( *psp++ ) ;
		index = *sp++ ;
		if ( index < ptr[ 1 ] || index > ptr[ 2 ] )
			err( VM1,VM2,VM3,index,ptr[ 1 ],ptr[ 2 ] ) ;
		ptr[ index - ptr[ 1 ] + 3 ] = tmp ;
		goto decode ;
	case UPB_OP:
		ptr = mkptr( *psp++ ) ; 
		*--sp = ptr[ 2 ] ;
		goto decode ;
	case LWB_OP:
		ptr = mkptr( *psp++ ) ; 
		*--sp = ptr[ 1 ] ;
		goto decode ;
	case CONCAT_OP: 
		{
			register char *res,*c1 ; 
			register int *s1,*s2 ;

			tmp = *psp ; tmp1 = ptr[ 1] ;
			s1 = mkptr( *psp ) ;
/*oops bad test - surely s1 == nilstr */
			if ( !mkpsptr( s1 ) ) psp++ ;
			else
			{
				s2 = mkptr( psp[ 1 ] ) ;
				if ( !mkpsptr( s2 ) ) *++psp = mkpsptr( s1 ) ;
				else
				{
					extern int *c_string() ;

					tmp = *s1 & B_0_15 ; 
					tmp1 = *s2 & B_0_15 ;
					res = ( char * ) ( c_string( tmp + tmp1 ) ) ;
					c1 = ( char * )( mkptr( psp[ 1 ] ) ) + 4 ;
					*++psp = mkpsptr( ( int * ) res ) ; 
					res += 4 ;
					while ( tmp1-- ) *res++ = *c1++ ;
					c1 = ( char * )( mkptr( psp[ -1 ] ) ) + 4 ;
					while ( tmp-- ) *res++ = *c1++ ;
				}
			}
		}
		goto decode ;
	case SUBSTR_OP: 
		{
			register int start,length,size,*s1 ;

			length = *sp++ ; 
			start = *sp++ ; 
			s1 = mkptr( *psp ) ;
			if ( !mkpsptr( s1 ) ) error( "Indexing on empty string" ) ;
			size = *s1++ & B_0_15 ;
			if ( start < 1 || start > size || start + length - 1 > size || length < 0 )
				err( SM1,SM2,SM3,size,start,length ) ;

			if ( !length ) *psp = 0 ;	/* nil string */
			else
				if ( length == 1 )
				{
					register char *c ; 
					register int ch ;

					c = ( char * ) s1 ; 
					ch = ( int )( c[ start - 1 ] ) ;
					*psp = mkpsptr( char_tab + ch * 2 ) ;
				}
				else
				{
					register char *c1,*c2 ;
					extern int *c_string() ;

					c1 = ( char * ) ( c_string( length ) ) ;
					c2 = ( char * )( mkptr( *psp ) ) ; 
					c2 += start + 3 ;

					*psp = mkpsptr( ( int * ) c1 ) ; 
					c1 += 4 ;
					while ( length-- ) *c1++ = *c2++ ;
				}
		}
		goto decode ;
	case LD_TM_5: 
		*--sp = ( int )( *pc++ ) ;
#ifdef PERQ
		tmp1 =  ( int )( *pc++ ) << 16 ; 
		tmp1 |= ( int )( *pc++ ) << 24 ; 
		tmp1 |= ( int )( *pc++ ) ; 
		tmp1 |= ( int )( *pc++ ) << 8 ; 
#else
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 24 ; 
		tmp1 |=  ( int )( *pc++ ) << 16 ; 
		tmp1 |= ( int )( *pc++ ) << 8 ; 
		tmp1 |= ( int )( *pc++ ) ; 
#else
#ifdef	ALPHA
		tmp1 = ( int )( *pc++ )  ;
		tmp1 |=  ( int )( *pc++ ) << 8 ; 
		tmp1 |= ( int )( *pc++ ) << 16 ; 
		tmp1 |= ( int )( *pc++ ) << 24 ; 
#else
		tmp1 = *( ( int * ) pc ) ;
		pc += 4 ;
#endif	ALPHA
#endif	M68000
#endif	PERQ
#ifdef	M68000
		*sp += ( ( int )( pc[ tmp1++ ] ) & B_0_6 ) << 8 ;
		*sp += ( int )( pc[ tmp1 ] ) ;
#else
		*sp += ( int )( pc[ tmp1++ ] ) ;
		*sp += ( ( int )( pc[ tmp1 ] ) & B_0_6 ) << 8 ;
#endif	M68000
		goto decode ;
	case LD_TM_6: 
#ifdef	M68000
		tmp = ( int )( *pc++ ) << 8 ;
		tmp |=  ( int )( *pc++ ) ;
#else
		tmp =  ( int )( *pc++ ) ;
		tmp |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		*--sp = tmp ;
#ifdef PERQ
		tmp1 =  ( int )( *pc++ ) << 16 ; 
		tmp1 |= ( int )( *pc++ ) << 24 ; 
		tmp1 |= ( int )( *pc++ ) ; 
		tmp1 |= ( int )( *pc++ ) << 8 ; 
#else
#ifdef	M68000
		tmp1 = ( int )( *pc++ ) << 24 ; 
		tmp1 |=  ( int )( *pc++ ) << 16 ; 
		tmp1 |= ( int )( *pc++ ) << 8 ; 
		tmp1 |= ( int )( *pc++ ) ; 
#else
#ifdef	ALPHA
		tmp1 = ( int )( *pc++ )  ;
		tmp1 |=  ( int )( *pc++ ) << 8 ; 
		tmp1 |= ( int )( *pc++ ) << 16 ; 
		tmp1 |= ( int )( *pc++ ) << 24 ; 
#else
		tmp1 = *( ( int * ) pc ) ;
		pc += 4 ;
#endif	ALPHA
#endif	M68000
#endif	PERQ
#ifdef	M68000
		*sp += ( ( int )( pc[ tmp1++ ] ) & B_0_6 ) << 8 ;
		*sp += ( int )( pc[ tmp1 ] ) ;
#else
		*sp += ( int )( pc[ tmp1++ ] ) ;
		*sp += ( ( int )( pc[ tmp1 ] ) & B_0_6 ) << 8 ;
#endif	M68000
		goto decode ;

		/*********** LOAD LITERAL INSTRUCTIONS ***********/

	case LL_INT_1: 
		*--sp = ( int )( *pc++ ) ;
		if ( *sp & B_7 ) *sp |= B_8_31 ;
		goto decode ;
	case LL_INT_4: 
#ifdef PERQ
		tmp =  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 24 ; 
		tmp |=  ( int )( *pc++ ) ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		*--sp = tmp ;
#else
#ifdef	M68000
		tmp =  ( int )( *pc++ ) << 24 ; 
		tmp |=  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		tmp |=  ( int )( *pc++ ) ; 
		*--sp = tmp ;
#else
#ifdef	ALPHA
		tmp =  ( int )( *pc++ ) ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		tmp |=  ( int )( *pc++ ) << 16; 
		tmp |=  ( int )( *pc++ ) << 24 ; 
		*--sp = tmp ;
#else
		*--sp = *( int * )( pc ) ; 
		pc += 4 ;
#endif	ALPHA
#endif	M68000
#endif	PERQ
		goto decode ;
	case LL_BOOL_1: 
		*--sp = ( *pc++ ? SFALSE : STRUE ) ;
		goto decode ;
	case LL_REAL_8:
#ifdef PERQ
		tmp =  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 24 ; 
		tmp |=  ( int )( *pc++ ) ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		sp[ -2 ] = tmp ;
		tmp =  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 24 ; 
		tmp |=  ( int )( *pc++ ) ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		sp[ -1 ] = tmp ;
		sp -= 2 ;
#else
#ifdef	M68000
		tmp =  ( int )( *pc++ ) << 24 ; 
		tmp |=  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		tmp |=  ( int )( *pc++ ) ; 
		sp[ -2 ] = tmp ;
		tmp =  ( int )( *pc++ ) << 24 ; 
		tmp |=  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		tmp |=  ( int )( *pc++ ) ; 
		sp[ -1 ] = tmp ;
		ptr = ( psint * )( &f1 ) ;
		ptr[ FLOAT1 ] = sp[ -2 ] ;
		ptr[ FLOAT0 ] = sp[ -1 ] ;
		sp -= 2 ; 
#else
#ifdef	ALPHA
		tmp =  ( int )( *pc++ ) ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		tmp |=  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 24 ; 
		sp[ -2 ] = tmp ;
		tmp =  ( int )( *pc++ ) ; 
		tmp |=  ( int )( *pc++ ) << 8 ; 
		tmp |=  ( int )( *pc++ ) << 16 ; 
		tmp |=  ( int )( *pc++ ) << 24 ; 
		sp[ -1 ] = tmp ;
		sp -= 2 ;
#else
		sp -= 2 ;
		*( ( double * ) sp ) = *( ( double * ) pc ) ;
		pc += 8 ; 
#endif	ALPHA
#endif	M68000
#endif	PERQ
		goto decode ;
	case LL_STR_1:
		pc = ( unsigned char * )( ( ( long ) pc + 3 ) & ~B_0_1 ) ; /* get alignment right */
		*--psp = mkpsptr ( ( int * ) pc ) ;
		pc += ( *( ( int * ) pc ) & B_0_15 ) + 4 ;

		goto decode ;
	case LL_CHAR_1: 
		tmp = ( int )( char_tab + 2 * ( ( int )*pc ) ) ;
		*--psp = mkpsptr( char_tab + 2 * ( ( int )( *pc++ ) ) ) ;
		goto decode ;
	case LL_FILE: 
		*--psp = 0 ;
		goto decode ;
	case LL_NIL_STRING: 
		*--psp = 0 ;
		goto decode ;
	case LL_N_P: 
		*--psp = mkpsptr( NIL_PTR ) ;
		goto decode ;

		/*********** COMPARISON INSTRUCTIONS ***********/

	case EQ_IB: 
		tmp = *sp++ ; 
		*sp = ( tmp == *sp ) ;
		goto decode ;
	case EQ_R:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			sp += 3 ;
			*sp = ( *fptr == fptr[ 1 ] ) ;
*/
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			*--sp = f1 == f2 ;
		}
		goto decode ;
	case EQ_S: 
		ptr = mkptr( *psp++ ) ;
		ptr1 = mkptr( *psp++ ) ;
		*--sp = eq_string( ptr,ptr1 ) ;
		goto decode ;
	case EQ_P: 
		tmp = *psp++ ; 
		*--sp = ( tmp == *psp++ ) ;
		goto decode ;
	case NEQ_IB: 
		tmp = *sp++ ; 
		*sp = ( tmp != *sp ) ;
		goto decode ;
	case NEQ_R:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			sp += 3 ;
			*sp = *fptr != fptr[ 1 ] ;
*/
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			*--sp = f1 != f2 ;
		}
		goto decode ;
	case NEQ_S: 
		ptr = mkptr( *psp++ ) ;
		ptr1 = mkptr( *psp++ ) ;
		*--sp = !( eq_string( ptr,ptr1 ) ) ;
		goto decode ;
	case NEQ_P: 
		tmp = *psp++ ; 
		*--sp = ( tmp != *psp++ ) ;
		goto decode ;
	case LT_I: 
		tmp = *sp++ ; 
		*sp = ( tmp > *sp ) ;
		goto decode ;
	case LT_R:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			sp += 3 ;
			*sp = *fptr > fptr[ 1 ] ;
*/
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			*--sp = f1 < f2 ;
		}
		goto decode ;
	case LT_S: 
		ptr = mkptr( *psp++ ) ; 
		*--sp = lt_string( mkptr( *psp++ ),ptr ) ;
		goto decode ;
	case LE_I: 
		tmp = *sp++ ; 
		*sp = ( tmp >= *sp ) ;
		goto decode ;
	case LE_R:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			sp += 3 ;
			*sp = *fptr >= fptr[ 1 ] ;
*/
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			*--sp = f1 <= f2 ;
		}
		goto decode ;
	case LE_S: 
		ptr2 = mkptr( *psp++ ) ; 
		ptr1 = mkptr( *psp++ ) ;
		*--sp = ( lt_string( ptr1,ptr2 ) || eq_string( ptr1,ptr2 ) ) ;
		goto decode ;
	case GT_I: 
		tmp = *sp++ ; 
		*sp = ( tmp < *sp ) ;
		goto decode ;
	case GT_R:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			sp += 3 ;
			*sp = *fptr < fptr[ 1 ] ;
*/
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			*--sp = f1 > f2 ;
		}
		goto decode ;
	case GT_S: 
		ptr = mkptr( *psp++ ) ; 
		*--sp = lt_string( ptr,mkptr( *psp++ ) ) ;
		goto decode ;
	case GE_I: 
		tmp = *sp++ ; 
		*sp = ( tmp <= *sp ) ;
		goto decode ;
	case GE_R:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			sp += 3 ;
			*sp = ( *fptr <= fptr[ 1 ] ) ;
*/
			ptr = ( psint * )( &f2 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			ptr = ( psint * )( &f1 ) ;
			ptr[ FLOAT1 ] = *sp++ ;
			ptr[ FLOAT0 ] = *sp++ ;
			*--sp = f1 >= f2 ;
		}
		goto decode ;
	case GE_S: 
		ptr = mkptr( *psp++ ) ; 
		ptr1 = mkptr( *psp++ ) ;
		*--sp = ( lt_string( ptr,ptr1 ) || eq_string( ptr,ptr1 ) ) ;
		goto decode ;

		/*********** ARITHMETIC OPERATORS ***********/

	case PLUS: 
		{ 
			tmp = *sp++ ; 
/*
			*sp = ILintAdd( *sp,tmp ) ; 
*/
			*sp = *sp + tmp ;
		}
		goto decode ;
	case TIMES: 
		{ 
			tmp = *sp++ ; 
/*
			*sp = ILintTimes( *sp,tmp ) ; 
*/
			*sp = *sp * tmp ;
		}
		goto decode ;
	case MINUS: 
		{ 
			tmp = *sp++ ; 
/*
			*sp = ILintMinus( *sp,tmp ) ; 
*/
			*sp = *sp - tmp ;
		}
		goto decode ;
	case DIV: 
		{ 
			tmp = *sp++ ; 
			*sp /= tmp ; 
		}
		goto decode ;
	case REM: 
		{ 
			tmp = *sp++ ; 
			*sp %= tmp ; 
		}
		goto decode ;
	case NEG: 
		{
/*
			*sp = ILintMinus( 0,*sp ) ;
*/
			*sp = 0 - *sp ;
		}
		goto decode ;
	case FPLUS:
		{
/*
			register double *fptr,f1 ;

			fptr = ( double * ) sp ; 
			sp += 2 ;
			f1 = *fptr++ ; 
			*fptr += f1 ;
*/
			psreal	result ;

			ptr = ( psint * ) &f2 ;          /* copy stack top to rhs - ensures psreal is aligned correctly */
			ptr[ FLOAT1 ] = sp[ 0 ] ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr = ( psint * ) &f1 ;          /* ditto for lhs */
			ptr[ FLOAT1 ] = sp[ 2 ] ;
			ptr[ FLOAT0 ] = sp[ 3 ] ;
			result = f1 + f2 ;                       /* perform the op */

			/* Check for overflow !! */
			sp += 2 ;
			ptr = ( psint * ) &result ;  /* copy res to top of stack - res is correctly aligned result */
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			sp[ 0 ] = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case FTIMES:
		{
/*
			register double *fptr,f1 ;

			fptr = ( double * ) sp ; 
			sp += 2 ;
			f1 = *fptr++ ; 
			*fptr *= f1 ;
*/
			psreal	result ;

			ptr = ( psint * ) &f2 ;          /* copy stack top to rhs - ensures psreal is aligned correctly */
			ptr[ FLOAT1 ] = sp[ 0 ] ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr = ( psint * ) &f1 ;          /* ditto for lhs */
			ptr[ FLOAT1 ] = sp[ 2 ] ;
			ptr[ FLOAT0 ] = sp[ 3 ] ;
			result = f1 * f2 ;                       /* perform the op */

			/* Check for overflow !! */
			sp += 2 ;
			ptr = ( psint * ) &result ;  /* copy result to top of stack - result is correctly aligned result */
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			sp[ 0 ] = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case FMINUS:
		{
/*
			register double *fptr,f1 ;

			fptr = ( double * ) sp ; 
			sp += 2 ;
			f1 = *fptr++ ; 
			*fptr -= f1 ;
*/
			psreal	result ;

			ptr = ( psint * ) &f2 ;          /* copy stack top to rhs - ensures psreal is aligned correctly */
			ptr[ FLOAT1 ] = sp[ 0 ] ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr = ( psint * ) &f1 ;          /* ditto for lhs */
			ptr[ FLOAT1 ] = sp[ 2 ] ;
			ptr[ FLOAT0 ] = sp[ 3 ] ;
			result = f1 - f2 ;                       /* perform the op */

			/* Check for overflow !! */
			sp += 2 ;
			ptr = ( psint * ) &result ;  /* copy result to top of stack - result is correctly aligned result */
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			sp[ 0 ] = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case FDIVIDE:
		{
/*
			register double *fptr,f1 ;

			fptr = ( double * ) sp ; 
			sp += 2 ;
			f1 = *fptr++ ; 
			*fptr /= f1 ;
*/
			psreal	result ;

			ptr = ( psint * ) &f2 ;          /* copy stack top to rhs - ensures psreal is aligned correctly */
			ptr[ FLOAT1 ] = sp[ 0 ] ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			ptr = ( psint * ) &f1 ;          /* ditto for lhs */
			ptr[ FLOAT1 ] = sp[ 2 ] ;
			ptr[ FLOAT0 ] = sp[ 3 ] ;
			result = f1 / f2 ;                       /* perform the op */

			/* Check for overflow !! */
			sp += 2 ;
			ptr = ( psint * ) &result ;  /* copy result to top of stack - result is correctly aligned result */
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			sp[ 0 ] = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case NOT_OP: 
		*sp = !( *sp ) ;
		goto decode ;
	case FNEG:
		{
/*
			register double *fptr ;

			fptr = ( double * ) sp ; 
			*fptr = -*fptr ;
*/
			psreal	result ;

			ptr = ( psint * ) &f2 ;          /* copy stack top to rhs - ensures psreal is aligned correctly */
			ptr[ FLOAT1 ] = sp[ 0 ] ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			result = -( f2 ) ;                       /* perform the op */

			/* Check for overflow !! */
			ptr = ( psint * ) &result ;  /* copy result to top of stack - result is correctly aligned result */
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			sp[ 0 ] = ptr[ FLOAT1 ] ;
		}
		goto decode ;
	case FLOATING1:
		{
/*
			register double *f ;

			f = ( double * )( --sp ) ;
			*f = sp[ 1 ] ;
*/
			psreal	result ;

			result = ( psreal )( *sp ) ;
			ptr = ( psint * )&result ;
			*sp = ptr[ FLOAT0 ] ;
			*--sp = ptr[ FLOAT1 ] ;

		}
		goto decode ; 
	case FLOATING2:
		{
/*
			register double *f1,*f2 ;

			f1 = ( double * )( sp-- ) ;
			f2 = ( double * ) sp ;
			*f2 = *f1 ;
*/

			f1 = ( psreal )( sp[ 2 ] ) ; /* The integer 2nd on the stack gets co-erced */
			ptr = ( psint * ) &f2 ; 
			ptr[ FLOAT1 ] = *sp ;
			ptr[ FLOAT0 ] = sp[ 1 ] ;
			sp-- ;
			sp[ 1 ] = ptr[ FLOAT0 ] ;
			*sp = ptr[ FLOAT1 ] ;
			ptr = ( psint * ) &f1 ; 
			sp[ 3 ] = ptr[ FLOAT0 ] ;
			sp[ 2 ]  = ptr[ FLOAT1 ] ;
		}
		goto decode ;

		/*********** STACK MANIPULATION, TERIMATION AND I/O ***********/

	case ERASE_IB: 
		sp++ ;
		goto decode ;
	case ERASE_R: 
		sp += 2 ;
		goto decode ;
	case ERASE_S:
	case ERASE_P: 
		psp++ ;
		goto decode ;
	case REV_MS: 
		tmp = *sp ; 
		*sp = sp[ 1 ] ; 
		sp[ 1 ] = tmp ;
		goto decode ;
	case REV_PS: 
		tmp = *psp ; 
		*psp = psp[ 1 ] ; 
		psp[ 1 ] = tmp ;
		goto decode ;
	case NLNE_1:
		line_no = ( int )( *pc++ ) ;
		if ( flow_sum ) flow_vec[ line_no ] += 1 ;
		goto decode ;
	case NLNE_2:
#ifdef	M68000
		line_no = ( int )( *pc++ ) << 8 ;
		line_no |= ( int )( *pc++ ) ;
#else
		line_no = ( int )( *pc++ ) ;
		line_no |= ( int )( *pc++ ) << 8 ;
#endif	M68000
		if ( flow_sum ) flow_vec[ line_no ] += 1 ;
		goto decode ;
	case FINISH_OP: 
	case ABORT_OP:
		break ;
	case READ_OP: 
		switch ( *pc++ )
		{
		case 0: 
			r_int() ;
			goto decode ;
		case 1: 
			r_string() ;
			goto decode ;
		case 2: 
			r_bool() ;
			goto decode ;
		case 3: 
			r_byte() ;
			goto decode ;
		case 4: 
			peek() ;
			goto decode ;
		case 5: 
			readc() ;
			goto decode ;
		case 6: 
			eof() ;
			goto decode ;
		case 7: 
			r_name() ;
			goto decode ;
		case 8: 
			r_line() ;
			goto decode ;
		case 9: 
			r_16() ;
			goto decode ;
		case 10: 
			r_32() ;
			goto decode ;
		default:
			error( "Unknown read op" ) ;
		}
	case WRITE_OP: 
		switch( *pc++ )
		{
		case 0:
			w_int() ;
			goto decode ;
		case 1:
			w_string() ;
			goto decode ;
		case 2:
			w_bool() ;
			goto decode ;
		case 3:
			out_byte() ;
			goto decode ;
		case 4:
			out_16() ;
			goto decode ;
		case 5:
			out_32() ;
			goto decode ;
		default:
			error( "Unknown write op" ) ;
		}
#ifdef	RASTER
        /* Raster graphics instructions */
 
        case R_COPY:
#ifdef  RASTOP
                rastop( 0 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_NOT:
#ifdef  RASTOP
                rastop( 1 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_AND:
#ifdef  RASTOP
                rastop( 2 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_NAND:
#ifdef  RASTOP
                rastop( 3 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_ROR:
#ifdef  RASTOP
                rastop( 4 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_NOR:
#ifdef  RASTOP
                rastop( 5 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_XOR:
#ifdef  RASTOP
                rastop( 6 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case R_XNOR:
#ifdef  RASTOP
                rastop( 7 ) ;
#else
                psp += 2 ;
#endif  RASTOP
                goto decode ;
        case SUB_IMAGE:
#ifdef  RASTOP
                tmp1 = *sp++ ;                        /* no. of planes */
                tmp2 = *sp++ ;                        /*  start plane  */
                ptr1 = mkptr( ( mkptr( *psp ) )[ 1 ] ) ;   /* ptr1
is vector of bitmaps for old image */
 
                                                        /* check if plane selection is valid */
                if( tmp2 < ( psint ) 0 || tmp2 >= ptr1[ 2 ] || tmp1 < ( psint )
1 || tmp1 + tmp2 > ptr1[ 2 ] )
                        sim_error( ptr1[ 2 ],tmp2,tmp1 ) ;
                                                        /* if selecting all the
planes - nothing to do */
                if ( tmp1 == ptr1[ 2 ] ) goto decode ;
 
                /* plane selection is legal and needs a new image descriptor */
                                                        /* first create a new image descriptor */
                ptr1 = c_image() ;        /* 7 - length image desc       */
                ptr2 = mkptr( *psp ) ;           /* ptr2 is old image
*/
                *psp = mkpsptr(  ptr1 ) ;                        /* put ptr1 on psp      */
                ptr1[ 1 ] = ( psint ) 0 ;                               /* temp. bitmap pointer */
                ptr1[ 2 ] = ptr2[ 2 ] ;                 /* copy file descriptor
*/
                ptr1[ 3 ] = ptr2[ 3 ] ;                 /*       x offset
*/
                ptr1[ 4 ] = ptr2[ 4 ] ;                 /*       y offset
*/
                ptr1[ 5 ] = ptr2[ 5 ] ;                 /*      x dimension
*/
                ptr1[ 6 ] = ptr2[ 6 ] ;                 /*      y dimension
*/
 
                ptr3 = c_v_p( 1,tmp1 ) ;  /* 3 - vector header + planes  */
                ptr1[ 1 ] = mkpsptr( ptr3 ) ;            /* copy bitmap pointer
*/
 
                /* next copy the pointers to the planes from old to new vector of bitmaps */
 
                ptr1 = mkptr( ptr1[ 1 ] ) + 3 ;           /* destination of the copy */
                ptr2 = mkptr( ptr2[ 1 ] ) + ( psint ) 3 + tmp2 ;  /* source of the copy */
                while( tmp1-- > ( psint ) 0 ) *ptr1++ = *ptr2++ ;
#else
                sp += 2 ;     /* dump the parameters */
#endif  RASTOP
                goto decode ;
        case SUB_PIXEL:
#ifdef	RASTOP
                tmp1 = *sp++ ;                /* tmp1 is the index */
                tmp2 = *sp ;                  /* tmp2 is the pixel */
                if( tmp1 >= ( ( tmp2 >> ( psint ) 24 ) & B_0_7 ) || tmp1 < ( psint ) 0 )
                        pix_error( ( tmp2 >> ( psint ) 24 ) & B_0_7,tmp1 ) ;
                *sp = EXTRACT_BIT( tmp1,tmp2 ) | ( ( psint ) 1 << ( psint ) 24 ) ;
#else
		sp += 1 ;
		*sp = 0 ;
#endif	RASTOP
                goto decode ;
        case MK_IMAGE:
#ifdef  RASTOP
                mk_image() ;
#else
                sp += 3 ;     /* dump dimension parameters */
                *--psp = 0 ;
#endif  RASTOP
                goto decode ;
        case MK_PIXEL:
                {
                        tmp1 = ( psint )( *pc++ ) ;      /*      no pixels on ms        */
                        ptr1 = sp + tmp1  ;             /* ptr1 points at first pixel  */
                        sp = ptr1 ;                    /*   final position of
pixel   */
                        tmp2 = *--ptr1 ;                 /*      tmp2 is new pixel      */
                        tmp3 = ( tmp2 >> ( psint ) 24 ) & B_0_7 ; /* tmp3 how many planes copied */
                        while( --tmp1 != ( psint ) 0 )           /*   copy pixles from stack    */
                        {
                                tmp2 |= *--ptr1 << tmp3 ;
                                tmp3 += ( *ptr1 >> ( psint ) 24 ) & B_0_7 ;
                        }
                        if ( tmp3 > ( psint ) 24 ) error( "pixel depth overflow ( max 24 )" ) ;
                        *--sp = ( tmp3 << ( psint ) 24 ) | ( tmp2 & B_0_23 )
;
                }
                goto decode ;
        case LIMIT_2:
#ifdef  RASTOP
                /* first create a new image descriptor */
                ptr1 = c_image() ;         		/* 7 - length image desc */
                                                        /* now sort out stray pointers */
                ptr2 = mkptr( *psp ) ;           /* pointer to an image desc */
                *psp = mkpsptr( ptr1 ) ;                        /* overwrite new image to top of p stack */
                ptr1[ 1 ] = ptr2[ 1 ] ;                 /* ptr1 is the new image */
                ptr1[ 2 ] = ptr2[ 2 ] ;                 /* ptr2 is the old image *
                                                        /* finally fill in fields of new desc */
 
                tmp1 = *sp ;                            /* y offset */
                tmp2 = ptr2[ 6 ] - tmp1 ;               /* y dimension */
                ptr1[ 6 ] = tmp2 ;                      /* y dimension */
                ptr1[ 4 ] = tmp1 + ptr2[ 4 ] ;          /* y offset */
                if ( tmp1 < ( psint ) 0 || tmp2 <= ( psint ) 0 )
                        lim1_error( "Error in Y limit",tmp1,tmp2 + tmp1 ) ;
                        tmp1 = sp[ 1 ] ;                     /* x offset */
                        tmp2 = ptr2[ 5 ] - tmp1 ;               /* x dimension */
                        ptr1[ 5 ] = tmp2 ;                      /* x dimension */
                        ptr1[ 3 ] = tmp1 + ptr2[ 3 ] ;          /* x offset */
                        if ( tmp1 < ( psint ) 0 || tmp2 <= ( psint ) 0 )
                        lim1_error( "Error in X limit",tmp1,tmp2 + tmp1 ) ;
#endif  RASTOP
                sp += 2 ;
                goto decode ;
        case LIMIT_4:
#ifdef  RASTOP
                /* first create a new image descriptor */
                ptr1 = c_image() ;			 /* 7 - length image desc */
                /* now sort out stray pointers */
                ptr2 = mkptr( *psp ) ;   /* pointer to an image desc */
                *psp = mkpsptr( ptr1 ) ;
                ptr1[ 1 ] = ptr2[ 1 ] ;
                ptr1[ 2 ] = ptr2[ 2 ] ;
                /* finally fill in fields of new desc */
 
                tmp1 = *sp ;                  /* y offset */
                tmp2 = sp[ 2 ] ;              /* y dimension */
                ptr1[ 6 ] = tmp2 ;
                ptr1[ 4 ] = tmp1 + ptr2[ 4 ] ;
                if ( tmp1 < ( psint ) 0 || tmp2 <= ( psint ) 0 || tmp1 + tmp2 >
ptr2[ 6 ] )
                        lim2_error( "Error in Y limit",tmp1,tmp2,ptr2[ 6 ] ) ;
                        tmp1 = sp[ 1 ] ;             /* x offset */
                        tmp2 = sp[ 3 ] ;             /* x dimension */
                        ptr1[ 5 ] = tmp2 ;
                        ptr1[ 3 ] = tmp1 + ptr2[ 3 ] ;
                        if ( tmp1 < ( psint ) 0 || tmp2 <= ( psint ) 0 || tmp1 + tmp2 > ptr2[ 5 ] )
                        lim2_error( "Error in X limit",tmp1,tmp2,ptr2[ 5 ] ) ;
#endif  RASTOP
                sp += 4 ;
                goto decode ;

#endif	RASTER
	default: 
		error( "Unknown op code" ) ;
	}
	if ( flow_sum) flow_info() ;
	tidy_up() ;
#ifdef	DEBUG
	{
		char *s = "            " ;
		sprintf( s,"%d",gc_count ) ;
		( void ) write_error( 2,"\nGarbage collections = ",23 ) ;
		( void ) write_error( 2,&s[0],12 ) ;
		( void ) write_error( 2,"\n",1 ) ;
	}
#endif	DEBUG
	Exit( 0 ) ;
}

error( s )
register char *s ;
{
	int size ; 
	register char *c ;
	extern char *i_to_s() ;

	extern command() ;

	tidy_up() ;
	( void ) write_error( 2,"\nRun-time error at line ",24 ) ;
	c = i_to_s( &size,line_no ) ;
	( void ) write_error( 2,c,size ) ;
	Error( s ) ;
}

Error( s )
register char *s ;
{
	register int size ; 
	register char *c ;

#ifdef	WEB
	extern	int	tmpf ;
#endif	/*WEB*/
#ifdef	RASTOP
	extern	reset_input() ;

	reset_input() ;
#endif	RASTOP
	c = s ; 
	size = 0 ;
	while ( *c++ ) size++ ;
	( void ) write_error( 2,"\n",1 ) ;
	( void ) write_error( 2,s,size ) ;
	( void ) write_error( 2,"\n",1 ) ;
#ifdef	WEB
	( void ) write_error( tmpf,"\n",1 ) ;
	( void ) write_error( tmpf,s,size ) ;
	( void ) write_error( tmpf,"\n",1 ) ;
#endif	/*WEB*/
	if ( inited ) trace( SFALSE )  ;
#ifdef	DEBUG
	{
		char *s = "            " ;
		sprintf( s,"%d",gc_count ) ;
		( void ) write_error( 2,"\nGarbage collections = ",23 ) ;
		( void ) write_error( 2,&s[0],12 ) ;
		( void ) write_error( 2,"\n",1 ) ;
	}
#endif	DEBUG
	Exit( -1 ) ;
}

err( m1,m2,m3,i1,i2,i3 )
char *m1,*m2,*m3 ; 
int i1,i2,i3 ;
{
	register char *c1,*c2 ; 
	int start,size ;
	extern char *i_to_s() ;

	c1 = work_space + 30 ; 
	start = (int ) c1 ;
	c2 = m1 ;
	while ( *c1++ = *c2++ ) ;
	c2 = i_to_s( &size,i1 ) ; 
	c1-- ;
	while ( size-- ) *c1++ = *c2++ ;
	c2 = m2 ;
	while ( *c1++ = *c2++ ) ;
	c2 = i_to_s( &size,i2 ) ; 
	c1-- ;
	while ( size-- ) *c1++ = *c2++ ;
	c2 = m3 ;
	while ( *c1++ = *c2++ ) ;
	c2 = i_to_s( &size,i3 ) ; 
	c1-- ;
	while ( size-- ) *c1++ = *c2++ ;
	*c1 = 0 ;

	error( ( char * ) start ) ;
}

#ifdef	RASTOP
pix_error( depth,index )        /* sub pixel  error */
psint depth,index  ;
{
        psint sz ;
	extern char *i_to_s() ;
#ifdef  SUN
        extern reset_input() ;

        reset_input() ;
#endif  SUN

        if ( !inerror )
        {
                inerror = STRUE ;

                error_heading( "Sub pixel error" ) ;
                put_error( "\npixel depth = " ) ;
                put_error( i_to_s( &sz,depth ) ) ;
                put_error( "\npixel index = " ) ;
                put_error( i_to_s( &sz,index ) ) ;
                put_error( "\n" ) ;
                if ( inited ) tidy_up() ;
                trace( SFALSE ) ;
        }
        Exit( -1 ) ;
}
 
lim1_error( s,off,rdim )
char *s ; psint off,rdim ;
{
        psint sz ;
	extern char *i_to_s() ;
#ifdef  SUN
        extern reset_input() ;

        reset_input() ;
#endif  SUN

        if ( !inerror )
        {
                inerror = STRUE ;

                error_heading( s ) ;
                put_error( "\nnew offset = " ) ;
                put_error( i_to_s( &sz,off ) ) ;
                put_error( "\ndimension of image = " ) ;
                put_error( i_to_s( &sz,rdim ) ) ;
                put_error( "\n" ) ;
                if ( inited ) tidy_up() ;
                trace( SFALSE ) ;
        }
        Exit( -1 ) ;
}

sim_error( im_len,sim_st,sim_len )      /* sum image error */
psint im_len,sim_st,sim_len ;
{
        psint sz ;
	extern char *i_to_s() ;
#ifdef  SUN
        extern reset_input() ;

        reset_input() ;
#endif  SUN

        if ( !inerror )
        {
                inerror = STRUE ;

                error_heading( "Sub image error" ) ;
                put_error( "\nimage depth = " ) ;
                put_error( i_to_s( &sz,im_len ) ) ;
                put_error( "\nplane start = " ) ;
                put_error( i_to_s( &sz,sim_st ) ) ;
                put_error( "\nno. of planes = " ) ;
                put_error( i_to_s( &sz,sim_len ) ) ;
                put_error( "\n" ) ;
                if ( inited ) tidy_up() ;
                trace( SFALSE ) ;
        }
        Exit( -1 ) ;
}

lim2_error( s,off,dim,rdim )
char *s ; psint off,dim,rdim ;
{
        psint sz ;
	extern char *i_to_s() ;
#ifdef  SUN
        extern reset_input() ;

        reset_input() ;
#endif  SUN

        if ( !inerror )
        {
                inerror = STRUE ;

                error_heading( s ) ;
                put_error( "\nnew offset = " ) ;
                put_error( i_to_s( &sz,off ) ) ;
                put_error( "\nnew dimension = " ) ;
                put_error( i_to_s( &sz,dim ) ) ;
                put_error( "\ndimension of image = " ) ;
                put_error( i_to_s( &sz,rdim ) ) ;
                put_error( "\n" ) ;
                if ( inited ) tidy_up() ;
                trace( SFALSE ) ;
        }
        Exit( -1 ) ;
}

error_heading( s )
char *s ;
{
	put_error( "S_algol error run time error: \n" ) ;
	put_error( s ) ;
	( void ) write_error( 2,"\n",1 ) ;
}

put_error( s )
char *s ;
{
        int sz = 0 ; char *c = s ;

        while ( *c++ != 0 ) sz++ ;
        ( void ) write_error( 2,s,sz ) ;
}

#endif	RASTOP

Exit( status )
int status ;
{
#ifdef	RASTOP
        click_to_quit() ;
        D_shutdownDisplay() ;
        stop_itimer() ;
#endif	RASTOP

        exit( status ) ;
}

write_error( fd,buf,nbyte )
int	fd ;
char	*buf ;
int	nbyte ;
{
#ifdef  SUN
        extern int pw_fd ;
        extern struct pixwin *scr_pw ;
        extern psint mouse_live ;
        extern psint sigwinch_ok ;
#ifdef	X11
	extern psint Xscreen ;
#endif	X11

        if ( ( scr_pw != ( struct pixwin * ) 0 ) && mouse_live && sigwinch_ok )
                psrop_str( buf,nbyte ) ; else
#ifdef	X11
        if ( Xscreen != ( psint ) -1 && mouse_live )
                psrop_str( buf,nbyte ) ; else
#endif	X11
#endif  SUN
        write( fd,buf,nbyte ) ;
}
