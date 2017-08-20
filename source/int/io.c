#include "int.h"
#include "errno.h"
#ifdef	RASTER
#include <sys/types.h>
#include <sys/time.h>
/*
#ifdef	SUN
#include <pixrect/pixrect_hs.h>
#include <sunwindow/window_hs.h>
#endif	SUN
*/
#endif	RASTER

#define EOF	-1
#define I_W_OFF	-7
#define S_W_OFF	-8
#define S_I_OFF	-2
#define S_O_OFF	-1
#define CP		2
#define FLAGS		1
#define REC_SIZE	3
#define FREE_VEC	BUFF_SIZE / 2 + OVERHEAD - 2
#define FDES		0377		/* low order 8 bits */
#define READ_FLAG	0100000
#define WRITE_FLAG	040000
#define WP_FLAG		020000
#define CLOSED_FLAG	010000
#define EOF_FLAG	04000

extern int errno ;
char *True =	"true" ;
char *False = 	"false" ;
char *Minstr =	"-2147483648" ;

char *rm1 =		"Read on nullfile" ;
char *rm2 =		"Read past end-of-file" ;
char *rm3 =		"Illegal character in integer" ;
char *rm4 =		"Illegal character in bool" ;
char *rm5 =		"String literal must start with a '\"' " ;
char *rm6 =		"String literal too large" ;
char *rm7 =		"Name too large" ;
char *rm8 =		"System read error on file" ;
char *rm9 =		"Read on closed file" ;
char *rm10 =		"Read on write-only file" ;
char *wm1 =		"Write to nullfile" ;
char *wm2 =		"System write error to file" ;
char *wm3 =		"Write to closed file" ;
char *wm4 =		"Write to read-only file" ;
char *wm5 =		"Out byte offset must be 0 - 3" ;
char *sm1 =		"Seek on nullfile" ;
char *sm2 =		"Seek on closed file" ;
char *sm3 =		"System seek error" ;
char *clm1 =		"Close on nullfile" ;
char *clm2 =		"Close on closed file" ;
char *clm3 =		"System close error" ;
char *crm1 =		"Create of nullstring file name" ;
char *om1 =		"Open of nullstring file name" ;

extern int *c_file(),*c_string() ;

#ifdef	WEB
extern	int tmpf ;
extern	int accept_Input(),send_output() ;
extern	char *data_received ;
#endif	WEB
/* The layout of a file is below.
**
**
**    ----------------------------------------------------------------------
**    |          |          |          |          |                        |
**    |          |  flags   |          |  record  |                        |
**    |  header  |    &     |    cp    |          |         buffer         |
**    |          | file des |          |   size   |                        |
**    |          |          |          |          |                        |
**    ----------------------------------------------------------------------
**
**
*/

readc()
{
	register int c,*fdes ;

	rcheck( fdes = mkptr( *psp ) ) ;
	if ( ( c = g_byte( fdes ) ) == EOF ) error( rm2 ) ;
	*psp = mkpsptr( (  ( c & B_0_6 ) * 2 ) + char_tab ) ;
}

r_byte()
{
	register int *fdes ;

	rcheck( fdes = mkptr( *psp++ ) ) ;
	if ( ( *--sp = g_byte( fdes ) ) == EOF ) error( rm2 ) ;
}

r_16()
{
	register int *fdes ;
	register int tmp ;

	rcheck( fdes = mkptr( *psp++ ) ) ;
	if ( ( *--sp = g_byte( fdes ) ) == EOF ) error( rm2 ) ;
	if ( ( tmp = g_byte( fdes ) ) == EOF ) error( rm2 ) ;
#ifdef	M68000
	*sp = ( *sp << 8 ) | tmp  ;
#else
	*sp |= tmp << 8 ;
#endif	M68000
}

