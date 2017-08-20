/****************************************************************/
/*                                                              */
/*             NAPIER MACHINE RASTER OP FUNCTIONS               */
/* This module implements the raster op functions required by   */
/* the Napier machines graphics. Alternative modules should be  */
/* used for each machine e.g. this one is for a SUN             */
/*                                                              */
/*  created  16/3/87   ALB                                      */
/*  revised  17/12/87  ALB                                      */
/*                                                              */
/****************************************************************/

#include "int.h"				/* interpreter definitions */

static int Nconvert_to_Xrule(int) ;
static void Nwindow_closeall(void) ;

					/* cursor to track mouse */
static psuint curs_init[] = {	0x00000000,0x7E000000,0x7C000000,0x78000000,
				0x7C000000,0x6E000000,0x47000000,0x03800000,
				0x01C00000,0x00E00000,0x00700000,0x00380000,
				0x00100000,0x00000000,0x00000000,0x00000000 } ;

static psint setupGraphics = PSFALSE ;

void Ninit_graphics(void)				/* do any work necessary to setup the graphics runtime support */
{
	if ( !setupGraphics )
	{
		D_initDisplay() ;
		setupGraphics = PSTRUE ;
	}
}

void Nshutdown_graphics(void)					/* zap any open raster devices */
{
	if ( setupGraphics )
	{
		setupGraphics = PSFALSE ;
		Nwindow_closeall() ;
		D_shutdownDisplay() ;
	}
}

/*******************************/
/*    RASTER OP FUNCTION       */
/*******************************/

