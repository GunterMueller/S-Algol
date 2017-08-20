void D_raster_op( dst,dslw,dx,dy,w,h,rule,src,sslw,sx,sy )
int *dst,dslw,dx,dy,w,h,rule,*src,sslw,sx,sy ;
{
	switch( rule )
	{
	case 0:		N_rop0( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 1:		N_rop1( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 2:		N_rop2( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 3:		N_rop3( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 4:		N_rop4( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 5:		N_rop5( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 6:		N_rop6( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 7:		N_rop7( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 8:		N_rop8( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 9:		N_rop9( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 10:	;
			break ;
	case 11:	N_rop11( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 12:	N_rop12( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 13:	N_rop13( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 14:	N_rop14( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	case 15:	N_rop15( dst,dslw,dx,dy,w,h,src,sslw,sx,sy ) ;
			break ;
	}
}

void D_raster_vector( dst,dslw,x1,y1,x2,y2,rule,pixel )
int *dst,dslw,x1,y1,x2,y2,rule,pixel ;
{
	Ndrawline( dst,x1,y1,x2,y2,dslw,pixel,rule ) ;
}