r_32()
{
	register int *fdes,tmp ;
	register char *c ;

	rcheck( fdes = mkptr( *psp++ ) ) ;
	c = ( char * )( --sp ) ;
	if ( ( tmp = g_byte( fdes ) ) == EOF ) error( rm2 ) ; *c++ = ( char ) tmp ;
	if ( ( tmp = g_byte( fdes ) ) == EOF ) error( rm2 ) ; *c++ = ( char ) tmp ;
	if ( ( tmp = g_byte( fdes ) ) == EOF ) error( rm2 ) ; *c++ = ( char ) tmp ;
	if ( ( tmp = g_byte( fdes ) ) == EOF ) error( rm2 ) ; *c = ( char ) tmp ;
}

r_int()
{
	register int *fdes,result,c ; 
	int minus ;

	rcheck( fdes = mkptr( *psp++ ) ) ;
	if ( ( c = skip( fdes ) ) == '+' ) c = g_byte( fdes ) ;
	else
		if ( ( minus = ( c == '-' ) ) ) c = g_byte( fdes ) ;
	if ( c == EOF ) error( rm2 ) ;
	if ( c >= '0' && c <= '9' ) result = c - '0' ; 
	else error( rm3 ) ;
	if ( minus ) result = -result ;
	c = g_byte( fdes ) ;
	while ( c >= '0' && c <= '9' )
	{
		c -= '0' ;
		result = result * 10 + ( minus ? -c : c ) ;
		c = g_byte( fdes ) ;
	}

	fdes[ CP ] -= 1 ;
	*--sp = result ;
}

r_bool()
{
	register int *fdes,c ;

	rcheck( fdes = mkptr( *psp++ ) ) ;
	switch ( c = skip( fdes ) )
	{
	case 'f': 
		if ( ( c = g_byte( fdes ) ) != 'a' ) berror( c ) ;
		if ( ( c = g_byte( fdes ) ) != 'l' ) berror( c ) ;
		if ( ( c = g_byte( fdes ) ) != 's' ) berror( c ) ;
		if ( ( c = g_byte( fdes ) ) != 'e' ) berror( c ) ;
		*--sp = SFALSE ;
		break ;
	case 't': 
		if ( ( c = g_byte( fdes ) ) != 'r' ) berror( c ) ;
		if ( ( c = g_byte( fdes ) ) != 'u' ) berror( c ) ;
		if ( ( c = g_byte( fdes ) ) != 'e' ) berror( c ) ;
		*--sp = STRUE ;
		break ;
	default : 
		berror( c ) ;
	}
}

berror( c )
int c ;
{
	if ( c == EOF ) error( rm2 ) ; 
	else error( rm4 ) ;
}

psptr make_string( size,c )
register int size ; 
register char *c ;
{
	register char *ptr,*base ;
	extern int *c_string() ;

	switch ( size )
	{
	case 0: 
		return( 0 ) ;
	case 1: 
		return( mkpsptr( char_tab + ( *c & B_0_6 ) * 2 ) ) ;
	default: 
		base = ( char * ) c_string( size ) ; 
		ptr = base + 4 ;
		while( size-- )
		{
			/**BYTEORDER( ptr ) = *c++ ; ptr++ ;          /* copy the string */
			*ptr++ = *c++ ;
		}
		return( mkpsptr( ( int * ) base ) ) ;
	}
}

r_string()
{
	register int *fdes,c ; 
	register char *wsp ;
	extern int make_string() ;
	int size ;

	rcheck( fdes = mkptr( *psp ) ) ;
	if ( ( c = skip( fdes ) & B_0_6 ) != '"' ) error( rm5 ) ;
	wsp = work_space ;

	size = 0 ;
	while ( size <= WORK_SIZE && ( c = g_byte( fdes ) ) != '"' )
	{
		if ( c == EOF ) error( rm2 ) ;
		if ( ( c &= B_0_6 ) == '\'' )
		{
			if ( ( c = g_byte( fdes ) ) == EOF ) error( rm1 ) ;
			switch ( c &= B_0_6 )
			{
			case '"':
			case '\'': 
				*wsp++ = c ;
				break ;
			case 'b': 
				*wsp++ = '\b' ;
				break ;
			case 'n': 
				*wsp++ = '\n' ;
				break ;
			case 'o': 
				*wsp++ = '\r' ;
				break ;
			case 'p': 
				*wsp++ = '\014' ;
				break ;
			case 't': 
				*wsp++ = '\t' ;
				break ;
			default : 
				*wsp++ = '\'' ; 
				*wsp++ = c ; 
				size++ ;
			}

		}
		else *wsp++ = c ;
		size++ ;
	}
	if ( size > WORK_SIZE ) error( rm6 ) ;
	*psp = make_string( size,work_space ) ;
}

