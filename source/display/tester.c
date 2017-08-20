#define AXD	64
#define AYD	4096
#define	ASLW	( ( AXD + 31 ) / 32 )
#define	ASLB	( ASLW * 4 )

#define BXD	64
#define BYD	4096
#define	BSLW	( ( BXD + 31 ) / 32 )
#define	BSLB	( BSLW * 4 )

main()
{
	/* a wee prog to test the display for speed */

	int w,x,y,z,i,j,rule,*rasterA,*rasterB ;
	extern char *malloc() ;


	rasterA = ( int * ) malloc( ASLB * AYD ) ;
	rasterB = ( int * ) malloc( BSLB * BYD ) ;

	D_initDisplay() ;
	w = D_openWindow( 0,8,&x,&y,&z ) ;
	printf( "opened %d : %d,%d,%d\n",w,x,y,z ) ;
	printf( "%s\n",getenv("DISPLAY") ) ;
	w = D_openWindow( getenv( "DISPLAY" ),8,&x,&y,&z ) ;
	printf( "opened %d : %d,%d,%d\n",w,x,y,z ) ;
	/*
	for ( i = 0 ; i < 32 ; i++ )
	{
		for ( rule = 0 ; rule < 16 ; rule++ )
		{
			for ( j = 0 ; j < 1 ; j++ )
			{
				D_raster_op( rasterB,BSLW,i,i,BXD - 37,BYD - 37,rule,rasterA,ASLW,i,i ) ;
			}
		}
	}
	*/
}
