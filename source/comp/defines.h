#define TRUE	( psint ) 1
#define FALSE	( psint ) 0
#define NUL	'\0'
#define MAXSIZE	( psint ) 2000    /* maximum length of names and symbols */
#define OUTPUT_LENGTH	( psint ) 2000
#define NL	'\n'
#define TAB	'\t'
#define BS	'\b'
#define FF	'\f'
#define CR	'\r'
#define DQUOTE	'"'
#define OFF_VALUE	( psint ) 0x01000000
#define	ON_VALUE	( psint ) 0x01000001
#define Stack_element_size	( psint ) 1
#define Version_no		( psint ) -8
#define	Code_v_size		( psint ) 12000
#define	twoto16		( ( psint ) 1 << 15 )
#define	twoto17		( ( psint ) 1 << 16 )

/***** Declare correct int and real types for this machine *****/

#ifdef MAC
   typedef short double	psreal ;
   typedef long		psint ;
#else
   typedef double	psreal ;
   typedef int		psint ;
#endif
#define scalar_eq( a,b )	( a == b )
#define streq( s,t )	( s == t || strcmp(s,t) == 0 )
#define Streq( s,t )	( s == t )