peek()
{
	register int *fdes,c ;

	rcheck( fdes = mkptr( *psp ) ) ;
	if ( ( c = g_byte( fdes ) ) == EOF ) error( rm2 ) ;
	fdes[ CP ] -= 1 ;
	*psp = mkpsptr( ( c & B_0_6 ) * 2 + char_tab ) ;
}

eof()
{
	register int *fdes ;

	rcheck( fdes = mkptr( *psp++ ) ) ;
	*--sp = g_byte( fdes ) == EOF ;
	fdes[ CP ] -= 1 ;
}

r_name()
{
	register int *fdes,c ; 
	register char *wsp ;
	char *ptr ; 
	int size ;
	extern int make_string() ;

	rcheck( fdes = mkptr( psp[ 1 ] ) ) ;

	size = 0 ; 
	wsp = work_space ;
	ptr = ( char * )( mkptr( *psp++ ) ) ;
	if ( ptr ) { 
		size++ ; 
		*wsp++ = ptr[ 4 ] ; 
	}
	while ( ( c = g_byte( fdes ) ) >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' ||
	    c == '.' || c <= '9' && c >= '0' )
	{
		if ( ++size > WORK_SIZE ) error( rm7 ) ;
		*wsp++ = c ;
	}
	fdes[ CP ] -= 1 ;
	*psp = make_string( size,work_space ) ;
}

r_line()
{
	register int *fdes,c ; 
	register char *wsp ; 
	int size ;
	extern int make_string() ;

	rcheck( fdes = mkptr( *psp ) ) ;
	size = 0 ; 
	wsp = work_space ;
	while ( ( c = g_byte( fdes ) ) != EOF && ( c &= B_0_6 ) != '\n' && size++ <= WORK_SIZE )
		*wsp++ = c ;

	*psp = make_string( size,work_space ) ;
}

readvec()
{
	register int *fdes,*ptr,nbytes ;
	register char *dest ;
	extern int *c_v_ib() ; 

	rcheck( fdes = mkptr( *psp ) ) ;
	nbytes = ( *sp++ ) * 4;
	if ( nbytes <= 0 ) error( "Cannot read -ve length vector" ) ;
	ptr = c_v_ib( 1,nbytes / 4 ) ;
	dest = ( char * )( ptr + 3 ) ;
	while( nbytes > 0 )
	{
		int cnt ;

		cnt = read( fdes[ FLAGS ] & FDES,dest,nbytes ) ;
		if ( ( cnt < 0 && errno != EINTR ) || cnt == 0 ) error( "read failed in read.vector" ) ; else
		if ( cnt > 0 )
		{
			dest += cnt ;
			nbytes -= cnt ;
		}
	}
	*psp = mkpsptr( ptr ) ;
}

wrtvec()
{
	register int *fdes,*vec,nbytes ;
	register char *src ;
	extern int *wcheck() ;

	vec = mkptr( *psp++ ) ;
	nbytes = ( vec[ 2 ] - vec[ 1 ] + 1 ) * 4 ;
	fdes = wcheck( mkptr( *psp++ ) ) ;
	src = ( char * )( vec + 3 ) ;
	while( nbytes > 0 )
	{
		int cnt ;

		cnt = write( fdes[ FLAGS ] & FDES,src,nbytes ) ;
		if ( cnt < 0 && errno != EINTR ) error( "write failed in write.vector" ) ; else
		if ( cnt > 0 )
		{
			src += cnt ;
			nbytes -= cnt ;
		}
	}
}