void Prasterop(void)					/* perform rasterop, source and destination are on top of the pstack */
{						/* rasterop function is on top on the main stack */
	psptr sbits,dbits,fdes ;
	psint *src,*Sbits,*dst,*Dbits,*Fdes,screen_rop,cursor_rop,flags ;
	psint sXdim,sYdim,sZdim,sx,sy,sz ;
	psint dXdim,dYdim,dZdim,dx,dy,dz ;
	psint rrule,rXdim,rYdim,rZdim ;

	src = mkptr(lpsp[ -ptrsize2 ]) ;
	sbits = RBMAPV( src ) ;
#ifdef	LHEAP
	if ( ISKEY(sbits) )
	{
		sbits = ill_adr( sbits ) ;
		if ( sbits == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WBMAPV( src,sbits ) ;
	}
#endif	/*LHEAP*/
	Sbits = mkptr(sbits) ;

	dst = mkptr(lpsp[ -ptrsize1 ]) ;
	dbits = RBMAPV( dst ) ;
#ifdef	LHEAP
	if ( ISKEY(dbits) )
	{
		dbits = ill_adr( dbits ) ;
		if ( dbits == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WBMAPV( dst,dbits ) ;
	}
#endif	/*LHEAP*/

	Dbits = mkptr(dbits) ;
	if ( *Dbits & CONST_BIT )			/* are we trying to rop onto a constant image ? */
	{
		if ( !dest_const( mkpsptr(dst),(psint) 1,(psint) 2 ) )
		{
			gcp-- ;
			sys_event() ;
		}
		return ;
	} else
	{
		if ( !Nmodify( Dbits ) )		/* make sure the rop can succeed */
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
	}

	fdes = RIDEV( dst ) ;
#ifdef	LHEAP
	if ( ISKEY(fdes) )
	{
		fdes = ill_adr( fdes ) ;
		if ( fdes == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WIDEV( dst,fdes ) ;
	}
#endif	/*LHEAP*/
	Fdes = mkptr(fdes) ;

	screen_rop = cursor_rop = PSFALSE ;
	if ( fdes != nilfile )				/* we either have a screen or cursor rasterop */
	{
		flags = RFDES( Fdes ) ;
		if ( !( flags & ROPFILE ) || RDSTAMP( Fdes ) != restartClock || !D_windowExists( flags & FILENUM ) )
		{
							/* if we have a closed screen/cursor */
			if ( ( flags & ROPFILE ) && !closedWindow_error( fdes,mkpsptr(dst),(psint) 1,(psint) 2 ) )
			{
				gcp-- ;			/* retry the op the error call failed */
				sys_event() ;
				return ;
			}
		} else
		{
			psptr screen ;
			screen = RSCREEN( Fdes ) ;

			if ( !Nmodify( Fdes ) )			/* make sure the fdes can be modified in case of an error */
			{
				gcp-- ;
				sys_event() ;
				return ;
			}

#ifdef	LHEAP
			if ( ISKEY(screen) )
			{
				screen = ill_adr( screen ) ;
				if ( screen == NULL )
				{
					gcp-- ;
					sys_event() ;
					return ;
				}
				WSCREEN( Fdes,screen ) ;
			}
#endif	/*LHEAP*/
			if ( screen == dbits ) screen_rop = PSTRUE ; else
			{
				psptr cursor ;

				cursor = RCURSOR( Fdes ) ;
#ifdef	LHEAP
				if ( ISKEY(cursor) )
				{
					cursor = ill_adr( cursor ) ;
					if ( cursor == NULL )
					{
						gcp-- ;
						sys_event() ;
						return ;
					}
					WCURSOR( Fdes,cursor ) ;
				}
#endif	/*LHEAP*/
				cursor_rop = cursor == dbits ;
			}
		}
	}

	lpsp -= ptrsize2 ;

	rrule = *--lmsp & LSIGN4 ;		/* convert raster rule to a SUN fn number */

	sXdim = RXDIM( src ) ;			/* the size of the real source image */
	sYdim = RYDIM( src ) ;
	sZdim = RZDIM( src ) ;
	sx = RXOFF( src ) ;			/* the lower left X coord of the source */
	sy = RYOFF( src ) ;			/* the lower left Y coord of the source */
	sz = RZOFF( src ) ;			/* the minimum Z coord of the source */

	dXdim = RXDIM( dst ) ;			/* the size of the real dest. image */
	dYdim = RYDIM( dst ) ;	
	dZdim = RZDIM( dst ) ;
	dx = RXOFF( dst ) ;			/* the lower left X coord of the source */
	dy = RYOFF( dst ) ;			/* the lower left Y coord of the source */
	dz = RZOFF( dst ) ;			/* the minimum Z coord of the source */

	rXdim = sXdim < dXdim ? sXdim : dXdim ;	/* keep rasterop dims no bigger than the dest. dims */
	rYdim = sYdim < dYdim ? sYdim : dYdim ;
	rZdim = sZdim < dZdim ? sZdim : dZdim ;

	sy = RBMYDIM( Sbits ) - sy - rYdim ;	/* upper left Y coord inverted to match the bitmaps coord space */
	dy = RBMYDIM( Dbits ) - dy - rYdim ;	/* upper left Y coord inverted to match the bitmaps coord space */
					
	Nropfn( Sbits,Dbits,dx,dy,dz,rXdim,rYdim,rZdim,sx,sy,sz,rrule ) ;

	if ( screen_rop )
	{					/* a raster screen was drawn on - so refresh it */
		psint win ;

		win = flags & FILENUM ;
		if ( sbits == dbits )
		{
			if ( D_refreshCopy( win,sx,sy,sz,rXdim,rYdim,rZdim,dx,dy,dz,Nconvert_to_Xrule( rrule ) ) != 0 )
			{
				Nwindow_close( Fdes ) ;			/* if the rop failed - close the window */
			}
		} else
		{
			switch( rrule )
			{
			case PAMROP_SET:
			case PAMROP_CLR:
			case PAMROP_NOT( PAMROP_DST ):
			case PAMROP_DST:
					break ;
			default:	rrule = PAMROP_SRC ;
			}
			if ( D_refreshRect( win,dx,dx + rXdim - 1,dy,dy + rYdim - 1,dz,dz + rZdim - 1,Nconvert_to_Xrule( rrule ) ) != 0 )
			{
				Nwindow_close( Fdes ) ;			/* if the rop failed - close the window */
			}
		}
	} else
	{
		if ( cursor_rop )
		{
			if ( NsetCursor( Fdes ) != (psint) 0 )
			{
				Nwindow_close( Fdes ) ;			/* if the rop failed - close the window */
			}
		}
	}
}

/******************************************/
/*       A primitive rasterop             */
/*       between Napier bits              */
/******************************************/

void Nropfn( psint *sbits,psint *dbits,psint dx,psint dy,psint dz,psint rXdim,psint rYdim,psint rZdim,
		psint sx,psint sy,psint sz,psint rrule )	/* perform rasterop */
{
	psint *src_bits,sPlane,sYdim,*dst_bits,dPlane,dYdim ;

	sYdim = RBMYDIM( sbits ) ;
	sPlane = RBMSLW( sbits ) * sYdim ;		/* set sPlane to the size of a plane in words */
	src_bits = sbits + BITS ;
	src_bits += sPlane * sz ;			/* jump to the start of the planes to be drawn */

	dYdim = RBMYDIM( dbits ) ;
	dPlane = RBMSLW( dbits ) * dYdim ;		/* set dPlane to the size of a plane in words */
	dst_bits = dbits + BITS ;
	dst_bits += dPlane * dz ;			/* jump to the start of the planes to be drawn */

	if ( sbits != dbits || sz > dz )
	{
		if ( rrule != PAMROP_DST )			/* is the raster op a NO-OP */
		{
			while ( rZdim-- > (psint) 0 )			/* do a rasterop for every plane */
			{
				D_raster_op( dst_bits,RBMSLW( dbits ),dx,dy,rXdim,rYdim,( int ) rrule,
						src_bits,RBMSLW( sbits ),sx,sy ) ;
				src_bits += sPlane ;
				dst_bits += dPlane ;
			}
		}
	} else
	{							/* rop may overlap - sbits == dbits && sz <= dz */
		src_bits += sPlane * rZdim ;			/* do planes in reverse order */
		dst_bits += dPlane * rZdim ;

								/* is it an inplace rasterop ? */
		if ( sz == dz && sx == dx && sy == dy )
		{						/* YES, so tweek the rast rule */
			switch( ( int ) rrule )
			{
			case 0:
			case 2:
			case 4:
			case 6:
				rrule = PAMROP_DST & PAMROP_NOT( PAMROP_DST ) ;
				break ;
			case 1:
			case 3:
			case 5:
			case 7:
				rrule = PAMROP_NOT( PAMROP_DST ) ;
				break ;
			case 8:
			case 10:
			case 12:
			case 14:
				return ;			/* a NO-OP - just abandon the rop */
			case 9:
			case 11:
			case 13:
			case 15:
				rrule = PAMROP_DST | PAMROP_NOT( PAMROP_DST ) ;
				break ;
			default:
				return ;			/* not sure what this was so give up */
			}
		}
		while ( rZdim-- > (psint) 0 )			/* do a rasterop for every plane in reverse order */
		{
			src_bits -= sPlane ;
			dst_bits -= dPlane ;
			D_raster_op( dst_bits,RBMSLW( dbits ),dx,dy,rXdim,rYdim,( int ) rrule,
					src_bits,RBMSLW( sbits ),sx,sy ) ;
		}
	}
}

/***********************************/
/*      RASTER LINE FUNCTION       */
/***********************************/

/* lcode and lclip is the clipping algorithm found in Newman & Sproull */

static psint lcode( psint x,psint y,psint cx1,psint cx2,psint cy1,psint cy2 )	/* code up clipping area */
{
	psint res = (psint) 0 ;

	if ( x < cx1 ) res |= (psint) 8 ; else
	if ( x > cx2 ) res |= (psint) 4 ;
	if ( y < cy1 ) res |= (psint) 2 ; else
	if ( y > cy2 ) res |= (psint) 1 ;
	return( res ) ;
}

static psint lclip( psint *x1,psint *y1,psint *x2,psint *y2,psint cx1,psint cx2,psint cy1,psint cy2 )	/* do the clipping */
{
	psint c1,c2 ;

	c1 = lcode( *x1,*y1,cx1,cx2,cy1,cy2 ) ;
	c2 = lcode( *x2,*y2,cx1,cx2,cy1,cy2 ) ;
	while ( ( c1 || c2 ) && !( c1 & c2 ) )
	{
		psint c,x,y ;

		c = ( c1 ? c1 : c2 ) ;
		if ( c & (psint) 8 )
		{
			x = cx1 ;
			y = *y1 + ( *y2 - *y1 ) * ( cx1 - *x1 ) / ( *x2 - *x1 ) ;
		} else
		if ( c & (psint) 4 )
		{
			x = cx2 ;
			y = *y1 + ( *y2 - *y1 ) * ( cx2 - *x1 ) / ( *x2 - *x1 ) ;
		} else
		if ( c & (psint) 2 )
		{
			y = cy1 ;
			x = *x1 + ( *x2 - *x1 ) * ( cy1 - *y1 ) / ( *y2 - *y1 ) ;
		} else
		{
			y = cy2 ;
			x = *x1 + ( *x2 - *x1 ) * ( cy2 - *y1 ) / ( *y2 - *y1 ) ;
		}
		if ( c1 )
		{
			*x1 = x ;
			*y1 = y ;
			c1 = lcode( x,y,cx1,cx2,cy1,cy2 ) ;
		} else
		{
			*x2 = x ;
			*y2 = y ;
			c2 = lcode( x,y,cx1,cx2,cy1,cy2 ) ;
		}
	}
	return( c1 & c2 ) ;
}

void Prastline(void)
{
	psptr bits,fdes ;
	psint *dst,*Bits,*Fdes,screen_rop,cursor_rop,flags ;
	psint x1,x2,y1,y2 ;
	psint pixel,rrule,Xdim,Ydim,pixZdim,scrZdim ;

	dst = mkptr(lpsp[ -1 ]) ;
	bits = RBMAPV( dst ) ;
#ifdef	LHEAP
	if ( ISKEY(bits) )
	{
		bits = ill_adr( bits ) ;
		if ( bits == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WBMAPV( dst,bits ) ;
	}
#endif	/*LHEAP*/
	Bits = mkptr(bits) ;
	if ( *Bits & CONST_BIT )
	{
		if ( !dest_const( mkpsptr(dst),(psint) 6,(psint) 1 ) )
		{
			gcp-- ;
			sys_event() ;
		}
		return ;
	} else
	{
		if ( !Nmodify( Bits ) )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
	}

	fdes = RIDEV( dst ) ;
#ifdef	LHEAP
	if ( ISKEY(fdes) )
	{
		fdes = ill_adr( fdes ) ;
		if ( fdes == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WIDEV( dst,fdes ) ;
	}
#endif	/*LHEAP*/
	Fdes = mkptr(fdes) ;

	screen_rop = cursor_rop = PSFALSE ;
	if ( fdes != nilfile )				/* we either have a screen or cursor rasterop */
	{
		flags = RFDES( Fdes ) ;
		if ( !( flags & ROPFILE ) || RDSTAMP( Fdes ) != restartClock || !D_windowExists( flags & FILENUM ) )
		{
								/* if we have a closed screen/cursor */
			if ( ( flags & ROPFILE ) && !closedWindow_error( fdes,mkpsptr(dst),(psint) 6,(psint) 1) )
			{
				gcp-- ;				/* retry the op the error called failed */
				sys_event() ;
				return ;
			}
		} else
		{
			psptr screen ;
			screen = RSCREEN( Fdes ) ;

			if ( !Nmodify( Fdes ) )			/* make sure the fdes can be modified in case of an error */
			{
				gcp-- ;
				sys_event() ;
				return ;
			}
#ifdef	LHEAP
			if ( ISKEY(screen) )
			{
				screen = ill_adr( screen ) ;
				if ( screen == NULL )
				{
					gcp-- ;
					sys_event() ;
					return ;
				}
				WSCREEN( Fdes,screen ) ;
			}
#endif	/*LHEAP*/
			if ( screen == bits ) screen_rop = PSTRUE ; else
			{
				psptr cursor ;

				cursor = RCURSOR( Fdes ) ;
#ifdef	LHEAP
				if ( ISKEY(cursor) )
				{
					cursor = ill_adr( cursor ) ;
					if ( cursor == NULL )
					{
						gcp-- ;
						sys_event() ;
						return ;
					}
					WCURSOR( Fdes,cursor ) ;
				}
#endif	/*LHEAP*/
				cursor_rop = cursor == bits ;
			}
		}
	}

	lpsp -= ptrsize1 ;					/* pop the pstack */

	Xdim = RBMXDIM( Bits ) ;			/* the size of the real image */
	Ydim = RBMYDIM( Bits ) ;

	rrule = *--lmsp & LSIGN4 ;			/* convert raster rule to a SUN fn number */
	pixel = *--lmsp ;
	pixZdim = ( pixel >> 24 ) & LSIGN8 ;
	y2  = *--lmsp + RYOFF( dst ) ;			/* normalise image -	*/
	x2  = *--lmsp + RXOFF( dst ) ;			/* back to 0,0		*/
	y1  = *--lmsp + RYOFF( dst ) ;
	x1  = *--lmsp + RXOFF( dst ) ;

		/* line clipping algorithm to keep coords on area */
		/* give up if definitely nothing to draw */
	if ( !lclip( &x1,&y1,&x2,&y2,RXOFF( dst ),
		    RXDIM( dst ) + RXOFF( dst ) - (psint) 1,
		    RYOFF( dst ),RYDIM( dst ) + RYOFF( dst ) - (psint) 1 ) )
	{
		psint *dst_bits ;

		y1 = Ydim - y1 - (psint) 1 ;
		y2 = Ydim - y2 - (psint) 1 ;

		dst_bits = Bits + BITS ;

		Xdim = RBMSLW( Bits ) * Ydim ;			/* set Xdim to the size of a plane in words */
		Ydim = RZDIM( dst ) ;				/* set Ydim to the number of planes to be drawn */
		dst_bits += Xdim * RZOFF( dst ) ;		/* jump to the start of the planes to be drawn */

		if ( Ydim > pixZdim ) Ydim = pixZdim ;		/* don't draw more lines than there are planes */
		scrZdim = Ydim ;				/* Z dim of screen to be refreshed, if present */
		while ( Ydim-- > (psint) 0 )			/* draw a line on every plane */
		{
			D_raster_vector( dst_bits,RBMSLW( Bits ),x1,y1,x2,y2,rrule,
					pixel & ( 1 << ( int )( pixZdim - Ydim - (psint) 1 ) ) ? -1 : 0 ) ;
			dst_bits += Xdim ;
		}

		if ( screen_rop )
		{					/* a raster screen was drawn on - so refresh it */
			int win ;

			win = flags & FILENUM ;
			if ( D_refreshLine( win,x1,y1,x2,y2,RZOFF( dst ),scrZdim + RZOFF( dst ) - 1,
						Nconvert_to_Xrule( rrule ),pixel ) != 0 )
			{
				Nwindow_close( Fdes ) ;
			}
		} else
		{
			if ( cursor_rop )
			{
				if ( NsetCursor( Fdes ) != 0 )
				{
					Nwindow_close( Fdes ) ;			/* if the rop failed - close the window */
				}
			}
		}
	}
}

/*************************/
/*   colour mapping ops  */
/*************************/

void Pcolmap(void)
{
	psptr pfdes,pscreen ;
	psint *fdes,*screen,flags,win ;
	psint entry,pixel,slw,Ydim,Zdim,cmapsize ;
	unsigned char *red,*green,*blue ;
	unsigned char rpix,gpix,bpix ;

	entry = lmsp[ -1 ] ;
	pixel = lmsp[ -2 ] ;

	pfdes = lpsp[ -ptrsize1 ] ;
	fdes = mkptr(pfdes) ;
	flags = RFDES( fdes ) ;
	win = flags & FILENUM ;
	if ( !( flags & ROPFILE ) || RDSTAMP( fdes ) != restartClock || !D_windowExists( win ) )
	{
		if ( !colourMap_error( pfdes,pixel,entry ) )
		{
			gcp-- ;
			sys_event() ;
		}
		return ;
	}

	pscreen = RSCREEN( fdes ) ;
#ifdef	LHEAP
	if ( ISKEY(pscreen) )
	{
		pscreen = ill_adr( pscreen ) ;
		if ( pscreen == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WSCREEN( fdes,pscreen ) ;
	}
#endif	/*LHEAP*/
	screen = mkptr(pscreen) ;

	if ( !Nmodify( fdes ) || !Nmodify( screen ) )
	{
		gcp-- ;
		sys_event() ;
		return ;
	}

	Ydim = RBMYDIM( screen ) ;
	Zdim = RBMZDIM( screen ) ;
	slw = RBMSLW( screen ) ;

	cmapsize = (psint) 1 << Zdim ;
	pixel &= ( cmapsize - (psint) 1 ) ;

	red = ( unsigned char * )( screen + BITS + slw * Ydim * Zdim ) ;
	green = red + cmapsize ;
	blue = green + cmapsize ;

	rpix = ( unsigned char )( entry & LSIGN8 ) ;
	entry >>= (psint) 8 ;
	gpix = ( unsigned char )( entry & LSIGN8 ) ;
	entry >>= (psint) 8 ;
	bpix = ( unsigned char )( entry & LSIGN8 ) ;

	if ( cmapsize == (psint) 2 )			/* monochrome case is special !!! */
	{
		if ( pixel == 0 )			/* setup the two alternate values */
		{
			gpix = rpix ? -1 : 0 ;
			bpix = rpix ? -1 : 0 ;
			rpix = rpix ? -1 : 0 ;
		} else
		{
			gpix = rpix ? 0 : -1 ;
			bpix = rpix ? 0 : -1 ;
			rpix = rpix ? 0 : -1 ;
		}
		if ( *BYTEORDER(red) != rpix )		/* if red[0] has changed - do the change */
		{
			*BYTEORDER(red) = rpix ; *BYTEORDER(red+1) = ~rpix ;
			*BYTEORDER(green) = gpix ; *BYTEORDER(green+1) = ~gpix ;
			*BYTEORDER(blue) = bpix ; *BYTEORDER(blue+1) = ~bpix ;

			if ( D_refreshCmap( win,pixel,*BYTEORDER(red+pixel),*BYTEORDER(green+pixel),*BYTEORDER(blue+pixel) ) != 0 )
			{
				Nwindow_close( fdes ) ;			/* close window & restart op to generate error message */
				gcp-- ;
				sys_event() ;
				return ;
			}
		}
	} else
	{
		if ( *BYTEORDER(red+pixel) != rpix || *BYTEORDER(green+pixel) != gpix || *BYTEORDER(blue+pixel) != bpix )
		{
			*BYTEORDER(red+pixel) = rpix ;
			*BYTEORDER(green+pixel) = gpix;
			*BYTEORDER(blue+pixel) = bpix ;

			if ( D_refreshCmap( win,pixel,*BYTEORDER(red+pixel),*BYTEORDER(green+pixel),*BYTEORDER(blue+pixel) ) != 0 )
			{
				Nwindow_close( fdes ) ;			/* close window & restart op to generate error message */
				gcp-- ;
				sys_event() ;
				return ;
			}
		}
	}
	lmsp -= 2 ; lpsp -= ptrsize1 ;
}

void Pcolof(void)
{
	psptr pfdes,pscreen ;
	psint *fdes,*screen,flags,pixel ;
	psint entry,slw,Ydim,Zdim,cmapsize ;
	unsigned char *red,*green,*blue ;

	pixel = lmsp[ -1 ] ;
	pfdes = lpsp[ -ptrsize1 ] ;
	fdes = mkptr(pfdes) ;
	flags = RFDES( fdes ) ;
	if ( !( flags & ROPFILE ) || RDSTAMP( fdes ) != restartClock )
	{
		if ( !colourOf_error( pfdes,pixel ) )
		{
			gcp-- ;
			sys_event() ;
		}
		return ;
	}

	pscreen = RSCREEN( fdes ) ;
#ifdef	LHEAP
	if ( ISKEY(pscreen) )
	{
		pscreen = ill_adr( pscreen ) ;
		if ( pscreen == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WSCREEN( fdes,pscreen ) ;
	}
#endif	/*LHEAP*/
	screen = mkptr(pscreen) ;

	Ydim = RBMYDIM( screen ) ;
	Zdim = RBMZDIM( screen ) ;
	slw = RBMSLW( screen ) ;

	cmapsize = (psint) 1 << Zdim ;
	pixel &= ( cmapsize - (psint) 1 ) ;

	red = ( unsigned char * )( screen + BITS + slw * Ydim * Zdim ) ;
	green = red + cmapsize ;
	blue = green + cmapsize ;

	entry = ( (psint)*BYTEORDER((blue+pixel)) << (psint) 16 ) |
		( (psint)*BYTEORDER((green+pixel)) << (psint) 8 ) | (psint)*BYTEORDER((red+pixel)) ;

	lmsp[ -1 ] = entry ;
	lpsp -= ptrsize1 ;
}

/*************************/
/* Get and Set pixel ops */
/*************************/

void Pgpixel(void)
{
	psptr bits ;
	psint *src,*Bits,xpos,ypos,sXdim,sYdim ;

	src = mkptr(lpsp[ -ptrsize1 ]) ;				/* the image to be read */
	bits = RBMAPV( src ) ;
#ifdef	LHEAP
	if ( ISKEY(bits) )
	{
		bits = ill_adr( bits ) ;
		if ( bits == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WBMAPV( src,bits ) ;
	}
#endif	/*LHEAP*/
	Bits = mkptr(bits) ;

	sXdim = RXDIM( src ) ;					/* the dimensions of the destination image */
	sYdim = RYDIM( src ) ;

	ypos = lmsp[ -1 ] ;					/* the pixel's position */
	xpos = lmsp[ -2 ] ;

	if ( xpos < 0 || xpos >= sXdim || ypos < 0 || ypos >= sYdim )
	{
		if ( !get_pixel_error( mkpsptr(src),xpos,ypos ) )
		{
			gcp-- ;
			sys_event() ;
		}
	} else
	{
		psint i,pixel,Plane,PixWord,PixBit,PixDim ;

		lmsp -= 2 ; lpsp -= ptrsize1 ;

		xpos += RXOFF( src ) ;				/* coerce position to absolute pos. in real image */
		ypos += RYOFF( src ) ;

		sXdim = RBMXDIM( Bits ) ;			/* dimensions of the real image */
		sYdim = RBMYDIM( Bits ) ;
		ypos = sYdim - ypos - (psint) 1 ;		/* invert ypos so addressing is from lower left of image */
		Plane = sYdim * RBMSLW( Bits ) ;		/* size of a plane is Ydim as size of a scanline in words */
		PixWord = xpos / (psint) 32 ;			/* pixels word in a scanline is xpo div 32 */
		PixWord += ypos * RBMSLW( Bits ) ;		/* pixels word in plane is ypos scanlines further on */
		PixBit = BIT( ( xpos % (psint) 32 ) ) ;	/* pixel is bit xpos rem 32 in the pixels word */
		Bits += BITS ;				/* set bits to the start of the images pixels */
		Bits += Plane * RZOFF( src ) ;			/* move bits to the start of the first plane for the pixel */

		PixDim = RZDIM( src ) ;				/* the depth of the final pixel */
		pixel = PixDim << (psint) 24 ;		/* initialise the pixel with no pixels set */
		for ( i = (psint) 0 ; i < PixDim ; i++ )
		{
			if ( Bits[ PixWord ] & PixBit )		/* is the next bit of the pixel set ? */
				pixel |= ( (psint) 1 << i ) ;
			Bits += Plane ;				/* move to the next plane */
		}
		*lmsp++ = pixel ;				/* return the pixel value read */
	}
}

void Pspixel(void)
{
	psptr fdes,bits ;
	psint *Fdes,*dst,*Bits,xpos,ypos,pixel,dXdim,dYdim ;
	psint flags,screen_rop,cursor_rop,i,Plane,PixWord,PixBit,PixDim ;

	dst = mkptr(lpsp[ -ptrsize1 ]) ;

	dXdim = RXDIM( dst ) ;					/* the dimensions of the destination image */
	dYdim = RYDIM( dst ) ;

	pixel = lmsp[ -1 ] ;					/* the pixel to be written */
	ypos = lmsp[ -2 ] ;					/* the pixel's position */
	xpos = lmsp[ -3 ] ;

								/* check the pixel is in the image area */
	if ( xpos < 0 || xpos >= dXdim || ypos < 0 || ypos >= dYdim )
	{							/* OOPS its not, give up */
		if ( !set_pixel_error( mkpsptr(dst),xpos,ypos,pixel ) )
		{
			gcp-- ;
			sys_event() ;
		}
		return ;
	}

	bits = RBMAPV( dst ) ;
#ifdef	LHEAP
	if ( ISKEY(bits) )
	{
		bits = ill_adr( bits ) ;
		if ( bits == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WBMAPV( dst,bits ) ;
	}
#endif	/*LHEAP*/
	Bits = mkptr(bits) ;

	if ( *Bits & CONST_BIT )
	{
		if ( !dest_const( mkpsptr(dst),(psint) 3,(psint) 1 ) )
		{
			gcp-- ;
			sys_event() ;
		}
		return ;
	} else
	{
		if ( !Nmodify( Bits ) )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
	}

	fdes = RIDEV( dst ) ;
#ifdef	LHEAP
	if ( ISKEY(fdes) )
	{
		fdes = ill_adr( fdes ) ;
		if ( fdes == NULL )
		{
			gcp-- ;
			sys_event() ;
			return ;
		}
		WIDEV( dst,fdes ) ;
	}
#endif	/*LHEAP*/
	Fdes = mkptr(fdes) ;

	screen_rop = cursor_rop = PSFALSE ;
	if ( fdes != nilfile )				/* we either have a screen or cursor rasterop */
	{
		flags = RFDES( Fdes ) ;
		if ( !( flags & ROPFILE ) || RDSTAMP( Fdes ) != restartClock || !D_windowExists( flags & FILENUM ) )
		{
							/* if we have a closed screen/cursor */
			if ( ( flags & ROPFILE ) && !closedWindow_error( fdes,mkpsptr(dst),(psint) 3,(psint) 1 ) )
			{
				gcp-- ;			/* retry the op the error called failed */
				sys_event() ;
				return ;
			}
		} else
		{
			psptr screen ;

			if ( !Nmodify( Fdes ) )		/* make sure fdes can be modified in case of error */
			{
				gcp-- ;
				sys_event() ;
				return ;
			}
			screen = RSCREEN( Fdes ) ;
#ifdef	LHEAP
			if ( ISKEY(screen) )
			{
				screen = ill_adr( screen ) ;
				if ( screen == NULL )
				{
					gcp-- ;
					sys_event() ;
					return ;
				}
				WSCREEN( Fdes,screen ) ;
			}
#endif	/*LHEAP*/
			if ( screen == bits ) screen_rop = PSTRUE ; else
			{
				psptr cursor ;

				cursor = RCURSOR( Fdes ) ;
#ifdef	LHEAP
				if ( ISKEY(cursor) )
				{
					cursor = ill_adr( cursor ) ;
					if ( cursor == NULL )
					{
						gcp-- ;
						sys_event() ;
						return ;
					}
					WCURSOR( Fdes,cursor ) ;
				}
#endif	/*LHEAP*/
				cursor_rop = cursor == bits ;
			}
		}
	}

	lmsp -= 3 ; lpsp -= ptrsize1 ;			/* pop the stacks */

	xpos += RXOFF( dst ) ;			/* coerce position to absolute pos. in real image */
	ypos += RYOFF( dst ) ;

	dXdim = RBMXDIM( Bits ) ;		/* dimensions of the real image */
	dYdim = RBMYDIM( Bits ) ;
	ypos = dYdim - ypos - (psint) 1 ;	/* invert ypos so addressing from lower left of image */
	Plane = dYdim * RBMSLW( Bits ) ;	/* size of a plane is Ydim as size of scanline in words */
	PixWord = xpos / (psint) 32 ;		/* pixels word in a scanline is xpo div 32 */
	PixWord += ypos * RBMSLW( Bits ) ;	/* pixels word in plane is ypos scanlines further on */
	PixBit = BIT( ( xpos % (psint) 32 ) ) ;	/* pixel is bit xpos rem 32 in the pixels word */
	Bits += BITS ;			/* set bits to the start of the images pixels */
	Bits += Plane * RZOFF( dst ) ;		/* move bits to the start of first plane for the pixel */

	PixDim = RZDIM( dst ) ;			/* the depth of the dest. image */
	i = ( pixel >> (psint) 24 ) & LSIGN8 ;/* the depth of the pixel */
	if ( i < PixDim ) PixDim = i ;		/* copy lesser of pixel's depth or image's depth */
	for ( i = (psint) 0 ; i < PixDim ; i++ )
	{
		if ( pixel & ( (psint) 1 << i ) )	/* is the next bit of the pixel set ? */
			Bits[ PixWord ] |= PixBit ;
		Bits += Plane ;			/* move to the next plane */
	}
	if ( screen_rop )
	{					/* a raster screen was drawn on - so refresh it */
		psint win ;

		win = flags & FILENUM ;
		if ( D_refreshRect( win,xpos,xpos,ypos,ypos,RZOFF( dst ),RZOFF( dst ) + PixDim - 1,
					Nconvert_to_Xrule( PAMROP_SRC ) ) != 0 )
		{
			Nwindow_close( Fdes ) ;	
		}
	} else
	{
		if ( cursor_rop )
		{
			if ( NsetCursor( Fdes ) != 0 )
			{
				Nwindow_close( Fdes ) ;			/* if the rop failed - close the window */
			}
		}
	}
}

/*************************/
/*  window init ops  */
/*************************/

static char *space_or_null( char *c )
{
	while ( *c != ' ' && *c != ( char ) 0 ) c++ ;
	return( c ) ;
}

static char *winarg( char *winargs,char *key,psint keylen,psint *num )
{
	if ( strncasecmp( winargs,key,keylen ) == 0 )	/* find the given key in the winargs */
	{
		char save,*num_chars ;

		num_chars = winargs + keylen ;
		winargs = space_or_null( num_chars ) ;
		save = *winargs ;
		*winargs = ( char ) 0 ;
		*num = atoi( num_chars ) ;
		*winargs = save ;
	}
	return( winargs ) ;				/* return the final pos of the winargs pntr no move no number */
}

static void NprocessWindowName( char *fname,char **wname,psint *xpos,psint *ypos,psint *xdim,psint *ydim,psint *zdim )
{
	char *c1,*c2 ;

	*wname = fname + 7 ;
	c1 = space_or_null( *wname ) ;
	*xpos = (psint) -1 ; *ypos = (psint) -1 ;
	*xdim = (psint) -1 ; *ydim = (psint) -1 ; *zdim = (psint) -1 ;
	while ( *c1 != ( char ) 0 )
	{
		*c1++ = ( char ) 0 ;
		c2 = winarg( c1,"XPOS:",5,xpos ) ;		/* look for each of the 5 poss win args */
		if ( c1 == c2 ) c2 = winarg( c1,"YPOS:",5,ypos ) ;
		if ( c1 == c2 ) c2 = winarg( c1,"XDIM:",5,xdim ) ;
		if ( c1 == c2 ) c2 = winarg( c1,"YDIM:",5,ydim ) ;
		if ( c1 == c2 ) c2 = winarg( c1,"ZDIM:",5,zdim ) ;
		if ( c1 == c2 )
		{
			*c1 = ( char ) 0 ;		/* if none found null terminate the string */
		} else
		{					/* something was found so move c1 on */
			c1 = c2 ;
		}
	}
}

static psint *Nwindow_register( psint *fdes )
{
	psint size,offset ;
	psint *ftab ;

	ftab = (psint *) file_tab ;
	size = RUPB( ftab ) - RLWB( ftab ) + (psint) 1 ;
	for ( offset = 1 ; offset <= size ; offset++ )
	{
		if ( file_tab[ offset * ptrsize1 + STARTFDS ] == nilfile )
		{
			file_tab[ offset * ptrsize1 + STARTFDS ] = mkpsptr(fdes) ;
			PntrEscapes( ftab,mkpsptr(fdes) ) ;
			return( fdes ) ;
		}
	}
	return( NULL ) ;
}

static psint Ninit_Cmap( psint win,psint cmapsize,unsigned char *red,unsigned char *green,unsigned char *blue )
{
	int i,stop ;					/* init all cells by query of display uninited display */
							/* cells set by us except foreground & background */

	*BYTEORDER(red) = *BYTEORDER(green) = *BYTEORDER(blue) = ( unsigned char ) 255 ;
	*BYTEORDER(red+1) = *BYTEORDER(green+1) = *BYTEORDER(blue+1) = ( unsigned char ) 0 ;
	if ( D_refreshCmap( win,0,*BYTEORDER(red),*BYTEORDER(green),*BYTEORDER(blue) ) != 0 ||
		D_refreshCmap( win,1,*BYTEORDER(red+1),*BYTEORDER(green+1),*BYTEORDER(blue+1) ) != 0 )
	{
		return( (psint) -1 ) ;
	}
	stop = cmapsize - 1 ;
	for ( i = 2 ; i < stop ; i++ )
	{
		if ( D_queryCmap( win,i,BYTEORDER(red+i),BYTEORDER(green+i),BYTEORDER(blue+i) ) != 0 )
		{
			return( (psint) -1 ) ;
		}
		if ( !*BYTEORDER(red+i) && !*BYTEORDER(green+i) && !*BYTEORDER(blue+i) )
		{
			switch( i % 8 )
			{
			case 0:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			case 1:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
				break ;
			case 2:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
				break ;
			case 3:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			case 4:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
				break ;
			case 5:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 255 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			case 6:
				*BYTEORDER(red+i) = ( unsigned char ) 255 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 255 ;
				break ;
			default:
				*BYTEORDER(red+i) = ( unsigned char ) 0 ;
				*BYTEORDER(green+i) = ( unsigned char ) 0 ;
				*BYTEORDER(blue+i) = ( unsigned char ) 0 ;
			}
			if ( D_refreshCmap( win,i,*BYTEORDER(red+i),*BYTEORDER(green+i),*BYTEORDER(blue+i) ) != 0 )
			{
				return( (psint) -1 ) ;
			}
		}
	}
	return( 0 ) ;
}

static psint Nreinit_Cmap( psint win,psint cmapsize,unsigned char *red,unsigned char *green,unsigned char *blue )
{
	int i ;							/* reload the display's colourmap */

	for ( i = 0 ; i < cmapsize ; i++ )
	{
		if ( D_refreshCmap( win,i,*BYTEORDER(red+i),*BYTEORDER(green+i),*BYTEORDER(blue+i) ) != 0 )
		{
			return( -1 ) ;
		}
	}
	return( 0 ) ;
}

psint *Nwindow_init( psint *fdes,char *fname )
{
	psint size,csize,win ;
	char *wname ;
	psint xpos,ypos,xdim,ydim,zdim ;
	psint Xdim,Ydim,Zdim,cmapsize,slw ;
	psint *bits,*cbits ;
	unsigned char *red,*green,*blue ;			/* pointers to a devices colour map entries */
	psint *initbits,*initcbits ;
	char *offsets[ MAXZDIM ] ;

	NprocessWindowName( fname,&wname,&xpos,&ypos,&xdim,&ydim,&zdim ) ;
	if ( zdim > MAXZDIM ) zdim = MAXZDIM ;
	stop_itimer() ;						/* suspend timer interrupts over window opening */
	win = D_openWindow( wname,zdim < 0 ? 1 : zdim,&Xdim,&Ydim,&Zdim ) ;
	start_itimer() ;
	if ( win == -1 || win > FILENUM )
	{
		throb[ FSTATPOS ] = EBADF ;
		return( mkptr(nilfile) ) ;
	}
	WFDES( fdes,( RFDES( fdes ) & ~FILENUM ) | win ) ;	/* record the window number */
								/* set dimensions based on screen & fname */
	if ( win != 0 )						/* window 0 will be an exact size */
	{
		if ( xdim > 0 )					/* using specified dimensions where possible */
		{
			if ( Xdim > xdim ) Xdim = xdim ;
		} else
		{						/* if not use 2/3 of the screen dimensions */
			Xdim = ( Xdim / 3 ) * 2 ;
		}
		if ( ydim > 0 )
		{
			if ( Ydim > ydim ) Ydim = ydim ;
		} else
		{
			Ydim = ( Ydim / 3 ) * 2 ;
		}
	}

	if ( zdim > 0 )						/* depth can be specified on all windows */
	{
		if ( Zdim > zdim ) Zdim = zdim ;		/* use the specified depth */
	} else
	{
		Zdim = 1 ;					/* if no depth spec. use 1 plane only */
	}

	cmapsize = (psint) 1 << Zdim ;			/* size of colour map in entries */


	slw = ( Xdim + (psint) 31 ) / (psint) 32 ;	/* length of scan line padded in 32bit words */
	size = slw * Ydim * Zdim ;			/* size is X * Y * depth, X is padded to pixslw */

	bits = claim_heap( VECOVERHEAD + BMAPOVERHEAD + size + cmapsize ) ;	/* create the new bitmap vector */
	if ( bits == NULL )
	{
		D_closeWindow( win ) ;
		return( NULL ) ;
	}
#ifdef	OCTOPUS
	WNPNTRS( bits,(psint) 1 ) ;			/* put nil in the wiring diagram field */
	WPTR1( bits,NILPTR ) ;
#endif	/*OCTOPUS*/
	WLWB( bits,(psint) 1 - BMAPOVERHEAD ) ;	/* the lower bound so pixels start at index 1 */
	WUPB( bits,size + cmapsize ) ;			/* size + cmapsize is the upper bound */
	WBMXDIM( bits,Xdim ) ;				/* fill in screen device's dimensions */
	WBMYDIM( bits,Ydim) ;
	WBMZDIM( bits,Zdim ) ;
	WBMSLW( bits,slw ) ;				/* the number of words in a scan line */
	WBMTYPE( bits,BMISSCREEN ) ;			/* the raster is a screen device */

	initbits = bits + BITS ;
	while( size-- > 0 ) *initbits++ = (psint) 0 ;

	WSCREEN( fdes,mkpsptr(bits) ) ;		/* add the bitmap to the new image */

							/* create the cursor bitmap vector */
	csize = CURSLW * CURYDIM * CURZDIM ;
	cbits = claim_heap( VECOVERHEAD + BMAPOVERHEAD + csize ) ;
	if ( cbits == NULL )
	{
		D_closeWindow( win ) ;
		return( NULL ) ;
	}
#ifdef	OCTOPUS
	WNPNTRS( cbits,(psint) 1 ) ;			/* put nil in the wiring diagram field */
	WPTR1( cbits,NILPTR ) ;
#endif	/*OCTOPUS*/
	WLWB( cbits,(psint) 1 - BMAPOVERHEAD ) ;	/* the lower bound so pixels start at index 1 */
	WUPB( cbits,csize ) ;				/* csize is the upper bound */
	WBMXDIM( cbits,CURXDIM ) ;			/* fill in screen device's dimensions */
	WBMYDIM( cbits,CURYDIM) ;
	WBMZDIM( cbits,CURZDIM ) ;
	WBMSLW( cbits,CURSLW ) ;			/* the number of words in a scan line */
	WBMTYPE( cbits,BMISCURSOR ) ;			/* the raster is a screen device */

	initcbits = cbits + BITS ;
	while( csize-- > (psint) 0 )
	{
		initcbits[ csize ] = curs_init[ csize ] ;
	}

	WCURSOR( fdes,mkpsptr(cbits) ) ;		/* add the cursor to the new image */

	WCUXSPOT( fdes,1 ) ;				/* setup the initial cursor info */
	WCUYSPOT( fdes,1 ) ;
	WCUDROP( fdes,PAMROP_SRC ) ;
	WCUXPOS( fdes,0 ) ;
	WCUYPOS( fdes,0 ) ;

	red = ( unsigned char * )( initbits ) ;		/* colour map starts at the end of the bitmap */
	green = red + cmapsize ;			/* green colour map follows red */
	blue = green + cmapsize ;			/* blue colour map follows green */

	bits += BITS ;
	for ( size = 0 ; size < Zdim ; size++ )
	{
		offsets[ size ] = ( char * )( bits + size * slw * Ydim ) ;
	}
	if ( D_setScreen( win,xpos,ypos,Xdim,Ydim,Zdim,slw * 4,BITMAPSFORMAT,offsets ) != 0 ||	/* redraw the screen */
		NsetCursor( fdes ) !=  0 ||							/* setup the cursor */
		Ninit_Cmap( win,cmapsize,red,green,blue ) != 0 ||				/* invent the devices colour map */
		Nwindow_register( fdes ) == NULL )						/* record the new fdesc. */
	{
		D_closeWindow( win ) ;
		throb[ FSTATPOS ] = EBADF ;
		return( mkptr(nilfile) ) ;
	}
	return( fdes ) ;
}

psint Nwindow_reinit( psint *fdes,char *fname )
{
	psint size,win ;
	char *wname ;
	psint xpos,ypos,xdim,ydim,zdim ;
	psint Xdim,Ydim,Zdim,cmapsize,slw ;
	psint *bits ;
	unsigned char *red,*green,*blue ;			/* pointers to a devices colour map entries */
	char *offsets[ MAXZDIM ] ;

								/* process window name to get display and x,y pos */
	NprocessWindowName( fname,&wname,&xpos,&ypos,&xdim,&ydim,&zdim ) ;

	bits = mkptr(RSCREEN( fdes )) ;				/* look up the screen bitmap */
	xdim = RBMXDIM( bits ) ;				/* look up its dimensions */
	ydim = RBMYDIM( bits ) ;
	zdim = RBMZDIM( bits ) ;

	stop_itimer() ;						/* suspend timer interrupts over window opening */
	win = D_openWindow( wname,zdim,&Xdim,&Ydim,&Zdim ) ;
	start_itimer() ;

	if ( win == -1 || win > FILENUM )			/* if the open has failed - giveup */
	{
		throb[ FSTATPOS ] = EBADF ;
		return( PSFALSE ) ;
	}

	if ( Xdim < xdim || Ydim < ydim || Zdim < zdim )	/* can we use the specified depth */
	{
		throb[ FSTATPOS ] = EBADF ;
		return( PSFALSE ) ;				/* the screen isnt big enough for the window */
	}

	slw = RBMSLW( bits ) ;					/* length of scan line in 32bit words */
	size = slw * ydim * zdim ;				/* size is slw * Y * depth */
	cmapsize = (psint) 1 << zdim ;				/* size of colour map in entries */

	bits += BITS ;					/* start of the screen image's bits */
	red = ( unsigned char * )( bits + size ) ;		/* colour map starts at the end of the bitmap */
	green = red + cmapsize ;				/* green colour map follows red */
	blue = green + cmapsize ;				/* blue colour map follows green */

	for ( size = 0 ; size < zdim ; size++ )			/* build an offsets vector for D_setScreen */
	{
		offsets[ size ] = ( char * )( bits + size * slw * Ydim ) ;
	}

	if ( D_setScreen( win,xpos,ypos,xdim,ydim,zdim,slw * 4,BITMAPSFORMAT,offsets ) != 0 ||	/* redraw the screen */
		NsetCursor( fdes ) !=  0 ||							/* setup the cursor */
		Nreinit_Cmap( win,cmapsize,red,green,blue ) != 0 ||				/* reset the devices colour map */
		Nwindow_register( fdes ) == NULL ||						/* record the new fdesc. */
												/* redraw the dindow */
		D_refreshRect( win,0,xdim-1,0,ydim-1,0,zdim-1,Nconvert_to_Xrule(PAMROP_SRC) ) != 0 )
		
	{
		D_closeWindow( win ) ;
		throb[ FSTATPOS ] = EBADF ;
		return( PSFALSE ) ;
	}

	WFDES( fdes,( RFDES( fdes ) & ~FILENUM ) | win ) ;	/* the reinit worked - record the window number */
	return( PSTRUE ) ;
}

void Nwindow_freeze(void)
{
	psint size,offset ;
	psint *ftab ;

	ftab = (psint *) file_tab ;
	size = RUPB( ftab ) - RLWB( ftab ) + (psint) 1 ;
	for ( offset = 1 ; offset <= size ; offset++ )
	{
		psptr fdes ;

		fdes = file_tab[ offset * ptrsize1 + STARTFDS ] ;
		if ( fdes != nilfile )
		{
			psint *Fdes,flags,win ;

			Fdes = mkptr(fdes) ;
			flags = RFDES( Fdes ) ;
			win = flags & FILENUM ;
			if ( ( flags & ROPFILE ) && ( RDSTAMP( Fdes ) == restartClock ) )
			{
				if ( D_freeze( win ) != 0 )
				{
					Nwindow_close( Fdes ) ;
				}
			}
		}
	}
}

void Nwindow_unfreeze(void)
{
	psint size,offset ;
	psint *ftab ;

	ftab = (psint *) file_tab ;
	size = RUPB( ftab ) - RLWB( ftab ) + (psint) 1 ;
	for ( offset = 1 ; offset <= size ; offset++ )
	{
		psptr fdes ;

		fdes = file_tab[ offset * ptrsize1 + STARTFDS ] ;
		if ( fdes != nilfile )
		{
			psint *Fdes,flags,win ;

			Fdes = mkptr(fdes) ;
			flags = RFDES( Fdes ) ;
			win = flags & FILENUM ;
			if ( ( flags & ROPFILE ) && ( RDSTAMP( Fdes ) == restartClock ) )
			{				/* fdes pntr fields are lons... */
				psint *screen,slw,Xdim,Ydim,Zdim,i ;
				char *offsets[ MAXZDIM ] ;

				screen = mkptr(RSCREEN( Fdes )) ;
				slw = RBMSLW( screen ) ;
				Xdim = RBMXDIM( screen ) ;
				Ydim = RBMYDIM( screen ) ;
				Zdim = RBMZDIM( screen ) ;
				screen += BITS ;
				for ( i = 0 ; i < Zdim ; i++ )
				{
					offsets[ i ] = ( char * )( screen + i * slw * Ydim ) ;
				}
									/* refresh the screen */
				if ( D_setScreen( win,0,0,Xdim,Ydim,Zdim,slw * 4,BITMAPSFORMAT,offsets ) != 0 ||
					D_unfreeze( win ) != 0 )
				{
					Nwindow_close( Fdes ) ;
				}
			}
		}
	}
}

static void Nwindow_closeall(void)
{
	psint size,offset ;
	psint *ftab ;

	ftab = (psint *) file_tab ;
	size = RUPB( ftab ) - RLWB( ftab ) + (psint) 1 ;
	for ( offset = 1 ; offset <= size ; offset++ )
	{
		psptr fdes ;

		fdes = file_tab[ offset * ptrsize1 + STARTFDS ] ;
		if ( fdes != nilfile )
		{
			psint *Fdes,flags,win ;

			Fdes = mkptr(fdes) ;
			flags = RFDES( Fdes ) ;
			win = flags & FILENUM ;
			if ( ( flags & ROPFILE ) && ( RDSTAMP( Fdes ) == restartClock ) )
			{				/* fdes pntr fields are lons... */
				file_tab[ offset * ptrsize1 + STARTFDS ] = nilfile ;
				D_closeWindow( win ) ;
				if ( Nmodify( Fdes ) )
				{
					WFDES( Fdes,NILFDES ) ;
					WDSTAMP( Fdes,0 ) ;
					WSCREEN( Fdes,NILPTR );
					WCURSOR( Fdes,NILPTR );
				}
			}
		}
	}
}

void Nwindow_close( psint *fdes )
{
	psint win,size,offset,*ftab ;

	ftab = (psint *) file_tab ;
	size = RUPB( ftab ) - RLWB( ftab ) + (psint) 1 ;
	offset = 0 ;
	while( offset++ < size )
	{
		if ( file_tab[ offset * ptrsize1 + STARTFDS ] == mkpsptr(fdes) )
		{
			file_tab[ offset * ptrsize1 + STARTFDS ] = nilfile ;
			offset = size ;
		}
	}
	win = RFDES( fdes ) & FILENUM ;
	D_closeWindow( win ) ;
	if ( Nmodify( fdes ) )
	{
		WFDES( fdes,NILFDES ) ;
		WDSTAMP( fdes,0 ) ;
		WSCREEN( fdes,NILPTR );
		WCURSOR( fdes,NILPTR );
	}
}

static int Nconvert_to_Xrule( int rule )
{
	rule &= PAMROP_NOT( 0 ) ;				/* convert pixrect rule into just the op */
	switch( rule )
	{
	case PAMROP_CLR:	return( 0x0 ) ;			/* clear destination pixels */
	case PAMROP_SRC & PAMROP_DST:				/* and */
				return( 0x1 ) ;
	case PAMROP_SRC & PAMROP_NOT( PAMROP_DST ):		/* and not */
				return( 0x2 ) ;
	case PAMROP_SRC:	return( 0x3 ) ;			/* copy */
	case PAMROP_NOT( PAMROP_SRC ) & PAMROP_DST:		/* not and */
				return( 0x4 ) ;
	case PAMROP_DST:	return( 0x5 ) ;			/* no-op */
	case PAMROP_SRC ^ PAMROP_DST:				/* xor */
				return( 0x6 ) ;
	case PAMROP_SRC | PAMROP_DST:				/* or */
				return( 0x7 ) ;
	case PAMROP_NOT( PAMROP_SRC ) & PAMROP_NOT( PAMROP_DST ):	/* not and not */
				return( 0x8 ) ;
	case PAMROP_NOT( PAMROP_SRC ) ^ PAMROP_DST:		/* not xor */
				return( 0x9 ) ;
	case PAMROP_NOT( PAMROP_DST ):
				return( 0xa ) ;			/* invert destination pixels */
	case PAMROP_SRC | PAMROP_NOT( PAMROP_DST ):		/* or not */
				return( 0xb ) ;
	case PAMROP_NOT( PAMROP_SRC ):			/* not */
				return( 0xc ) ;
	case PAMROP_NOT( PAMROP_SRC ) | PAMROP_DST:		/* not or */
				return( 0xd ) ;
	case PAMROP_NOT( PAMROP_SRC ) | PAMROP_NOT( PAMROP_DST ):	/* not or not */
				return( 0xe ) ;
	case PAMROP_SET:	return( 0xf ) ;			/* set destination pixels */
	default:		return( 0x5 ) ;			/* no-op if things dont make sense */
	}
}

psint NsetCursor( psint *fdes )
{
	psint *cursor,win,xdim,ydim,slb,result ;
	unsigned char *cbits ;

	cursor = mkptr(RCURSOR( fdes )) ;	/* we already know this is a local heap object */
	slb = RBMSLW( cursor ) * WORDSIZE ;		/* scan line length in bytes */
	cbits = ( unsigned char * )( cursor + BITS ) ;
	win = RFDES( fdes ) & FILENUM ;			/* now set the cursor image */
	xdim = RBMXDIM( cursor ) ;
	ydim = RBMYDIM( cursor ) ;
	result = D_setCursor( win,&xdim,&ydim,slb,RCUXSPOT( fdes ),RCUYSPOT( fdes ),cbits ) ;
	WBMXDIM( cursor,xdim ) ;
	WBMYDIM( cursor,ydim ) ;
	return( result ) ;
}

psint NreadWindow( psint win,char *buffer,psint n )
{
	n /= 8 ;					/* divide by 8 - 2 ints per keyboard event */
	n = D_readW( win,(D_keyboardEvents *) buffer,n ) ;
	if ( n > 0 )
	{
		n *= 8 ;				/* scale up result by 8 i f anything was read */
	}
	if ( n == 0 )
	{
		n = -1 ;
		errno = EINTR ;
		threadOpSignal = PSTRUE ;		/* if multiple threads give someone else a chance to get in */
		if ( pausedThrob == realNILPTR )	/* mark this thread as paused - if no-one already waiting to a pause */
		{
			pausedThrob = throb ;
		}
	}
	return( n ) ;
}

psint Nlocator( psint *fdes,psint ydim )
{
	psint win ;
	D_locatorReply reply ;

	win = RFDES( fdes ) & FILENUM ;
	if ( D_locatorI( win,&reply ) != 0 )
	{
		return( -1 ) ;
	}

	if ( reply.dateStamp == -1 )
	{
		WLOCDSTAMP( fdes,reply.dateStamp ) ;
		threadOpSignal = PSTRUE ;		/* if multiple threads give someone else a chance to get in */
		if ( pausedThrob == realNILPTR )	/* mark this thread as paused - if no-one already waiting to a pause */
		{
			pausedThrob = throb ;
		}
	} else
	{
		WCUXPOS( fdes,reply.xpos ) ;		/* up date fdesc info. if something has happened */
		WCUYPOS( fdes,ydim - reply.ypos - 1 ) ;
		WLOCDSTAMP( fdes,reply.dateStamp == -1 ? 0 : reply.dateStamp ) ;
		WWINBUT( fdes,1,reply.but1 ) ;
		WWINBUT( fdes,2,reply.but2 ) ;
		WWINBUT( fdes,3,reply.but3 ) ;
		WWINBUT( fdes,4,reply.but4 ) ;
		WWINBUT( fdes,5,reply.but5 ) ;
	}
	return( 0 ) ;
}
