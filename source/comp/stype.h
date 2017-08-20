/*	This module is an attempt to mimic type pntr in S-Algol 	*/
/*	Any S-algol structure used in the compiler is described by a	*/
/*	struct below. Fields of type pntr are mimiced using "pointer	*/
/*	to void" since "C" can cope with mutually recursive structs	*/
/*	and unions.							*/


/* Unions must carry their own tag - Definitions below */

#define	NIL			( psint ) -1
#define	SCALAR_CLASS		( psint ) 0
#define	VECTOR_CLASS		( psint ) 1
#define	PROC_CLASS		( psint ) 2
#define	VAR_CLASS		( psint ) 3
#define	STRUCTURE_CLASS		( psint ) 4
#define	FIELD_CLASS		( psint ) 5
#define	CONS_CLASS		( psint ) 6
#define	STRUCT_TABLE_CLASS	( psint ) 7
#define	CONST_CLASS		( psint ) 8
#define	STR_CLASS		( psint ) 9
#define	P_DECL_CLASS		( psint ) 10
#define	S_DECL_CLASS		( psint ) 11
#define	EXTERNAL_CLASS		( psint ) 12
#ifdef	RASTER
#define	IMAGE_CLASS		( psint ) 13
#endif	RASTER
#define	LINK_CLASS		( psint ) 14
#define	NAMELIST_CLASS		( psint ) 15

/* Structure definitions */

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
#ifdef  RASTER
struct Image{ void *image_elements ; } ;
#endif  RASTER ;
struct link{ char *name ; psint addr1,addr2 ; void *type1,*left,*right ; } ;
struct namelist{ char *names ; void *types,*next ; } ;

/* A union combination of them all */

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
#ifdef  RASTER 
			struct Image Image ;
#endif  RASTER
			struct link link ;
			struct namelist namelist ;
		      }	class ;
	      } ;

typedef	struct	stype *pntr ;

#define	stype_size	sizeof( struct stype )