rcheck( fdes )
register int *fdes ;
{
	if ( !mkpsptr( fdes ) ) error( rm1 ) ;
	if ( fdes[ FLAGS ] & CLOSED_FLAG ) error( rm9 ) ;
	if ( !( fdes[ FLAGS ] & READ_FLAG ) ) error( rm10 ) ;
}

int skip( fdes )
register int *fdes ;
{
	register int c ;

	while ( ( c = g_byte( fdes ) ) == ' ' || c == '\t' || c == '\n' )
		if ( c == EOF ) error( rm2 ) ;

	return( c ) ;
}

w_string()
{
	register int *fdes,size ; 
	psptr str ;
	register char *c ;
	extern int *wcheck() ;


	if ( str = *psp++ )
	{
		int *istr ;

		istr = mkptr( str ) ;
		size = ( *istr++  & B_0_15 ) ;
		c = ( char * ) istr ;
	}
	else size = 0 ;
	fdes = wcheck( mkptr( *psp ) ) ;
	spaces( fdes,*sp++ - size ) ; 
	while ( size-- ) w_byte( fdes,*c++ ) ;
}

w_bool()
{
	register int width,*fdes ; 
	register char *ptr ;
	int size ;
	extern int *wcheck() ;

	fdes = wcheck( mkptr( *psp ) ) ; 
	width = *sp++ ;
	if ( *sp++ ) { 
		ptr = True ; 
		size = 4 ; 
	}
	else { 
		ptr = False ; 
		size = 5 ; 
	}
	spaces( fdes,width - size ) ;
	while ( size-- ) w_byte( fdes,*ptr++ ) ;
}

w_int()
{
	register int val,*fdes ; 
	register char *wsp ;
	int width,size ;
	extern int *wcheck() ; 
	extern char *i_to_s() ;

	width = *sp++ ; 
	fdes = wcheck( mkptr( *psp ) ) ;
	val = *sp++ ;
	if ( width < 0 ) width = sb[ I_W_OFF ] ;
	wsp = i_to_s( &size,val ) ;
	spaces( fdes,width - size ) ;
	while ( size-- ) w_byte( fdes,*wsp++ ) ;
	spaces( fdes,sb[ S_W_OFF ] ) ;
}

out_byte()
{
	register char *c ;
	register int offset,*fdes ;
	extern int *wcheck() ;

	offset = *sp++ ;
#ifdef PERQ
	switch ( offset )
	{
	case 0  : 
		offset = 2 ; 
		break ;
	case 1  : 
		offset = 3 ; 
		break ;
	case 2  : 
		offset = 0 ; 
		break ;
	case 3  : 
		offset = 1 ; 
		break ;
	default : 
		error( wm5 ) ;
	}
#else
#ifdef  M68000
        switch( offset )
        {
        case 0 : offset = 3 ; break ;
        case 1 : offset = 2 ; break ;
        case 2 : offset = 1 ; break ;
        case 3 : offset = 0 ; break ;
        default : error( wm5 ) ;
        }
#else

	if ( offset < 0 || offset > 3 ) error( wm5 ) ;	
#endif PERQ
#endif M68000
	fdes = wcheck( mkptr( *psp++ ) ) ;
	c = ( char * ) ( sp++ ) + offset ;
	w_byte( fdes,*c ) ;
}

out_16()
{
	register char *c ;
	register int offset,*fdes ;
	extern int *wcheck() ;

	offset = *sp++ * 2 ;
	if ( offset && offset != 2 ) error( wm5 ) ;	
#ifdef PERQ
	if ( offset ) offset = 0 ; else offset = 2 ;
#endif
	fdes = wcheck( mkptr( *psp++ ) ) ;
	c = ( char * ) ( sp++ ) + offset ;
	w_byte( fdes,*c++ ) ; w_byte( fdes,*c ) ;
}

out_32()
{
	register char *c ;
	register int *fdes ;
	register int i ;
	extern int *wcheck() ;

	fdes = wcheck( mkptr( *psp++ ) ) ;
	c = ( char * ) ( sp++ ) ;
	for ( i = 0 ; i < 4 ; i++ ) w_byte( fdes,*c++ ) ;
}

