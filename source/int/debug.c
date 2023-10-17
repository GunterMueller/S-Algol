#include "int.h"
#include <stdio.h>

int deb_line =	 SFALSE ;
int deb_step =	 SFALSE ;
int deb_auto =	 SFALSE ;
int deb_go =	 SFALSE ;
int count =	 0 ;
unsigned char *dbgcp ; 

char *dbg_name = "DBG> " ;

debug( pc )
unsigned char *pc ;
{

/*
	inst = ( int )( *pc ) ;
	disp_inst( inst )
*/
	dbgcp = pc ;
	if ( deb_auto ) disp_inst( ( int )( *dbgcp ) ) ;
	if ( deb_line ) { 
		if ( line_no == count ) { 
			deb_line = SFALSE ; 
			command( dbgcp ) ; 
		} 
	}
	else
		if ( deb_step ) { 
			if ( count == 1 ) { 
				deb_step = SFALSE ; 
				command( dbgcp ) ; 
			} 
			else count-- ; 
		}
		else
			if ( deb_go ) {
			}
			else command( dbgcp ) ;
}

command( pc )
unsigned char *pc ;
{
	int inst ;
	char c ; 
	int *a1,*a2 ;

	inst = ( int )( * pc ) ;
	disp_inst( inst ) ; 
	deb_go = SFALSE ;
	deb_auto = SFALSE ;

	while( !deb_go )
	{
		( void ) printf( "%s",dbg_name ) ;
		( void ) scanf( "%c",&c ) ;
		switch( c )
		{
		case 'o':
			deb_auto = STRUE ;
			deb_go = STRUE ;
			break ;
		case 'l': 
			deb_line = STRUE ; 
			( void ) scanf( "%d",&count ) ; 
			deb_go = STRUE ;
			break ;
		case 's': 
			deb_step = STRUE ; 
			( void ) scanf( "%c",&c ) ;
			if ( c == '\n' ) count = 1 ; 
			else ( void ) scanf( "%d",&count ) ;
			deb_go = STRUE ;
			break ;
		case 'e': 
			( void ) scanf( "%x %x",&a1,&a2 ) ;
			for ( ; a1 <= a2 ; a1++ )
				( void ) printf( "%s%04x :  %04x ( %d )\n",dbg_name,a1,*a1,*a1 ) ;
			break ;
		case 'g': 
			deb_go = STRUE ;
			break ;
		case 'i': 
			dbg_info( inst ) ;
			break ;
		case 'q': 
			exit( 0 ) ;
		case '?':
			help_info() ;
			break ;
		default : 
			( void ) printf( "%sWhat ?\n",dbg_name ) ;
		}

		newline() ;
	}
}

help_info()
{
	( void ) printf( "e ( m,n ) - examine the locations given\n" ) ;
	( void ) printf( "g	   - leave debug mode and run normally\n" ) ;
	( void ) printf( "i  	   - display information on stacks etc.\n" ) ;
	( void ) printf( "l	   - run until the given line number\n" ) ;
	( void ) printf( "o         - put out debug info each time round the loop\n" ) ;
	( void ) printf( "s ( n )   - single step the program n steps\n" ) ;
	( void ) printf( "q	   - quit the program\n" ) ;
	( void ) printf( "?	   - display this information\n" ) ;
}

newline()
{
	while ( getchar() != ( int ) '\n' ) ;
}

dbg_info( inst )
int inst ;
{
	register int i,*tsp,*tpsp ;
/*
	short int *start ;
*/
	( void ) printf( "Sp = %x, Psp = %x\n",sp,psp ) ;
	tsp = sp ; 
	tpsp = psp ;
	( void ) printf( " Main stack ( sp,*sp )     Pointer Stack ( psp,*psp )\n" ) ;
	for ( i = 1 ; i <= 3 ; i++ )
	{ 
		( void ) printf( "%6x  %6x           %6x  %6x\n",tsp,*tsp,tpsp,*tpsp ) ; 
		tsp++ ; 
		tpsp++ ; 
	}

	( void ) printf( "sf = %x  sb = %x  psf = %x  psb = %x heap_base = %x storeBase = %x\n",sf,sb,psf,psb,heap_base,storeBase ) ;
/*
	start = ( short int * ) storeBase ;
*/
#ifdef PERQ
	( void ) printf( "pc = %04x  inst = %04x\n",( int ) dbgcp / 2,inst ) ;
#else
	( void ) printf( "pc = %04x  inst = %04x\n",dbgcp,inst ) ;
#endif
}

disp_inst( inst )
int inst;
{
	( void ) printf( "\n  loc       instr\n  ---       -----\n" ) ;
#ifdef PERQ
	( void ) printf( "%4x         %2x\n",( int ) dbgcp / 2 - ( int ) storeBase,inst ) ;
#else
	( void ) printf( "%4x         %2x\n",( int ) dbgcp - ( int ) storeBase,inst ) ;
#endif
}
