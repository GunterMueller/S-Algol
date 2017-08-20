/* a scanline rasterop copying data from top to bottom and left to right */
static void ROPFUNC( unsigned int *dst,int dslw,int dx,int dy,int w,int h,unsigned int *src,int sslw,int sx,int sy )
{
	int Sshift1,Sshift2,DMask1,DMask2 ;
	int sw0,dw0,dwn,savedwords,dwords ;

	/* Build mask to protect bits in first word of destination scan line */
	DMask1 = dx & 31 ;	/* how many bits to set. */
	if ( DMask1 != 0 )
	{
		DMask1 = 32 - DMask1 ;
		DMask1 = 1 << DMask1 ;
		DMask1 = ~( DMask1 - 1 ) ;
	}

	/* Build mask to protect bits in last word of destination scan line */
	DMask2 = ( dx + w ) & 31 ;	/* how many bits to keep clear */
	if ( DMask2 != 0 )
	{
		DMask2 = 32 - DMask2 ;
		DMask2 = 1 << DMask2 ;
		DMask2 = DMask2 - 1 ;
	}

	/* Calculate how many desintation words are modified */
	sw0 = sx >> 5 ;			/* first word of src to read */
	dw0 = dx >> 5 ;			/* first word of dest to write */
	dwn = ( dx + w - 1 ) >> 5 ;	/* last word of dest to write */
	dwords = dwn - dw0 + 1 ;	/* no. of words modified */
	savedwords = dwords ;

	if ( dwords == 1 )
	{
		DMask1 |= DMask2 ;
		DMask2 = 0 ;
	}

	src += sw0 ; src += sy * sslw ;			/* set src to start of bits to rop */
	dst += dw0 ; dst += dy * dslw ;			/* set dst to start of bits to rop */

	/* calculate shift operands for constructing the source */
	sx &= 31 ;
	dx &= 31 ;
	Sshift1 = abs( sx - dx ) & 31 ;
	if ( Sshift1 != 0 && sx < dx ) Sshift1 = 32 - Sshift1 ;
	Sshift2 = 32 - Sshift1 ;

	/* Now work out how to encode the loops */

	if ( Sshift1 == 0 )		/* are the words aligned for us... */
	{
		while( h-- > 0 )
		{
			unsigned int srcw,dstw,*ndst,*nsrc ;

			nsrc = src ; ndst = dst ;
			src += sslw ; dst += dslw ;
			dwords = savedwords ;

			if ( DMask1 != 0 )
			{
				dwords-- ;
				GETSOURCE ;
				GETDESTINATION ;
				OPERATOR ;
				srcw &= ~DMask1 ;
				dstw = *ndst & DMask1 ;
				*ndst++ = dstw | srcw ;
			}
			if ( DMask2 != 0 ) dwords-- ;

			while( dwords-- > 0 )
			{
				GETSOURCE ;
				GETDESTINATION ;
				OPERATOR ;
				*ndst++ = srcw ;
			}

			if ( DMask2 != 0 )
			{
				GETSOURCE ;
				GETDESTINATION ;
				OPERATOR ;
				srcw &= ~DMask2 ;
				dstw = *ndst & DMask2 ;
				*ndst = dstw | srcw ;
			}
		}
	} else
	{				/* src words must be constructed via shifting.... */
		while( h-- > 0 )	
		{
			unsigned int srcw,src0,src1,dstw,*ndst,*nsrc ;

			nsrc = src ; ndst = dst ;
			src += sslw ; dst += dslw ;
			src0 = sx < dx ? 0 : *nsrc++ ; src1 = *nsrc++ ;
			dwords = savedwords ;

			if ( DMask1 != 0 )
			{
				dwords-- ;
				GETSHIFTEDSOURCE ;
				GETDESTINATION ;
				OPERATOR ;
				srcw &= ~DMask1 ;
				dstw = *ndst & DMask1 ;
				*ndst++ = dstw | srcw ;
				NEXTSOURCE ;
			}
			if ( DMask2 != 0 ) dwords-- ;

			while( dwords-- > 0 )
			{
				GETSHIFTEDSOURCE ;
				GETDESTINATION ;
				OPERATOR ;
				*ndst++ = srcw ;
				NEXTSOURCE ;
			}

			if ( DMask2 != 0 )
			{
				GETSHIFTEDSOURCE ;
				GETDESTINATION ;
				OPERATOR ;
				srcw &= ~DMask2 ;
				dstw = *ndst & DMask2 ;
				*ndst = dstw | srcw ;
			}
		}
	}
}