int g_byte( fdes )
register int *fdes ;
{
	register int flags ; 
	register char *c ;
#ifdef  SUN
        extern psint si_console ;
#endif  SUN
	int i ;

	flags = fdes[ FLAGS ] ;
	if ( fdes == mkptr( psb[ S_I_OFF ] ) )
		f_lush( ( mkptr( psb[ S_O_OFF ] ) ) ) ;
	if ( flags & EOF_FLAG ) return( EOF ) ;
	if ( flags & WP_FLAG ) { 
		f_lush( fdes ) ; 
		return( EOF ) ; 
	}
	if ( fdes[ CP ] == fdes[ REC_SIZE ] )
	{
#ifdef	RASTOP
                if ( fdes == mkptr( psb[ S_I_OFF ] ) ) in_pend = SFALSE ; /* No input available for next read(2) */
#endif	RASTOP
#ifdef  SUN
                if ( ( ( flags & FDES ) == ( psint ) 0 ) && si_console )
                {
                        i = psrd_str( ( char * )( fdes + OVERHEAD ),BUFF_SIZE ) ;
                } else
#endif	SUN
#ifdef	WEB
		if ( fdes == mkptr( psb[ S_I_OFF ] ) ) 			/* STandard input */
		{
			char tmp[ 1024 ] ;
			accept_input() ;
			strcpy( ( char * )( fdes + OVERHEAD ),data_received ) ;
			sprintf( tmp,"Received: %s\n",data_received ) ;
			write ( tmpf,tmp,strlen( tmp ) ) ;
			i = strlen( data_received ) ;
		} else
#endif	/* WEB */
		do i = read( flags & FDES,( char * )( fdes + OVERHEAD ),BUFF_SIZE ) ;
		while ( i == -1 && errno == EINTR ) ;
		if ( i == -1 ) error( rm8 ) ;
		fdes[ REC_SIZE ] = i ; 
		fdes[ CP ] = 0 ;
		if ( i == 0 )
		{ 
			fdes[ FLAGS ] |= EOF_FLAG ; 
			return( EOF ) ; 
		}
	}
	c = ( char * ) ( fdes + OVERHEAD ) ;
	i = c[ fdes[ CP ] ] ; 
	fdes[ CP ] += 1 ;
	return( i & B_0_7 ) ;
}

#ifdef	RASTER
empty_buff()                                                            /* is the standard input buffer empty ? */
{
        register psint *fdes,empty ;
#ifdef  SUN
        extern int si_console ;
#endif  SUN
 
        fdes = mkptr( psb[ S_I_OFF ] ) ;                                   /* fdes
is standard input */
                                                                        /* is it a nullfile or closed or eof */
        if ( fdes == mkptr( 0 ) || fdes[ FLAGS ] & CLOSED_FLAG || fdes[ FLAGS ] & EOF_FLAG ) return( STRUE ) ;
        empty = ( fdes[ CP ] == fdes[ REC_SIZE ] ) ;
#ifdef  SUN
                                                                        /* if the buffer is empty is the suntty buffer empty? */
        if ( empty && si_console ) return( tty_empty() ) ; else
#endif  SUN
        return( empty ) ;                                               /* return true if empty buffer - false otherwise */
}

