#include "display.h" 

#define NOT( x ) ( ( ~x ) & 1 )

#ifdef XX 
#define Nplot( bitmap,x,y,xdim,value,rrule ) \
  bitword = bitmap + ( ( y * xdim ) + ( x >> 5 ) ); \
  bit_offset = 31 -  ( x & 31 ); \
  pixel = ( ( ( *bitword ) >> bit_offset ) & 1 ); \
  switch ( rrule ) \
    { \
    case 0  : pixel = 0; \
              break; \
    case 1  : pixel = NOT( value | pixel ); \
              break; \
    case 2  : pixel = NOT( value ) & pixel; \
              break; \
    case 3  : pixel = NOT( value ); \
              break; \
    case 4  : pixel = value & NOT( pixel ); \
              break; \
    case 5  : pixel = NOT( pixel ); \
              break; \
    case 6  : pixel = value ^ pixel; \
              break; \
    case 7  : pixel = NOT( value & pixel ); \
              break; \
    case 8  : pixel = value & pixel; \
              break; \
    case 9  : pixel = NOT( value ) ^ pixel; \
              break; \
    case 10 : break; \
    case 11 : pixel = NOT( value ) | pixel; \
              break; \
    case 12 : pixel = value; \
              break; \
    case 13 : pixel = value | NOT( pixel ); \
              break; \
    case 14 : pixel = value | pixel; \
              break; \
    case 15 : pixel = 1; \
              break; \
    } \
  ( *bitword ) = ( ( *bitword ) & ( ~( 1 << bit_offset ) ) ) | ( pixel << bit_offset );
#else
#define Nplot( bitmap,x,y,xdim,value ) \
  bitword = bitmap + ( ( y * xdim ) + ( x >> 5 ) ); \
  bit_offset = 31 -  ( x & 31 ); \
  pixel = ( ( ( *bitword ) >> bit_offset ) & 1 ); \
  ( *bitword ) = ( ( *bitword ) & ( ~( 1 << bit_offset ) ) ) | ( rbits[ ( value << 1 ) + pixel ] << bit_offset );
#endif XX 
  
void Ndrawline( int *bitmap,int x1,int y1,int x2,int y2,int xdim,int value,int rrule )
{
  int i,e,dx,dy,ddx,ddy,offset,*bitword,pixel,bit_offset,rbits[4];

  value &= 1 ;					/* only interested in the least significant bit */
  rbits[0] = ( ( 0xaaaa ) >> rrule ) & 1;
  rbits[1] = ( ( 0xcccc ) >> rrule ) & 1;
  rbits[2] = ( ( 0xf0f0 ) >> rrule ) & 1;
  rbits[3] = ( ( 0xff00 ) >> rrule ) & 1;

  if ( ( x1 == x2 ) && ( y1 == y2 ) )             /* pathological case where line is a point */
    {
      Nplot( bitmap,x1,y1,xdim,value );
    }
  else
    {
      dx = abs( x2 - x1 );
      dy = abs( y2 - y1 );
      ddx = dx * 2;
      ddy = dy * 2;
      if ( dx >= dy )
	{
	  if ( x1 > x2 )
	    {
	      i = x1; x1 = x2; x2 = i;
	      i = y1; y1 = y2; y2 = i;
	    }
	  offset = ( y2 > y1 ) ? 1 : -1;
	  e = ddy - dx;
	  ddx = ddy - ddx;
	  for ( i = 0 ; i <= dx ; i++ )
	    {
	      Nplot( bitmap,x1,y1,xdim,value );
	      if ( e > 0 )
		{
		  y1 += offset;
		  e += ddx;
		}
	      else
		e += ddy;
	      x1++;
	    }
	}
      else
	{
	  if ( y1 > y2 )
	    {
	      i = x1; x1 = x2; x2 = i;
	      i = y1; y1 = y2; y2 = i;
	    }
	  offset = ( x2 > x1 ) ? 1 : -1;
	  e = ddx - dy;
	  ddy = ddx - ddy;
	  for ( i = 0 ; i <= dy ; i++ )
	    {
	      Nplot( bitmap,x1,y1,xdim,value );
	      if ( e > 0 )
		{
		  x1 += offset;
		  e += ddy;
		}
	      else
		e += ddx;
	      y1++;
	    }
	}
    }
}


