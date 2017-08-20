#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "stype.h"
#include "functions.h"

/**************************************** IO DEPENDENT PARTS ARE HERE ************************************/

extern	char	*getenv();    /* C library function */
extern	char	*malloc();    /* C library function */
static char	code_file_name[ 255 ] ;

file_error( s,t )
char	*s,*t ;
{
	( void ) printf( "\nCannot %s file %s\n",s,t ) ;
	exit(-1) ;
}

char	*strsub( s,i,j )
register char	*s ;
register psint	i,j ;
/* copy j characters from s starting at position i */
{
	register char	*t ;
	register psint	k ;
	t = ( char * ) malloc( ( unsigned )( j+1 ) ) ;
	for ( k = 0; k < j; k++ )
	t[k] = s[k+i] ;
	t[j] = '\0' ;
	return( t ) ;
}

FILE *Create_code( filename )
char	*filename ;
{
	FILE	*f ;
	char	s[ MAXSIZE ],s1[ MAXSIZE ] ;
	psint	i,found = FALSE ;

	i = strlen( filename ) - 1 ;
	while ( i >= 0 && !found )
	if ( filename[i] == '/' ) found = TRUE; else i-- ;
	if ( found )
	{
		if ( i == strlen( filename ) ) ( void ) strcpy( s,"" ) ;
		else ( void ) strcpy( s,strsub( filename,i+1,strlen( filename ) - i - 1 ) ) ;
	} else ( void ) strcpy( s,filename ) ;
	if ( strlen( s ) > 2 && strcmp( strsub( s,strlen(s)-2,2 ),".S" ) == 0 )
	( void ) strcpy( s1,strsub( s,0,strlen( s ) - 2 ) ); else ( void ) strcpy( s1,s ) ;
	( void ) strcat( s1,".out" ) ;

        ( void ) strcpy( code_file_name,s1 ) ;
	if ((f=fopen( s1,"w" ))==NULL) file_error( "create",s ) ;
	return(f) ;
}

Delete_code_file()
{
#ifdef	PERQ
	FILE	*so ;

	so = Create_code( filename ) ;
	( void ) fclose( so ) ;
#else
	char	rm[ MAXSIZE ] ;

	( void ) strcpy( rm,"rm " ) ;
	( void ) strcat( ( char * )rm,code_file_name ) ;
	( void ) system( rm ) ;
#endif	PERQ
}

FILE *Create_flsum()
{
	FILE	*f ;
	char	*s = "listing.SS" ;

	if ( ( f = fopen( s,"w" ) ) == NULL ) file_error( "create",s ) ;
	return( f ) ;
}

FILE *Open_source( filename )
char	*filename ;
{
	FILE	*f ;

	if ( ( f = fopen( filename,"r" ) ) == NULL ) file_error( "open",filename ) ;
	return( f ) ;
}

#ifdef	OUTLINE
FILE *Open_prelude()
{
	FILE	*f ;
	char	*s ;
	
	s = getenv( "SPRELUDE" ) ;
	if ( s == NULL ) s = "/usr/local/lib/S/S.prelude" ;

	if ( ( f = fopen( s,"r" ) ) == NULL ) file_error( "open",s ) ;
	return( f ) ;
}
#endif	OUTLINE

FILE *Open_std_decls()
{
	FILE	*f ;
	char	*s ;
	
	s = getenv( "SSTAND" ) ;
	if ( s == NULL ) s = "/usr/local/lib/S/S.stand" ;

	if ( ( f = fopen( s,"r" ) ) == NULL ) file_error( "open",s ) ;
	return( f ) ;
}