inp_pend()              /* standard function to test for keyboard input - in graphics.c for PERQ */
{
#ifdef SUN
        /* must fix this to cope with the fd being closed !!!! */
        psint *fdes ;
        int rdmask,wrmask,xcptmask,nfds ;
        struct timeval tim ;
#ifdef  X11
        extern psint Xscreen ;
#endif  X11
 
        if ( !empty_buff() ) *--sp = STRUE ; else
#ifdef  X11
        if ( Xscreen != ( psint ) -1 ) *--sp = SFALSE ; else         /* dont use select if we've got an Xwindow */
#endif  X11
        {                                                       /* file descriptor empty so ask 4.2 BSD */
                fdes = mkptr( psb[ S_I_OFF ] ) ;
                                                                /* if s.i is nullfile or closed or eof then no input pending */
                if ( fdes == mkptr( 0 ) || fdes[ FLAGS ] & CLOSED_FLAG != ( psint ) 0 ||
                     fdes[ FLAGS ] & EOF_FLAG != ( psint ) 0 ) *--sp = SFALSE ; else
                {
                        nfds = ( int )( fdes[ FLAGS ] & FDES ) ;
        /* use select(2) to test s.i - see 4.2 Manuals */
                        wrmask = 0 ;                                    /* no writes of interest */      
                        xcptmask = 0 ;                                  /* no exceptions of interest */  
                        rdmask = 1 << nfds ;                            /* mask
for required fd */       
                        tim.tv_sec = 0 ;                                /* 0 out tim so select is a polling */
                        tim.tv_usec = 0 ;
 
                        nfds = select( nfds + 1,( fd_set * )&rdmask,( fd_set * )&wrmask,( fd_set * )&xcptmask,&tim ) ;
                        if ( nfds == -1 ) error( "unable to perform input.pending" ) ;
                        *--sp = ( psint )( nfds != 0 ) ;
                }
        }
#else
#ifdef  PERQ
                                                        /* OK if buffer not empty or chars to be read */
        *--sp = !( empty_buff() ) || ( in_pend ? STRUE : chk_keyb() ) ;
#else
        *--sp = !( empty_buff() ) ;                   /* noddy version! is input buffer empty? */
#endif  PERQ
#endif SUN
}
#endif	RASTER
 
w_byte( fdes,ch )
register int *fdes ; 
register char ch ;
{
	register char *c ;

	if ( fdes[ CP ] == BUFF_SIZE ) f_lush( fdes ) ;
	c = ( char * ) ( fdes + OVERHEAD ) ; 
	c[ fdes[ CP ] ] = ch ;
	fdes[ CP ] += 1 ; 
	fdes[ FLAGS ] |= WP_FLAG ;
	if ( ( ch == ( char ) 10 ) && ( fdes == mkptr( psb[ S_O_OFF ] ) ) ) f_lush( fdes ) ; 
}

f_lush( fdes )
register int *fdes ;
{
	register int i,nbytes ;
	register char *src ;

	register int	fd ;
        extern psint Xscreen ;

	src = ( char * )( fdes + OVERHEAD ) ;
	nbytes = fdes[ CP ] ;
	fd = ( int )( fdes[ FLAGS ] & FDES ) ;
/*
        if ( fd == 1 && Xscreen != ( psint ) -1 )
        {
 
		write( Xscreen,src,nbytes ) ;
        } else
*/
#ifdef	WEB
	if ( fdes = mkptr( psb[ S_O_OFF ] ) )
	{
		char	tmp[ 1024 ] ;

		strcpy( tmp,"Sent: " ) ;
		strncat( tmp,src,nbytes ) ;
		strcat( tmp,"\n" ) ;
		write ( tmpf,tmp,strlen( tmp ) ) ;
		send_output( src,nbytes ) ;
	}
#else
	while( nbytes > 0 )
	{
		i = write( fdes[ FLAGS ] & FDES,src,nbytes ) ;
		if ( i == -1 && errno != EINTR ) error( wm2 ) ; else
		if ( i > 0 )
		{
			src += i ;
			nbytes -= i ;
		}
	}
#endif	/* WEB */
	fdes[ CP ] = 0 ; 
	fdes[ FLAGS ] &= ~WP_FLAG ;
}

int *wcheck( fdes )
register int *fdes ;
{
	register int flags ;

	if ( !mkpsptr( fdes ) ) error( wm1 ) ;
	flags = fdes[ FLAGS ] ;
	if ( flags & CLOSED_FLAG ) error( wm3 ) ;
	if ( !( flags & WRITE_FLAG ) ) error( wm4 ) ;
	return( fdes ) ;
}

sseek()
{
	register int *fdes,desc,ptrname ;

	fdes = mkptr( *psp++ )  ;
	if ( !mkpsptr( fdes ) ) error( sm1 ) ;
	if ( fdes[ FLAGS ] & CLOSED_FLAG ) error( sm2 ) ;
	desc = fdes[ FLAGS ] & FDES ; 
	ptrname = *sp++ ;
	if ( fdes[ FLAGS ] & WP_FLAG ) f_lush( fdes ) ;
	else
	{
		long diff ;	/* used to be a register int */

		if ( diff = fdes[ CP ] - fdes[ REC_SIZE ] )
		{ 
			if ( lseek( desc,diff,1 ) == -1 ) error( sm3 ) ; 
		}
	}
	fdes[ CP ] = 0 ; 
	fdes[ REC_SIZE ] = 0 ; 
	fdes[ FLAGS ] &= ~EOF_FLAG ;
	if ( lseek( desc,( *sp++ ),ptrname ) == -1 ) error( sm3 ) ;
}

sclose()
{
	register int *fdes ;

	fdes = mkptr( *psp++ ) ;
	if ( !mkpsptr( fdes ) ) error( clm1 ) ;
	if ( fdes[ FLAGS ] & CLOSED_FLAG ) error( clm2 ) ; 
	else fdes[ FLAGS ] |= CLOSED_FLAG ;
	if ( fdes[ FLAGS ] & WP_FLAG ) f_lush( fdes ) ;
	if ( close( fdes[ FLAGS ] & FDES ) == -1 ) error( clm3 ) ;
#ifdef PERQ
	free_curs( fdes ) ;	/* free saved cursor if any */
#endif
#ifdef	COMPACT
	fdes[ CP ] = FREE_VEC | S_FILE ;	/* dummy buffer up as file */
	*fdes = 2 | S_FILE ;			/* minimum sized file */
#else
	fdes[ CP ] = FREE_VEC ; 
	*fdes = 2 | B_30 ;
#endif	COMPACT
}

create()
{
	register char *c1,*c2 ; 
	register int size ;
	int *ptr ;

	ptr = mkptr( *psp ) ;
	if ( !mkpsptr( ptr ) ) error( crm1 ) ;
	size = *ptr++ & B_0_15 ; 
	c1 = ( char * ) ptr ;
	c2 = work_space ;
	while ( size-- ) *c2++ = *c1++ ;
	*c2 = 0 ;
	ptr = c_file( OVERHEAD + BUFF_SIZE / 2 ) ;
	if ( ( ptr[ FLAGS ] = creat( work_space,*sp ) ) == -1 ) { 
		*psp = 0 ; 
		return ; 
	}
	ptr[ FLAGS ] |= WRITE_FLAG ; 
	ptr[ CP ] = 0 ; 
	ptr[ REC_SIZE ] = 0 ;
	*psp = mkpsptr( ptr ) ;
}

sopen()
{
	register char *c1,*c2 ; 
	register int size ;
	int *ptr ;

	ptr = mkptr( *psp ) ;
	if ( !mkpsptr( ptr ) ) error( om1 ) ;
	size = *ptr++ & B_0_15 ; 
	c1 = ( char * ) ptr ;
	c2 = work_space ;
	while ( size-- ) *c2++ = *c1++ ;
	*c2 = 0 ;
	ptr = c_file( OVERHEAD + BUFF_SIZE / 2 ) ;
	if ( ( ptr[ FLAGS ] = open( work_space,*sp ) ) == -1 ) { 
		*psp = 0 ; 
		return ; 
	}
	switch ( *sp )
	{
	case 0: 
		ptr[ FLAGS ] |= READ_FLAG ;
		break ;
	case 1: 
		ptr[ FLAGS ] |= WRITE_FLAG ;
		break ;
	case 2: 
		ptr[ FLAGS ] |= ( READ_FLAG | WRITE_FLAG ) ;
	}
	ptr[ CP ] = 0 ; 
	ptr[ REC_SIZE ] = 0 ;
	*psp = mkpsptr( ptr ) ;
}

sflush()
{
	extern int *wcheck() ;

	f_lush( wcheck( mkptr( *psp++ ) ) ) ;
}

int *open_si()
{
	register int *ptr ;

	ptr = c_file( OVERHEAD + BUFF_SIZE / 2 ) ;
	ptr[ FLAGS ] = READ_FLAG ; 
	ptr[ CP ] = 0 ; 
	ptr[ REC_SIZE ] = 0 ;
	return( ptr ) ;
}

int *create_so()
{
	register int *ptr ;

	ptr = c_file( OVERHEAD + BUFF_SIZE / 2 ) ;
	ptr[ FLAGS ] = WRITE_FLAG | 1 ; 
	ptr[ CP ] = 0 ; 
	ptr[ REC_SIZE ] = 0 ;
	return( ptr ) ;
}

char *i_to_s( size,val )
register int val ; 
int *size ;
{
	register char *wsp,*start ; 
	int minus ;
	if ( val == MININT ) { 
		*size = 11 ; 
		return( Minstr ) ; 
	}
	if ( minus = val & B_31 ) val = -val ;
	wsp = work_space + 20 ; 
	start = wsp ;
	if ( !val ) *--wsp = '0' ;
	else
		while ( val )
		{ 
			*--wsp = val % 10 + '0' ; 
			val /= 10 ; 
		}
	if ( minus ) *--wsp = '-' ;
	*size = start - wsp ;
	return( wsp ) ;
}

spaces( fdes,n )
register int *fdes,n ;
{
	while ( n-- > 0 ) w_byte( fdes,' ' ) ;
}

tidy_up()
{
	register int *fdes,flags ;
#ifdef PERQ
	extern undo_cursor() ;

	undo_cursor() ;
#endif
#ifdef	SUN
	extern	reset_input() ;

	reset_input() ;
#endif	SUN
	if ( fdes = mkptr( psb[ S_O_OFF ] ) )
	{
		flags = fdes[ FLAGS ] ;
		if ( !( flags & CLOSED_FLAG ) ) { 
			*--psp = mkpsptr( fdes ) ; 
/*
			sclose() ; 
*/
		}
	}
}

#ifdef	RASTER
echo_cstr( s,size )                             /* write a C string to s.o */
char *s ; psint size ;                          /* its got len chars, do flush */
{
        psint *fd,flgs ;

        /* is s.o OK for writing to ? */

        fd = mkptr( psb[ S_O_OFF ] ) ;
        if ( ( psint ) fd == ( psint ) 0 ) error( wm1 ) ;
        flgs = fd[ FLAGS ] ;
        if ( ( flgs & CLOSED_FLAG ) != ( psint ) 0 ) error( wm3 ) ;
        if ( ( flgs & WRITE_FLAG ) == ( psint ) 0 ) error( wm4 ) ;

                                                /* do the write ! */
        w_bytes( fd,size,s ) ;
        f_lush( fd ) ;                          /* really do the write! */
}

#define	CH_BF_SIZE	( ( psint ) 512 )
w_bytes( fdes,len,ch )
register psint *fdes,len ; register char *ch ;
{
        register char *c ;
        register psint chp ;
 
        chp = fdes[ CP ] ;
        c = ( char * ) ( fdes + OVERHEAD ) ;
        while( len-- > ( psint ) 0 )
        {
                if ( chp == CH_BF_SIZE )
                {
                        fdes[ CP ] = chp ;
                        f_lush( fdes ) ;
                        chp = fdes[ CP ] ;
                }
                c[ chp++ ] = *ch ;
                fdes[ FLAGS ] |= WP_FLAG ;
                if ( ( *ch == '\r' || *ch == '\n' ) && ( fdes == mkptr( psb[ S_O_OFF ]  ) ) )
                {
                        fdes[ CP ] = chp ;
                        f_lush( fdes ) ;
                        chp = fdes[ CP ] ;
                }
                ch++ ;
        }
        fdes[ CP ] = chp ;
}
#endif	RASTER
