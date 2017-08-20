! %nonewline
! 10-12-84
! This package describes the raster graphics functions written in S
procedure Ofill( c#pixel image.param ; cpixel col.param ; int x,y )
if Pixel( image.param,x,y )( 0 ) ~= col.param( 0 ) do
begin
     let i = image.param( 0|1 )
     let col = col.param( 0 )
     let line.length = X.dim( i )
     let line = image line.length by 1 of col
     procedure put( cint x1,x2,y )
     begin
          copy limit line to x1 - x2 + 1 by 1 at 0,0
          onto limit i at x2,y
     end
     forward recfill( int,int -> int ) 
     procedure recfill( int x,y -> int )
     begin
          let xstart = x
          x := line.end( i,col,x,y,0 )
          put( xstart,x + 1,y )
          let xend = x
          if y < Y.dim( i ) - 1 do
          begin
               let y = y + 1
               x := line.end( i,col,xstart,y,3 )
               while x > xend do
               begin
                    x := line.end( i,if col = on then off else on,x,y,0 )
                    if x > xend do x := recfill( x,y )
               end
          end 
          if y > 0 do
          begin
               let y = y - 1
               x := line.end( i,col,xstart,y,3 )
               while x > xend do
               begin
                    x := line.end( i,if col = on then off else on,x,y,0 )
                    if x > xend do x := recfill( x,y )
               end
          end 
          xend
     end
     x := line.end( i,col,x,y,2 )
     x := x - 1
     let discard = recfill( x,y )
end
procedure the.menu( c#pixel title ;
		    c*c#pixel entries ;
                    cbool vertical 
                    -> pntr )
begin
     let Lwb = lwb( entries )
     let Upb := upb( entries ) + 1
     let no.elmts = Upb - Lwb
     ! Xdim,Ydim for each element + title
     ! Plus 2 for width and height, offset
     ! by the Lwb minus the obligatory 1
     let Upb2 = ( no.elmts + 1 ) * 3 + 2 + Lwb - 1
     let spacing = 1
     let indent = 2
     let indent2 = indent * 2
     let indent4 = indent * 4
     let border = 3
     let border2 = border * 2
     let height := 0                     ! these may be actually reversed.
     let width := 0                      ! if ~vertical the height will be width
     ! make vectors containing the lowest border of each element
     let dim.vec = vector Lwb :: Upb2 of 0
     procedure height.fn( c#pixel the.entry ; cbool ht -> int )
     case true of
     ( vertical and ht ) or ( ~vertical and ~ht )  : Y.dim( the.entry )
     default          : X.dim( the.entry )
     let j := Lwb
     for i = Lwb to Upb2 - 5 by 3 do ! find out the size of it
     begin
          dim.vec( i ) := X.dim( entries( j ) )
          dim.vec( i + 1 ) := Y.dim( entries( j ) )
          dim.vec( i + 2 ) := height
          height := height + height.fn( entries( j ),true ) + indent4 + spacing
          if height.fn( entries( j ),false ) > width do ! update maximum width
               width := height.fn( entries( j ),false )
          j := j + 1
     end
     ! Now fill in dimensions of  title
     dim.vec( Upb2 - 4 ) := X.dim( title )
     dim.vec( Upb2 - 3 ) := Y.dim( title )
     dim.vec( Upb2 - 2 ) := height
                                         ! allow for title and highlighting in width
     if height.fn( title,false ) > width do width := height.fn( title,false )
     width := width + indent4
     ! next initialise the pop up menu
     let pop := screen
     if vertical then pop := image width + border2 by height + border2 + Y.dim( title ) + indent4 of on else
     begin                              ! swap height and width if its a horizontal menu
          let t = width
          width := height
          height := t
          pop := image width + border2 + X.dim( title ) + indent4 by height + border2 of on
     end
     for i = Lwb to Upb - 1 do ! put entries in menu
     begin
          let offset = i + ( i - Lwb ) * 2
          let dest = if vertical then limit pop to width by Y.dim( entries( i ) ) + indent4 at
                                                   border,border + height - dim.vec( offset + 5 )
                     else limit pop to X.dim( entries( i ) ) + indent4 by height at
                                       border + dim.vec( offset + 2 ) + indent4 + X.dim( title ),border
          xor dest onto dest
          copy entries( i ) onto limit dest at indent2,indent2
     end
     if vertical then not title onto limit pop at border,border + height + indent2
     else not title onto limit pop at border + indent2,border
     ! Now fill in width and height values in the vector
     dim.vec( Upb2 - 1 ) := height
     dim.vec( Upb2 ) := width
     menu( pop,dim.vec,vertical )
end
    
procedure call.the.menu( cpntr the.menu ; cint X,Y -> int )
! Find out what option user wants giving visual feedback
begin
     let indent = 2
     let indent2 = indent * 2
     let indent4 = indent * 4
 
     let border = 3
     let border2 = border * 2
     ! redeclare X and Y as variable ints
     let X := X ; let Y := Y
     let Height = Y.dim( the.menu( menu.image ) )
     let Width = X.dim( the.menu( menu.image ) )
     let dim.vec = the.menu( menu.dimensions.vector )
     let Lwb = lwb( dim.vec )
     let Upb2 = upb( dim.vec )
     let height = dim.vec( Upb2 - 1 )
     let width = dim.vec( Upb2 )
     let vertical = the.menu( vertical )
     let no.elements = ( Upb2 - Lwb + 1 - 2 - 3 ) div 3
     let Upb = no.elements + Lwb 
     if Height > Y.dim( screen ) or Width > X.dim( screen ) do
     begin                     ! the menu wont fit onto the screen
          write "S-algol run time error: Screen is too small for the menu'n"
          trace
          abort
     end
                               ! move the menu so it is all on the screen
     if Width + X > X.dim( screen ) do X := X.dim( screen ) - Width
     if Height + Y > Y.dim( screen ) do Y := Y.dim( screen ) - Height
     let menu.index := Upb ! index of selected word in menu
     let old.menu.index := Upb  ! index of last selected word in menu
     let last.x := 0 ; let last.y := 0  ! last known position of mouse
     let where := locator
     ! get space to hold screen 'below' the menu
     let temp = image Width by Height of on
     let new = limit screen to Width by Height at X,Y
     ! modify X and Y and select image of the menu contents
     X := X + border
     if ~vertical do X := X + dim.vec( Upb2 - 4 ) + indent4
     Y := Y + border
     let images = limit screen to width by height at X,Y
     
     procedure invert( int index )
     ! invert user option
     begin
          let offset = index + ( index - Lwb ) * 2
          let lit = if vertical then limit images to
                                     width - indent2 by dim.vec( offset + 1 ) + indent2
                                     at indent,height - dim.vec( offset + 5 ) + indent
          else limit images to
               dim.vec( offset ) + indent2 by height - indent2
               at dim.vec( offset + 2 ) + indent,indent
          not lit onto lit
     end
     procedure highlight( int index )
     ! highlight user option if the menu.index has changed
     if old.menu.index ~= index do
     begin
          if old.menu.index ~= Upb do invert( old.menu.index )
          if index ~= Upb do invert( index )
          old.menu.index := index
     end          
     procedure index.no( int last.x,last.y -> int ) 
     if last.y < Y or last.x < X or last.x >= X + width or last.y >= Y + height then Upb else
     if vertical then 
     begin
          let i := Lwb
          begin
               while Y + height - last.y - 1 >= dim.vec( i + 2 ) do i := i + 3
               ( ( i - 3 ) + ( 2 * Lwb ) ) div 3
          end
     end else
     begin
          let i := Lwb
          begin
               while last.x - X >= dim.vec( i + 2 ) do i := i + 3
               ( ( i - 3 ) + ( 2 * Lwb ) ) div 3
          end
     end
     copy new onto temp                    ! save screen
     copy the.menu( menu.image )  onto new ! display menu
    
     while ~where( selected ) or ~where( the.buttons )( 1 ) do where := locator
     while where( the.buttons )( 1 ) do
     begin
          last.x := where( X.pos ) ; last.y := where( Y.pos )
          menu.index := index.no( last.x,last.y )
          highlight( menu.index )
          repeat where := locator while ~where( selected )
     end
    
     copy temp onto new      ! replace screen
     menu.index
end
procedure Oline( c#pixel im ; int x1,y1,x2,y2,dx,dy ; ( c#pixel ) style )
begin
     let delta.x := x2 - x1 ; let delta.y := y2 - y1
     let xincr = if delta.x > 0 then 1 else -1
     let yincr = if delta.y > 0 then 1 else -1
     delta.x := abs( delta.x ) ; delta.y := abs( delta.y )
     dx      := abs( dx )      ; dy      := abs( dy )
     if delta.x > delta.y then	! more horizontal line
     begin
          let slope = delta.y / delta.x
          let Xdiff := float( dx ) ; let Ydiff := float( dy )
          for x = x1 to x2 by xincr do 
          begin
               Ydiff := Ydiff + slope
               if Ydiff >= dy and Xdiff >= dx do
               begin
                    let y = y1 + yincr * truncate( abs( x - x1 ) * slope )
                    Ydiff := 0 ; Xdiff := 0
                    if x < X.dim( im ) and x > 0 and y < Y.dim( im ) and y > 0 do
                    style( limit im at x,y )
               end
               Xdiff := Xdiff + 1
          end
     end else
     if delta.y ~= 0 do		! line has length and more vertical line
     begin
          let Xdiff := float( dx ) ; let Ydiff := float( dy )
          let slope = delta.x / delta.y
          for y = y1 to y2 by yincr do
          begin
               Xdiff := Xdiff + slope
               if Ydiff >= dy and Xdiff >= dx do
               begin
                    let x = x1 + xincr * truncate( abs( y - y1 ) * slope  )
                    Ydiff := 0 ; Xdiff := 0
                    if x < X.dim( im ) and x > 0 and y < Y.dim( im ) and y > 0 do
                    style( limit im at x,y )
               end
               Ydiff := Ydiff + 1
          end
     end
end
procedure the.str.to.tile ( cstring the.string,font.name -> #pixel )
begin
     procedure make.font.structure( cstring font.name -> pntr )
     ! returns a pointer to a S-algol font
     begin
     
     
          let font.names.vec = @1 of cstring[ "cou20","fix13","gac16n","hci45i" ]
          let dir.name = "/usr/local/lib/S/fonts"		! This is BAD
     
          procedure check.name
          begin
               let continue := true
               let index := lwb( font.names.vec )
               let Upb = upb( font.names.vec )
     
               while continue and ( index <= Upb ) do
                    if font.name = font.names.vec( index ) then continue := false
                    else index := index + 1
     
               if continue do { write "No font ",font.name," available'n" ; abort }
          end
     
          procedure the.file( cstring file.name -> file )
          begin
               let fd = open( file.name,0 )
               if fd = nullfile do { write "Cannot open file ",file.name,"'n" ; abort }
               fd
          end
     
          procedure get.short1( file f -> int )
          read.byte( f ) + shift.l( read.byte( f ),8 )

          procedure get.short2( file f -> int )
          shift.l( read.byte( f ),8 ) + read.byte( f ) 
     
          procedure get.short( file f -> int ) ; get.short1( f )

          !procedure get.long( file f -> int )
               !shift.l( read.byte( f ),16 ) +
               !shift.l( read.byte( f ),24 ) +
               !read.byte( f ) +
               !shift.l( read.byte( f ),8 )

          procedure get.long( file f -> int )
          begin
	       let b0 = read.byte( f ) 
	       let b1 = read.byte( f ) 
	       let b2 = read.byte( f ) 
	       let b3 = read.byte( f ) 

               let x0 = shift.l( b0,16 ) +
			shift.l( b1,24 ) +
			b2 +
			shift.l( b3,8 )

               let x1 = b0 +
			shift.l( b1,8 ) +
			shift.l( b2,16 ) +
			shift.l( b3,24 )

               let x2 = shift.l( b0,24 ) +
			shift.l( b1,16 ) +
			shift.l( b2,8 ) +
			b3
               x0
          end
     
          procedure height.of( *pntr fnt -> int )
          begin
               let biggest := 0
               for i = lwb( fnt ) to upb( fnt ) do 
               if fnt( i,font.lno ) > biggest do biggest := fnt( i,font.lno )
               biggest + 1
          end
     
          check.name
          let ff = the.file( dir.name ++ "/" ++ font.name ++ ".kst")
          let height = get.short( ff )
          let base = height - get.short( ff )
          let font.info = vector 0 :: 127 of nil
          let new.font = vector 0 :: 127 of nil
     
          for next = 0 to 127 do
          begin
               let tangle = get.short( ff )
               font.info( next ) := the.font.info( shift.r( tangle,10 ),
                                          b.and( tangle,1023 ),
                                          get.short( ff ) )
          end
          let garbage = get.long( ff )
     
          let total.height = height.of( font.info )
     
          let the.ch = image 768 by height * total.height of on
          let the.bits = plane.of( the.ch,0 )
          let cnt := 0 ; let Upb = upb( the.bits )
          while cnt < Upb and ~eof( ff ) do
          begin
               ! -1 start pos of bmap
               the.bits( cnt + the.bits( -1 ) - 6 ) := get.long( ff )
               cnt := cnt + 1
          end
     
          let final = vector 0 :: 127 of constant.image( image 1 by height of on )
          for next = 0 to 127 do
          if font.info( next,font.width ) ~= 0 do
          begin
               let char = image font.info( next,font.width ) by height of off
               copy limit the.ch at font.info( next,font.offset ),
                                    Y.dim( the.ch ) - ( font.info( next,font.lno ) + 1 ) * height 
               onto char
               final( next ) := constant.image( char )
          end
     
          font( font.name,height,base,final,nil )
     end
     procedure font.lookup( cstring font.name -> pntr )
     begin
          let list := fonts.pntr
          let continue := true
          while list ~= nil and continue do 
          if font.name = list( the.fontname ) then continue := false
          else list := list( next.font )
          if list ~= nil then list else
          begin
               let new = make.font.structure( font.name )
               new( next.font ) := fonts.pntr
               fonts.pntr := new
               fonts.pntr
          end
     end
     if the.string = "" then
     begin
          write "S-algol run time error: Empty string passed to string.to.tile'n"
          trace
          abort
          image 1 by 1 of on
     end else
     begin
          let the.font = font.lookup( font.name )
          let our.font = the.font( the.chars )
          let height = the.font( font.height )
          let width := 0
          let no.chars = length( the.string )
          for i = 1 to no.chars do width := width + X.dim( variable.image( our.font( decode( the.string(i|1) ) ) ) )
          let the.image = image width by height of off
          let so.far := 0
          for i = 1 to no.chars do 
          begin
               let next.one = our.font( decode( the.string(i|1) ) )
               copy next.one onto limit the.image at so.far,0
               so.far := so.far + X.dim( variable.image( next.one ) )
          end
          the.image
     end
end
! *********************** Outline code starts here *************************
procedure Odraw.function( pntr draw.type ;
			 pic t ;
			 real xmin,xmax,ymin,ymax )
begin
 
     !*************** Colours Supported by Outline *****************
     
     ! The 4 plane colours for the Tektronix T4107
     let t4107.background   = off & off & off & off
     let t4107.white        = on  & off & off & off
     let t4107.red          = off & on  & off & off
     let t4107.green        = on  & on  & off & off
     let t4107.blue         = off & off & on  & off
     let t4107.cyan         = on  & off & on  & off
     let t4107.magenta      = off & on  & on  & off
     let t4107.yellow       = on  & on  & on  & off
     let t4107.orange       = off & off & off & on
     let t4107.green.yellow = on  & off & off & on
     let t4107.green.cyan   = off & on  & off & on
     let t4107.blue.cyan    = on  & on  & off & on
     let t4107.blue.magenta = off & off & on  & on
     let t4107.red.magenta  = on  & off & on  & on
     let t4107.dark.grey    = off & on  & on  & on
     let t4107.light.grey   = on  & on  & on  & on
     
     ! The 2 plane colours for the Canon CX4800
     let cx4800.black = off & off
     let cx4800.blue  = on  & off
     let cx4800.red   = off & on
     let cx4800.green = on  & on
     
     ! The pens for the 6320
     let g6320.background   = off & off & off & off
     let g6320.black        = on  & off & off & off
     let g6320.red          = off & on  & off & off
     let g6320.blue         = on  & on  & off & off
     let g6320.green        = off & off & on  & off
     let g6320.cyan         = on  & off & on  & off
     let g6320.violet       = off & on  & on  & off
     let g6320.orange       = on  & on  & on  & off
     let g6320.yellow       = off & off & off & on
     let g6320.brown        = on  & off & off & on
     let g6320.magenta      = off & on  & off & on
     let dev.type := ""
     if draw.type is Image.type then dev.type := "image"
     else dev.type := draw.type( Dev.type )
     procedure check.colour( cstring dev.type ; pixel col  -> int  )
     case dev.type of
     "image"  : begin
			let n := 0
			for i = pixel.depth( col ) - 1 to 0 by -1 do
			n := n * 2 +
			( case col( i ) of
                	  on      : 1
                	  default : 0 )
			n
		end
     "t4107"  : begin
                     col :=
                     case pixel.depth( col ) of
                     1       : col & off & off & off
                     2       : col & off & off
                     3       : col & off
                     4       : col
                     default : col( 0 ) & col( 1 ) & col( 2 ) & col( 3 )
     
                     case col of
                     t4107.background   : 0
                     t4107.white        : 1
                     t4107.red          : 2
                     t4107.green        : 3
                     t4107.blue         : 4
                     t4107.cyan         : 5
                     t4107.magenta      : 6
                     t4107.yellow       : 7
                     t4107.orange       : 8
                     t4107.green.yellow : 9
                     t4107.green.cyan   : 10
                     t4107.blue.cyan    : 11
                     t4107.blue.magenta : 12
                     t4107.red.magenta  : 13
                     t4107.dark.grey    : 14
                     default            : 15
                end
     "cx4800" : begin
                     col :=
                     case pixel.depth( col ) of
                     1       : col & off
                     2       : col
                     default : col( 0 ) & col( 1 )
     
                     case col of
                     cx4800.black : 0
                     cx4800.blue  : 1
                     cx4800.red   : 2
                     default      : 3
                end
     "L800"   : case col( 0 ) of
                off     : 1
                default : 0
     "PostScript"
              : case col( 0 ) of
                off     : 1
                default : 0
     "g6320"  : begin
                     col :=
                     case pixel.depth( col ) of
                     1       : col & off & off & off
                     2       : col & off & off
                     3       : col & off
                     4       : col
                     default : col( 0 ) & col( 1 ) & col( 2 ) & col( 3 )
     
                     case col of
                     g6320.background   : 0
                     g6320.black        : 1
                     g6320.red          : 2
                     g6320.blue         : 3
                     g6320.green        : 4
                     g6320.cyan         : 5
                     g6320.violet       : 6
                     g6320.orange       : 7
                     g6320.yellow       : 8
                     g6320.brown        : 9
                     default            : 10
                end
     default  : 0
     procedure Odraw.fn ( ( int,int,int ) point ;
                          ( int,int,int,int,int ) join ;
                          ( string,pixel -> int ) check.colour ;
                          int default.colour ;
                          int Clipx1,Clipx2,Clipy1,Clipy2 ;
                          cpic t ; real xmin,xmax,ymin,ymax )
     begin
          if xmax <= xmin or ymax <= ymin do
          begin
               write Outline.error,
                     "The parameters for DRAW are ",
                     xmin : 7,xmax : 7,ymin : 7,ymax : 7,"'n"
               abort
          end
     
          let points := false ; let del = code( 127 )
          let A = ( Clipx2 - Clipx1 ) / ( xmax - xmin )
          let B = Clipx1 - xmin * A + 0.5
          let C = ( Clipy2 - Clipy1 ) / ( ymax - ymin )
          let D = Clipy1 - ymin * C + 0.5
          let oprn := 0 ; let to.rads = pi/180
          let lastx := 0 ; let lasty := 0
     
          let one = @1 of creal[ 1.0,0.0,0.0 ]
          let two = @1 of creal[ 0.0,1.0,0.0 ]
          let three = @1 of creal[ 0.0,0.0,1.0 ]
     
          !************** tree translation procedures ****************
     
          forward takeapart( cpntr,c*c*creal,cint )
     
          procedure characters( cstring message ; c*c*creal matrix ; creal x1,y1,x2,y2 ; cint col )
          begin
               procedure drchar( cstring chr -> pic )
               begin
                    let bel := 0
                    let k := 2
                    let letr := [ 0,0 ]
                    let c = decode( chr )
                    let t = if c < 77 then 1 else 2
                    let charry.t.c = charry( t,c )
                    let noprs := charry.t.c( 1 )
                    let x := charry.t.c( 2 )
                    let y := charry.t.c( 3 )
     
                    if x = -2 then
                    begin
                          bel := y
                          letr := [ charry.t.c( 4 ),charry.t.c( 5 ) - bel ]
                          k := k + 1
                    end else letr := [ x,y ]
     
                    while k <= noprs do
                    begin
                         let j := k * 2
                         x := charry.t.c( j )
                         y := charry.t.c( j + 1 )
                         if x = -1 then
                         begin
                              j := j + 2
                              letr := letr & [ charry.t.c( j ),charry.t.c( j + 1 ) - bel ]
                              k := k + 2
                         end else
                         begin
                              letr := letr ^ [ x,y - bel ]
                              k := k + 1
                         end
                    end
                    letr
               end  ! ** drchr procedure **
     
               let x.diff = x2 - x1 ; let y.diff = y2 - y1
               let L = sqrt( ( y.diff ) * ( y.diff ) + ( x.diff ) * ( x.diff ) )
               let theta := if 300 * rabs( x.diff ) < rabs( y.diff )
                            then if y.diff > 0 then 90 else -90
                            else atan( ( y.diff )/( x.diff ) ) * ( 180/pi )
               if x.diff < 0 do theta := theta + 180
               let no.chars = length( message )
               let start := 1
               while start <= no.chars and ( message( start|1 ) <= " " or message( start|1 ) = del ) do start := start + 1
     
               if start > no.chars then points := false else
               begin
                    let Text := drchar( message( start|1 ) )
                    start := start + 1
                    for i = start to no.chars do
                    if message( i|1 ) = del or message( i|1 ) <= " " then
                       Text := shift Text by -6,0
                    else
                       Text := ( shift Text by -6,0 ) & drchar ( message( i|1 ) )
                    Text := shift Text by 6 * ( no.chars - 1 ),0
                    Text := scale Text by L/( no.chars * 6 ),L/( no.chars * 6 )
                    Text := rotate Text by -theta
                    Text := shift Text by x1,y1
                    takeapart( pic.pntr( Text ),matrix,col )
               end
          end  ! *** characters procedure ***
     
          procedure matrix.mult( c*c*creal m1,m2 -> *c*creal )
          begin
               let m11 = m1( 1 ) ; let m21 = m2( 1 )
               let m12 = m1( 2 ) ; let m22 = m2( 2 )
               let m13 = m1( 3 ) ; let m23 = m2( 3 )
     
               let m111 = m11( 1 ) ; let m112 = m11( 2 )
               let m121 = m12( 1 ) ; let m122 = m12( 2 )
               let m211 = m21( 1 ) ; let m212 = m21( 2 )
               let m221 = m22( 1 ) ; let m222 = m22( 2 )
               let m131 = m13( 1 ) ; let m132 = m13( 2 )
               let m133 = m13( 3 )
     
               @1 of c*creal[
                 @1 of creal[ m111 * m211 + m112 * m221,
                              m111 * m212 + m112 * m222,0.0 ],
                 @1 of creal[ m121 * m211 + m122 * m221,
                              m121 * m212 + m122 * m222,0.0 ],
                 @1 of creal[ m131 * m211 + m132 * m221 + m133 * m23( 1 ),
                              m131 * m212 + m132 * m222 + m133 * m23( 2 ),1.0 ] ]
          end
     
          procedure takeapart( cpntr tree ; c*c*creal matrix ; cint col )
          case true of
          tree is poin.strc :
          begin
               let m1 = matrix( 1 ) ; let m2 = matrix( 2 ) ; let m3 = matrix( 3 )
               let pntx = truncate( A * ( m1( 1 ) * tree( pnx ) + m2( 1 ) * tree( pny ) + m3( 1 ) ) + B )
               let pnty = truncate( C * ( m1( 2 ) * tree( pnx ) + m2( 2 ) * tree( pny ) + m3( 2 ) ) + D )
               let colo = if col ~= -1 then col else default.colour
               if points and oprn = 2 then
                   if ~ ( lastx = pntx and lasty = pnty ) do
                   begin
                        join( lastx,lasty,pntx,pnty,colo )
                        lastx := pntx ; lasty := pnty
                   end
               else { point( pntx,pnty,colo ) ; lastx := pntx ; lasty := pnty }
               points := true
          end
     
          tree is oprtn.strc :
          begin
               let tpoints := points
               takeapart( tree( lft ),matrix,col )
               if points do oprn := tree( opoo )
               points := tpoints or points ; tpoints := points
               takeapart( tree( rght ),matrix,col )
               points := tpoints or points
          end
     
          tree is trnsfrm.strc :
          case tree( trnsfrm ) of
          1 : takeapart( tree( mrtre ),matrix.mult( @1 of c*creal[
                                       @1 of creal[ tree( trnsfrm.x ),0.0,0.0 ],
                                       @1 of creal[ 0.0,tree( trnsfrm.y ),0.0 ],
                                       three ],matrix ),col )
     
          2 : takeapart( tree( mrtre ),
              matrix.mult( @1 of c*creal[ one,two,
                           @1 of creal[ tree( trnsfrm.x ),tree( trnsfrm.y ),1.0 ] ],
                           matrix ),col )
     
          3 : takeapart( tree( mrtre ),
              matrix.mult( @1 of c*creal[
              @1 of creal[ cos( tree( trnsfrm.x )*to.rads ),-sin( tree( trnsfrm.x )*to.rads ),0.0 ],
              @1 of creal[ sin( tree( trnsfrm.x )*to.rads ),cos( tree( trnsfrm.x )*to.rads ),0.0 ],
              three ],matrix ),col )
          default : {}
     
          tree is culr.strc:
          takeapart( tree( nxtre ),matrix,if col ~= -1 then col else check.colour( dev.type,tree( shade ) ) )
     
          tree is scrbl.strc : 
          if  tree( xxl ) = tree( xxr ) and tree( yyl ) = tree( yyr ) then
                 write "'n*** Outline run-time error ***",
                       "'nThe points in a text statement must be distinct.",
                       "'nx1 :-",tree( xxl )," y1 :-",tree( yyl )," x2 :-",tree( xxr )," y2 :-",tree( yyr )
          else
          characters( tree( msge ),matrix,tree( xxl ),tree( yyl ),tree( xxr ),tree( yyr ),col )
     
         default : {}
     
         takeapart( pic.pntr( t ),@1 of c*creal[ one,two,three ],-1 )
     end ! ** of drawing function **
      
!**************************** Main bit starts here *************************
     ! the dimenions and drawing area.
     let Clipx1 := case dev.type of
                   "L800"          : 400         ! 1/1000ths inch
                   "PostScript"    : 29          ! 1/72nds inch
                   default         : 0
     let Clipx2 := case dev.type of
                   "image" : 0
                   "t4010","t4006" : 1023
                   "t4107"         : 4095
                   "cx4800"        : 1919
                   "L800"          : 7800        ! 1/1000ths inch
                   "PostScript"    : 562         ! 1/72nds inch
                   "g6320"         : 14400       ! I hope this is correct
                   default         : 0
     let Clipy1 := case dev.type of
                   "L800"          : 400         ! 1/1000ths inch
                   "PostScript"    : 29          ! 1/72nds inch
                   default         : 0
     let Clipy2 := case dev.type of
                   "image" : 0
                   "t4010","t4006" : 779
                   "t4107"         : 3132
                   "cx4800"        : 2039
                   "L800"          : 11300       ! 1/1000ths inch
                   "PostScript"    : 814         ! 1/72nds inch
                   "g6320"         : 11200       ! I hope this is correct
                   default         : 0
     
     let default.colour :=  case dev.type of
                            "image","t4107","g6320" : 1
                            default                 : 0
     
     let drawing.colour := default.colour
     
     ! two variables to hold device being drawn on.
     let the.image := image 1 by 1 of on
     let the.file := nullfile
        
     procedure inttopixel( int i -> pixel )
     begin
         let pix := if b.and( i,1 ) = 1 then on else off 
         for j = 1 to 7 do
         pix := pix & ( if b.and( shift.r( i,j ),1 ) = 1 then on else off )
          pix
     end

     case dev.type of
     "image" :
     begin    
          procedure point( int x1,y1,col )
          pnx.line( the.image,x1,y1,x1 + 1,y1,inttopixel( col ),0 )
     
          procedure join( int x1,y1,x2,y2,col )
          pnx.line( the.image,x2,y2,x1,y1,inttopixel( col ),0 )
          the.image := draw.type( The.image )
          Odraw.fn( point,join,
                    check.colour,default.colour,
                    0,X.dim( the.image ) - 1,0,Y.dim( the.image ) - 1,
                    t,xmin,xmax,ymin,ymax )
          the.image := image 1 by 1 of on
     end
     "t4006","t4010" :
     begin
          let cr = 13 ; let nul = 0 ; let gs = 29
          let hix := -127 ; let hiy := hix ; let loy := hix
     
          procedure out( int n )
          out.byte the.file,n,0
     
          procedure curve( int x,y ; bool new,point )
          begin
               if new do out( gs )
               let HIY = ( y div 32 ) + 32
               let LOY = ( y rem 32 ) + 96
               let HIX = ( x div 32 ) + 32
               let LOX = ( x rem 32 ) + 64
               let cas := 0
               cas := cas + { if HIY = hiy then 0 else { hiy := HIY ; 1 } }
               cas := cas + { if HIX = hix then 0 else { hix := HIX ; 4 } }
               cas := cas + { if LOY = loy then 0 else { loy := LOY ; 2 } }
               case cas of
               0  : { out( LOX ) ; out( nul ) }
               1  : { out( HIY ) ; out( LOX ) ; out( nul ) }
               2  : { out( LOY ) ; out( LOX ) ; out( nul ) }
               3  : { out( HIY ) ; out( LOY ) ; out( LOX ) ; out( nul ) }
               4,6 : { out( LOY ) ; out( HIX ) ; out( LOX ) ; out( nul ) }
               default : { out( HIY ) ; out( LOY ) ; out( HIX ) ; out( LOX ) ; out( nul ) }
               if point do { out( LOX ) ; out( nul ) }
          end
     
     
          procedure point( cint x1,y1 ; cint col )
          if x1 >= Clipx1 and x1 <= Clipx2 and y1 >= Clipy1 and y1 <= Clipy2 do
          curve( x1,y1,true,true )
     
          procedure join( int x1,y1,x2,y2 ; cint col )
          begin
               procedure Code( cint x,y -> int )
               begin
                    let c := if x < Clipx1 then 8 else
                             if x > Clipx2 then 4 else 0
                    if y < Clipy1 then c + 2 else
                    if y > Clipy2 then c + 1 else c
               end
               let c1 := Code( x1,y1 ) ; let c2 := Code( x2,y2 )
               while ( c1 ~= 0 or c2 ~= 0 ) and b.and( c1,c2 ) = 0 do
               begin
                    let c := if c1 = 0 then c2 else c1
                    let x := 0 ; let y := 0
                    case true of
                    b.and( c,8 ) = 8 :
                    begin
                         x := Clipx1
                         y := y1 + (y2-y1) * (Clipx1-x1) div (x2-x1)
                    end
                    b.and( c,4 ) = 4 :
                    begin
                         x := Clipx2
                         y := y1 + (y2-y1) * (Clipx2-x1) div (x2-x1)
                    end
                    b.and( c,2 ) = 2 :
                    begin
                         y := Clipy1
                         x := x1 + (x2-x1) * (Clipy1-y1) div (y2-y1)
                    end
                    default :
                    begin
                         y := Clipy2
                         x := x1 + (x2-x1) * (Clipy2-y1) div (y2-y1)
                    end
                    if c = c1 then
                    { x1 := x ; y1 := y ; c1 := Code(x,y) } else
                    { x2 := x ; y2 := y ; c2 := Code(x,y) }
               end
               if b.and( c1,c2 ) = 0 do
               begin
                    curve( x2,y2,false,false )
               end
          end
          the.file := draw.type( The.file )
          Odraw.fn( point,join,
                    check.colour,default.colour,
                    Clipx1,Clipx2,Clipy1,Clipy2,
                    t,xmin,xmax,ymin,ymax )
          point( 0,900,default.colour )
          output the.file,code( 31 )
          flush( the.file )
          the.file := nullfile
     end
     "t4107" :
     begin
          let cr = 13 ; let nul = 0 ; let gs = 29
          let hix := -127 ; let hiy := hix ; let loy := hix ; let xtr := hix
     
          procedure out( int n )
          out.byte the.file,n,0
     
          procedure put( int n )
          begin
               let neg = n < 0
               n := abs( n )
               if n div 1024 > 0 do out( 64 + n div 1024 )
               if n div 16 > 0 do out( 64 + n rem 1024 div 16 )
               out( 32 + n rem 16 + ( if neg then 0 else 16 ) )
          end
     
          procedure curve( int x,y ; bool new,point ; cint col )
          begin
               if col ~= drawing.colour do
               begin
                    out( 31 )
                    out( 27 )
                    out( decode( "M" ) )
                    out( decode( "L" ) )
                    put( col )
                    out( 0 )
                    drawing.colour := col
                    new := true
               end
               if new do out( gs )
     
               let HIY = ( y div 128 ) + 32
               let LOY = ( y rem 128 div 4 ) + 96
               let HIX = ( x div 128 ) + 32
               let LOX = ( x rem 128 div 4 ) + 64
               let XTR = ( y rem 4 ) * 4 + ( x rem 4 ) + 96
               let cas := 0
               cas := cas + { if HIY = hiy then 0 else { hiy := HIY ; 1 } }
               cas := cas + { if LOY = loy then 0 else { loy := LOY ; 2 } }
               cas := cas + { if HIX = hix then 0 else { hix := HIX ; 4 } }
               cas := cas + { if XTR = xtr then 0 else { xtr := XTR ; 8 } }
               case cas of
               0     : { out( LOX ) ; out( nul ) }
               1     : { out( HIY ) ; out( LOX ) ; out( nul ) }
               2     : { out( LOY ) ; out( LOX ) ; out( nul ) }
               3     : { out( HIY ) ; out( LOY ) ; out( LOX ) ; out( nul ) }
               4,6   : { out( LOY ) ; out( HIX ) ; out( LOX ) ; out( nul ) }
               5,7   : { out( HIY ) ; out( LOY ) ; out( HIX ) ; out( LOX ) ; out( nul ) }
               8,10  : { out( XTR ) ; out( LOY ) ; out( LOX ) ; out( nul ) }
               9,11  : { out( HIY ) ; out( XTR ) ; out( LOY ) ; out( LOX ) ; out( nul ) }
               12,14 : { out( XTR ) ; out( LOY ) ; out( HIX ) ; out( LOX ) ; out( nul ) }
               default : { out( HIY ) ; out( XTR ) ; out( LOY ) ; out( HIX ) ; out( LOX ) ; out( nul ) }
               if point do { out( LOX ) ; out( nul ) }
          end
     
     
          procedure point( cint x1,y1 ; cint col )
          if x1 >= Clipx1 and x1 <= Clipx2 and y1 >= Clipy1 and y1 <= Clipy2 do
          curve( x1,y1,true,true,col )
     
          procedure join( int x1,y1,x2,y2 ; cint col )
          begin
               procedure Code( cint x,y -> int )
               begin
                    let c := if x < Clipx1 then 8 else
                             if x > Clipx2 then 4 else 0
                    if y < Clipy1 then c + 2 else
                    if y > Clipy2 then c + 1 else c
               end
               let c1 := Code( x1,y1 ) ; let c2 := Code( x2,y2 )
               while ( c1 ~= 0 or c2 ~= 0 ) and b.and( c1,c2 ) = 0 do
               begin
                    let c := if c1 = 0 then c2 else c1
                    let x := 0 ; let y := 0
                    case true of
                    b.and( c,8 ) = 8 :
                    begin
                         x := Clipx1
                         y := y1 + (y2-y1) * (Clipx1-x1) div (x2-x1)
                    end
                    b.and( c,4 ) = 4 :
                    begin
                         x := Clipx2
                         y := y1 + (y2-y1) * (Clipx2-x1) div (x2-x1)
                    end
                    b.and( c,2 ) = 2 :
                    begin
                         y := Clipy1
                         x := x1 + (x2-x1) * (Clipy1-y1) div (y2-y1)
                    end
                    default :
                    begin
                         y := Clipy2
                         x := x1 + (x2-x1) * (Clipy2-y1) div (y2-y1)
                    end
                    if c = c1 then
                    { x1 := x ; y1 := y ; c1 := Code(x,y) } else
                    { x2 := x ; y2 := y ; c2 := Code(x,y) }
               end
               if b.and( c1,c2 ) = 0 do
               begin
                    curve( x2,y2,false,false,col )
               end
         end
         drawing.colour := -1
         the.file := draw.type( The.file )
         Odraw.fn( point,join,
                   check.colour,default.colour,
                   Clipx1,Clipx2,Clipy1,Clipy2,
                   t,xmin,xmax,ymin,ymax )
         output the.file,code( 31 )
         flush( the.file )
         the.file := nullfile
     end
     "cx4800" :
     begin
          procedure out( cint n ) ; out.byte the.file,n,0
          procedure outs( string s )
          begin
               let len = length( s )
               if len > 0 do
               for i = 1 to len do out( decode( s(i|1) ) )
          end
          procedure point( cint x1,y1 ; cint col )
          begin                          
!++
!  ************    Warning!! This is most likely wrong ******************
!  ************    See PS-algol outline module for more details **********
!--
               ! set colour
               outs( "C" ++ iformat (col) ++ "'n" )
               ! move to point - don't plot anything
               if x1 >= Clipx1 and x1 <= Clipx2 and
                  y1 >= Clipy1 and y1 <= Clipy2 do
                  outs( "M" ++ iformat (x1) ++ "," ++ iformat (y1) ++ "'n" )
          end
 
          procedure join( int x1,y1,x2,y2 ; cint col )
          begin
               procedure Code( cint x, y -> int )
               begin
                    let c := case true of
                             x < Clipx1 : 8
                             x > Clipx2 : 4
                             default    : 0
                    case true of
                    y < Clipy1 : c + 2
                    y > Clipy2 : c + 1
                    default    : c
               end
               let c1 := Code( x1,y1 )
               let c2 := Code( x2,y2 )
               while ( c1 ~= 0 or c2 ~= 0 ) and b.and( c1,c2 ) = 0 do
               begin
                    let c := if c1 = 0 then c2 else c1
                    let x := 0 ; let y := 0
                    case true of
                    b.and( c,8 ) = 8 : begin
                                            x := Clipx1
                                            y := y1 + ( y2 - y1 ) * ( Clipx1 - x1 ) div ( x2 - x1 )
                                       end
                    b.and( c,4 ) = 4 : begin
                                            x := Clipx2
                                            y := y1 + ( y2 - y1 ) * ( Clipx2 - x1 ) div ( x2 - x1 )
                                       end
                    b.and( c,2 ) = 2 : begin
                                            y := Clipy1
                                            x := x1 + ( x2 - x1 ) * ( Clipy1 - y1 ) div ( y2 - y1 )
                                       end
                    default          : begin
                                            y := Clipy2
                                            x := x1 + ( x2 - x1 ) * ( Clipy2 - y1 ) div ( y2 - y1 )
                                       end
                    if c = c1 then { x1 := x ; y1 := y ; c1 := Code( x,y ) }
                    else { x2 := x ; y2 := y ; c2 := Code( x,y ) }
               end
               if b.and( c1,c2 ) = 0 do
               begin
                    ! set colour
	            outs( "C" ++ iformat( col ) ++ "'n" ++ "'n" )
                    ! move to one endpoint
                    outs( "M" ++ iformat( x1 ) ++ "," ++ iformat( y1 ) ++ "'n" )
                    ! draw to other endpoint
                    outs( "J" ++ iformat( x2 - x1 ) ++
                          "," ++ iformat( y2 - y1 ) ++ "'n" )
               end
          end
         the.file := draw.type( The.file )
         Odraw.fn( point,join,
                   check.colour,default.colour,
                   Clipx1,Clipx2,Clipy1,Clipy2,
                   t,xmin,xmax,ymin,ymax )
         flush( the.file )
         the.file := nullfile
     end
     "L800" :
     begin
          procedure convert( cint num -> string )
          begin
               let converted = iformat( num )
               case length( converted ) of
               1       : "0000" ++ converted
               2       : "000" ++ converted
               3       : "00" ++ converted
               4       : "0" ++ converted
               5       : converted
               default : begin
                              write Outline.error,
                                     "Internal error converting to QUIC format'n"
                               abort
                               ""
                          end
          end
          procedure point( cint x1,y1 ; cint col ) 
          if x1 >= Clipx1 and x1 <= Clipx2 and y1 >= Clipy1 and y1 <= Clipy2 do
               output the.file,"^U",convert( x1 ),":",convert( 11300 - y1 ),"^D:"
 
          procedure join( int x1,y1,x2,y2 ; cint col )
          begin
               procedure Code( cint x, y -> int )
               begin
                    let c := case true of
                             x < Clipx1 : 8
                             x > Clipx2 : 4
                             default    : 0
                    case true of
                    y < Clipy1 : c + 2
                    y > Clipy2 : c + 1
                    default    : c
               end
               let c1 := Code( x1,y1 )
               let c2 := Code( x2,y2 )
               while ( c1 ~= 0 or c2 ~= 0 ) and b.and( c1,c2 ) = 0 do
               begin
                    let c := if c1 = 0 then c2 else c1
                    let x := 0 ; let y := 0
                    case true of
                    b.and( c,8 ) = 8 : begin
                                            x := Clipx1
                                            y := y1 + ( y2 - y1 ) * ( Clipx1 - x1 ) div ( x2 - x1 )
                                       end
                    b.and( c,4 ) = 4 : begin
                                            x := Clipx2
                                            y := y1 + ( y2 - y1 ) * ( Clipx2 - x1 ) div ( x2 - x1 )
                                       end
                    b.and( c,2 ) = 2 : begin
                                            y := Clipy1
                                            x := x1 + ( x2 - x1 ) * ( Clipy1 - y1 ) div ( y2 - y1 )
                                       end
                    default          : begin
                                            y := Clipy2
                                            x := x1 + ( x2 - x1 ) * ( Clipy2 - y1 ) div ( y2 - y1 )
                                       end
                    if c = c1 then { x1 := x ; y1 := y ; c1 := Code( x,y ) }
                    else { x2 := x ; y2 := y ; c2 := Code( x,y ) }
               end
               if b.and( c1,c2 ) = 0 do
               begin
                    output the.file,"^U",convert( x1 ),":", convert( 11300 - y1 )    ! move to one endpoint
	            output the.file,if col = 0 then "^D" else "^E"                   ! set col the draw or erase
                    output the.file,convert( x2 ),":",convert( 11300 - y2 )          ! draw to other endpoint 
               end   
          end
         the.file := draw.type( The.file )
         output draw.type( The.file ),"^PY^-'o"                        ! Quic on
         output draw.type( The.file ),"^IGV"                           ! Enter vector graphics mode
         output draw.type( The.file ),"^PW01"                          ! line width 1 pixel                   
         Odraw.fn( point,join,
                   check.colour,default.colour,
                   Clipx1,Clipx2,Clipy1,Clipy2,
                   t,xmin,xmax,ymin,ymax )
         output draw.type( The.file ),"^-'o"                           ! terminator                           
         output draw.type( The.file ),"^IGE"                           ! Exit vector graphics mode            
         output draw.type( The.file ),"^PN"                            ! Quic off 
         flush( the.file )
         the.file := nullfile
     end
     "PostScript" :
     begin
          procedure point( cint x1,y1 ; cint col ) 
          if x1 >= Clipx1 and x1 <= Clipx2 and y1 >= Clipy1 and y1 <= Clipy2 do
               output the.file,iformat( x1 )," ",iformat( y1 )," moveto'n",
                               iformat( x1 )," ",iformat( y1 )," lineto'n"
 
          procedure join( int x1,y1,x2,y2 ; cint col )
          begin
               procedure Code( cint x, y -> int )
               begin
                    let c := case true of
                             x < Clipx1 : 8
                             x > Clipx2 : 4
                             default    : 0
                    case true of
                    y < Clipy1 : c + 2
                    y > Clipy2 : c + 1
                    default    : c
               end
               let c1 := Code( x1,y1 )
               let c2 := Code( x2,y2 )
               while ( c1 ~= 0 or c2 ~= 0 ) and b.and( c1,c2 ) = 0 do
               begin
                    let c := if c1 = 0 then c2 else c1
                    let x := 0 ; let y := 0
                    case true of
                    b.and( c,8 ) = 8 : begin
                                            x := Clipx1
                                            y := y1 + ( y2 - y1 ) * ( Clipx1 - x1 ) div ( x2 - x1 )
                                       end
                    b.and( c,4 ) = 4 : begin
                                            x := Clipx2
                                            y := y1 + ( y2 - y1 ) * ( Clipx2 - x1 ) div ( x2 - x1 )
                                       end
                    b.and( c,2 ) = 2 : begin
                                            y := Clipy1
                                            x := x1 + ( x2 - x1 ) * ( Clipy1 - y1 ) div ( y2 - y1 )
                                       end
                    default          : begin
                                            y := Clipy2
                                            x := x1 + ( x2 - x1 ) * ( Clipy2 - y1 ) div ( y2 - y1 )
                                       end
                    if c = c1 then { x1 := x ; y1 := y ; c1 := Code( x,y ) }
                    else { x2 := x ; y2 := y ; c2 := Code( x,y ) }
               end
               if b.and( c1,c2 ) = 0 do
               begin
                    output the.file,iformat( x1 )," ",iformat( y1 )," moveto'n"    ! move to one endpoint
	            !output the.file,if col = 0 then "^D" else "^E"                   ! set col the draw or erase
                    output the.file,iformat( x2 )," ",iformat( y2 )," lineto'n"          ! draw to other endpoint 
               end   
          end
         the.file := draw.type( The.file )
         output draw.type( The.file ),"%!PS-Adobe-2.0'n"                      ! start new path
         output draw.type( The.file ),"newpath'n"                      ! start new path
         output draw.type( The.file ),"0 setlinewidth'n"               ! line width as small as possible
         Odraw.fn( point,join,
                   check.colour,default.colour,
                   Clipx1,Clipx2,Clipy1,Clipy2,
                   t,xmin,xmax,ymin,ymax )
         output draw.type( The.file ),"stroke'n"                       ! draw the path
         output draw.type( The.file ),"showpage'n"                     ! output the page
         flush( the.file )
         the.file := nullfile
     end
     "g6320" :
     begin
          let current.x := -1 ; let current.y := -1
          s.w := 0
          procedure point( cint x1,y1 ; cint col ) 
                   begin
                        if current.x ~= x1 or current.y ~= y1 do
                        begin
                             current.x := x1 ; current.y := y1
                             output the.file,"PU",x1:1,",",y1:1,";"
                        end
                        if col ~= drawing.colour do
                             { output the.file,"SP",col:1,";" ; drawing.colour := col }
                        output the.file,"PD;"
                   end
          procedure join( int x1,y1,x2,y2 ; cint col )
                  begin
                       if current.x ~= x1 or current.y ~= y1 do
                            output the.file,"PU",x1:1,",",y1:1,";"
                       if col ~= drawing.colour do
                            { output the.file,"SP",col:1,";" ; drawing.colour := col }
                       output the.file,"PD",x2:1,",",y2:1,";"
                       current.x := x2 ; current.y := y2
                  end
         the.file := draw.type( The.file )
         output draw.type( The.file ),"SP0;"                        ! Drop any pen                            
         drawing.colour := 0
         Odraw.fn( point,join,
                   check.colour,default.colour,
                   Clipx1,Clipx2,Clipy1,Clipy2,
                   t,xmin,xmax,ymin,ymax )
         output the.file,"SP0;PU0,0;"        ! Drop pen, go to (0,0) and form feed        
         flush( the.file )
         the.file := nullfile
     end
     default : {}
end ! of make.draw.fn
     
!structure file.erase( proc( file,pixel ) Ferase )  
!structure image.erase( proc( #pixel,pixel ) Ierase )
!     
!let make.erase.fn = proc( string dev.type -> pntr )
!begin
!     let check.colour = check.colour( dev.type )
!
!     case dev.type of
!     "image"  :
!     image.erase( proc( #pixel i ; pixel col )
!                  case check.colour( col ) of
!                  0       : xor i onto i
!                  default : xnor i onto i )
!     "t4010","t4006" :
!     file.erase( proc( file f ; pixel col )
!                 begin
!                      let erase.delay = 250
!     
!                      let out = proc( cint n )
!                      out.byte f,n,0
!     
!                      out( 27 )
!                      out( 12 )
!                      for i = 1 to erase.delay do out( 0 )
!                      flush( f )
!                 end )
!     "t4107" :
!     file.erase( proc( file f ; pixel col )
!                 begin
!                      let erase.delay = 250
!     
!                      let out = proc( cint n )
!                      out.byte f,n,0
!     
!                      let put = proc( int n )
!                      begin
!                           let neg = n < 0
!                           n := abs( n )
!                           if n div 1024 > 0 do out( 64 + n div 1024 )
!                           if n div 16 > 0 do out( 64 + n rem 1024 div 16 )
!                           out( 32 + n rem 16 + ( if neg then 0 else 16 ) )
!                      end
!     
!                      out( 27 )
!                      out( 12 )
!                      for i = 1 to erase.delay do out( 0 )
!     
!                      out( 27 )
!                      out( decode( "T" ) )
!                      out( decode( "B" ) )
!
!                      case check.colour( col ) of
!                      1       : { put( 0 ) ; put( 100 ) ; put( 0 ) }
!                      2       : { put( 120 ) ; put( 50 ) ; put( 100 ) }
!                      3       : { put( 240 ) ; put( 50 ) ; put( 100 ) }
!                      4       : { put( 0 ) ; put( 50 ) ; put( 100 ) }
!                      5       : { put( 300 ) ; put( 50 ) ; put( 100 ) }
!                      6       : { put( 60 ) ; put( 50 ) ; put( 100 ) }
!                      7       : { put( 180 ) ; put( 50 ) ; put( 100 ) }
!                      8       : { put( 150 ) ; put( 50 ) ; put( 100 ) }
!                      9       : { put( 210 ) ; put( 50 ) ; put( 100 ) }
!                      10      : { put( 270 ) ; put( 50 ) ; put( 100 ) }
!                      11      : { put( 330 ) ; put( 50 ) ; put( 100 ) }
!                      12      : { put( 30 ) ; put( 50 ) ; put( 100 ) }
!                      13      : { put( 90 ) ; put( 50 ) ; put( 100 ) }
!                      14      : { put( 0 ) ; put( 33 ) ; put( 0 ) }
!                      15      : { put( 0 ) ; put( 66 ) ; put( 0 ) }
!                      default : { put( 0 ) ; put( 0 ) ; put( 0 ) }
!
!                      for i = 1 to erase.delay do out( 0 )
!     
!                      flush( f )
!                 end )
!     "cx4800"        :
!     file.erase( proc( file f ; pixel col )
!                 begin
!                      let out = proc( cint n ) ; out.byte f,n,0
!       
!                      out( 28 ) ! set to print mode
!                      out( 27 ) ! set to ...
!                      out( 34 ) ! ... unlimited line
!                      out( 12 ) ! emit form feed
!                      out( 30 ) ! reset to graphic mode
!                      out( 67 ) ! "C"
! 
!                      out( 48 + check.colour( col ) )
!                      out( 10 ) ! newline
!                      flush( f )
!                 end )
!     "L800"        :
!     file.erase( proc( file f ; pixel col )
!                 begin
!                      output f,"^,"           ! page throw
!                      flush( f )
!                 end )
!     default         : nil
!end
procedure OdumpPS( c#pixel the.bits ; cstring target.file ; cint dpi )
begin
     let hexdigit = @0 of string[ "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F" ]
     procedure hex( int n,digs -> string )
     begin
          let r := ""
          for i = 1 to digs do
               r := r ++ hexdigit( b.and( shift.r( n,32 - i * 4 ),15 ) )
          r
     end

     let pagewidth = 562 - 29 ! 72nds of inch
     let pageheight = 814 - 29 ! 72nds of inch
     let xdim = X.dim( the.bits )
     let width := xdim * 72.0 / float( dpi )
     if width > pagewidth do width := pagewidth
     let Xoffset = truncate( pagewidth - width ) div 2
     let ydim = Y.dim( the.bits )
     let height := ydim * 72.0 / float( dpi )
     if height > pageheight do height := pageheight
     let Yoffset = truncate( pageheight - height ) div 2
     let Ystep = height / ydim
     let slen = ( xdim + 31 ) div 32                  ! slen is no. of 32bit words per PS-algol scan line
     let plen = xdim div 32                           ! plen is no. of complete 32bit words per postscript scan line
     let lastbits = ( ( xdim rem 32 ) + 7 ) div 8 * 2 ! num of hex chars to make up last bits in last 32bit word
     let slunit := 32768 div ( plen * 8 + lastbits )  ! num of scanlines per 32K string
     if slunit > ydim do slunit := ydim               ! can't do more scan lines than there are?
     let lastunit = ydim rem slunit                   ! num of scan lines left in last unit to be drawn
     let f = create( target.file,486 )
     if f = nullfile do 
     begin
          write "Sorry cannot open the file: ",target.file,"'n"
          abort
     end
     output f,"/concatprocs'n"
     output f,"{'n"
     output f,"/proc2 exch cvlit def'n"
     output f,"/proc1 exch cvlit def'n"
     output f,"'n"
     output f,"/newproc proc1 length proc2 length add array def'n"
     output f,"newproc 0 proc1 putinterval'n"
     output f,"newproc proc1 length proc2 putinterval'n"
     output f,"newproc cvx'n"
     output f,"} def'n"
     output f,"'n"
     output f,"{1 exch sub} currenttransfer concatprocs settransfer'n"
     output f,"/dosl'n"
     output f,"{'n"
     output f,"gsave'n"
     output f,"/bits exch def'n"
     output f,"/yoffs exch def'n"
     output f,"/ystep exch def'n"
     output f,"/ydim exch def'n"
     output f,29 + Xoffset:0,"yoffs translate'n"
     output f,width:0,"ystep scale'n"
     output f,xdim:0,"ydim 1'n"
     output f,"[ ",xdim:0,"0 0 ydim 0 0 ]'n"
     output f,"{ bits }'n"
     output f,"image'n"
     output f,"grestore'n"
     output f,"} def'n"
     procedure dumpPlane( *int bits )
     begin
          procedure scanlines( int scanl,slunit )
          begin
               output f,slunit:0
               output f,Ystep * slunit:0
               output f,29 + Yoffset + Ystep * ( ydim - 1 - scanl ):0
               output f,"<"
               for i = 0 to slunit - 1 do
               begin
                    let start = ( scanl - i ) * slen + 1
                    for i = 0 to plen - 1 do
                         output f,hex( bits( start + i ),8 )
                    if lastbits ~= 0 do
                         output f,hex( bits( start + plen ),lastbits )
                    output f,"'n"
               end
               output f,"> dosl'n"
          end
          for i = ydim - 1 to slunit - 1 by -slunit do
               scanlines( i,slunit )
          if lastunit ~= 0 do
               scanlines( lastunit - 1,lastunit )
     end
     dumpPlane( plane.of( the.bits,0 ) )
     output f,"showpage'n"
     close( f )
end
! **********  The character co-ordinates vector  **********
procedure Ofrst( -> *c*cint )
@33 of c*cint[ 
     @1 of cint[ 11,2,1,2,0,1,0,1,1,2,1,-1,0,1,2,1,7,2,7,2,2,1,2 ],
     @1 of cint[ 5,0,5,1,7,-1,0,2,7,1,5 ],
     @1 of cint[ 11,0,2,4,2,-1,0,2,0,3,6,-1,0,4,4,0,4,-1,0,2,6,1,0 ],
     @1 of cint[ 16,0,2,1,1,3,1,4,2,4,3,0,4,0,5,1,6,3,6,4,5,-1,0,3,7,2,0,-1,0,1,0,2,7 ],
     @1 of cint[ 14,0,0,4,7,-1,0,1,7,0,7,0,6,1,6,1,7,-1,0,3,1,4,1,4,0,3,0,3,1 ],
     @1 of cint[ 12,4,2,2,0,1,0,0,1,0,2,3,5,3,6,2,7,1,7,0,6,0,5,4,0 ],
     @1 of cint[ 2,0,5,1,7 ],
     @1 of cint[ 4,4,0,3,2,3,5,4,7 ],
     @1 of cint[ 4,0,0,1,2,1,5,0,7 ],
     @1 of cint[ 11,2,0,2,4,-1,0,0,2,4,2,-1,0,4,4,0,0,-1,0,0,4,4,0 ],
     @1 of cint[ 5,2,0,2,4,-1,0,0,2,4,2 ],
     @1 of cint[ 7,-2,2,1,0,2,2,2,3,1,3,1,2,2,2 ],
     @1 of cint[ 2,0,2,4,2 ],
     @1 of cint[ 5,1,0,2,0,2,1,1,1,1,0 ],
     @1 of cint[ 2,4,7,0,0 ],
     @1 of cint[ 9,1,0,0,2,0,5,1,7,3,7,4,5,4,2,3,0,1,0 ],
     @1 of cint[ 3,2,0,2,7,1,6 ],
     @1 of cint[ 8,4,0,0,0,0,1,4,4,4,6,3,7,1,7,0,6 ],
     @1 of cint[ 9,0,1,1,0,3,0,4,1,4,3,3,4,1,4,4,7,0,7 ],
     @1 of cint[ 6,3,0,3,4,-1,0,4,2,0,2,2,7 ],
     @1 of cint[ 8,0,0,3,0,4,1,4,3,3,4,0,4,0,7,4,7 ],
     @1 of cint[ 10,3,7,0,3,0,1,1,0,3,0,4,1,4,3,3,4,1,4,0,3 ],
     @1 of cint[ 6,1,0,1,2,2,4,4,6,4,7,0,7 ],
     @1 of cint[ 16,1,4,0,3,0,1,1,0,3,0,4,1,4,3,3,4,1,4,0,5,0,6,1,7,3,7,4,6,4,5,3,4 ],
     @1 of cint[ 10,3,3,1,3,0,4,0,6,1,7,3,7,4,6,4,4,3,3,1,0 ],
     @1 of cint[ 11,1,0,2,0,2,1,1,1,1,0,-1,0,1,2,1,3,2,3,2,2,1,2 ],
     @1 of cint[ 13,-2,2,1,4,1,5,2,5,2,4,1,4,-1,0,1,0,2,2,2,3,1,3,1,2,2,2 ],
     @1 of cint[ 3,4,4,0,2,4,0 ],
     @1 of cint[ 5,0,1,4,1,-1,0,4,3,0,3 ],
     @1 of cint[ 3,0,4,4,2,0,0 ],
     @1 of cint[ 14,0,6,1,7,3,7,4,6,4,5,1,3,1,2,2,2,-1,0,2,1,2,0,1,0,1,1,2,1 ],
     @1 of cint[ 16,4,1,3,0,1,0,0,1,0,4,1,5,3,5,4,4,4,2,3,1,3,4,2,4,1,3,1,2,2,1,3,2 ],
     @1 of cint[ 8,0,0,1,3,2,7,3,3,4,0,-1,0,3,3,1,3 ],
     @1 of cint[ 12,0,4,0,0,3,0,4,1,4,3,3,4,0,4,0,7,3,7,4,6,4,5,3,4 ],
     @1 of cint[ 8,4,1,3,0,1,0,0,1,0,6,1,7,3,7,4,6 ],
     @1 of cint[ 7,0,0,0,7,3,7,4,6,4,1,3,0,0,0 ],
     @1 of cint[ 8,4,0,0,0,0,4,4,4,-1,0,0,4,0,7,4,7 ],
     @1 of cint[ 7,0,0,0,4,4,4,-1,0,0,4,0,7,4,7 ],
     @1 of cint[ 9,3,3,4,3,4,0,1,0,0,1,0,6,1,7,3,7,4,6 ],
     @1 of cint[ 8,0,0,0,7,-1,0,0,4,4,4,-1,0,4,7,4,0 ],
     @1 of cint[ 8,1,0,3,0,-1,0,2,0,2,7,-1,0,1,7,3,7 ],
     @1 of cint[ 6,0,2,0,1,1,0,3,0,4,1,4,7 ],
     @1 of cint[ 6,0,0,0,7,-1,0,4,7,0,4,4,0 ],
     @1 of cint[ 3,0,7,0,0,4,0 ] ]
procedure Oscnd( -> *c*cint )
@77 of c*cint[
     @1 of cint[ 5,0,0,0,7,2,3,4,7,4,0 ],
     @1 of cint[ 4,0,0,0,7,4,0,4,7 ],
     @1 of cint[ 9,1,0,0,1,0,6,1,7,3,7,4,6,4,1,3,0,1,0 ],
     @1 of cint[ 7,0,0,0,7,3,7,4,6,4,4,3,3,0,3 ],
     @1 of cint[ 12,1,0,0,1,0,6,1,7,3,7,4,6,4,1,3,0,1,0,-1,0,4,0,2,2 ],
     @1 of cint[ 10,0,0,0,7,3,7,4,6,4,4,3,3,0,3,-1,0,2,3,4,0 ],
     @1 of cint[ 12,0,1,1,0,3,0,4,1,4,3,3,4,1,4,0,5,0,6,1,7,3,7,4,6 ],
     @1 of cint[ 5,2,0,2,7,-1,0,0,7,4,7 ],
     @1 of cint[ 6,0,7,0,1,1,0,3,0,4,1,4,7 ],
     @1 of cint[ 3,0,7,2,0,4,7 ],
     @1 of cint[ 5,0,7,1,0,2,3,3,0,4,7 ],
     @1 of cint[ 5,0,0,4,7,-1,0,0,7,4,0 ],
     @1 of cint[ 6,2,0,2,4,4,7,-1,0,2,4,0,7 ],
     @1 of cint[ 4,0,7,4,7,0,0,4,0 ],
     @1 of cint[ 4,4,7,2,7,2,0,4,0 ],
     @1 of cint[ 2,0,7,4,0 ],
     @1 of cint[ 4,0,7,2,7,2,0,0,0 ],
     @1 of cint[ 3,0,5,2,7,4,5 ],
     @1 of cint[ 3,-2,1,0,0,4,0 ],
     @1 of cint[ 2,3,5,2,7 ],
     @1 of cint[ 12,0,3,1,4,3,4,4,3,4,0,-1,0,4,1,3,0,1,0,0,1,1,2,4,2 ],
     @1 of cint[ 11,0,1,1,0,3,0,4,1,4,3,3,4,1,4,0,3,-1,0,0,0,0,7 ],
     @1 of cint[ 8,4,3,3,4,1,4,0,3,0,1,1,0,3,0,4,1 ],
     @1 of cint[ 11,4,3,3,4,1,4,0,3,0,1,1,0,3,0,4,1,-1,0,4,0,4,7 ],
     @1 of cint[ 10,0,2,4,2,4,3,3,4,1,4,0,3,0,1,1,0,3,0,4,1 ],
     @1 of cint[ 7,2,0,2,6,3,7,4,7,-1,0,4,4,1,4 ],
     @1 of cint[ 15,-2,3,0,1,1,0,3,0,4,1,4,7,-1,0,4,6,3,7,1,7,0,6,0,4,1,3,3,3,4,4 ],
     @1 of cint[ 8,0,0,0,7,-1,0,0,3,1,4,3,4,4,3,4,0 ],
     @1 of cint[ 5,2,0,2,4,-1,0,2,5,2,5 ],
     @1 of cint[ 8,-2,2,0,0,1,0,2,1,2,6,-1,0,2,7,2,7 ],
     @1 of cint[ 6,1,0,1,7,-1,0,4,4,1,2,4,0 ],
     @1 of cint[ 4,2,7,2,1,3,0,4,0 ],
     @1 of cint[ 12,0,0,0,4,-1,0,0,3,1,4,2,3,2,0,-1,0,2,3,3,4,4,3,4,0 ],
     @1 of cint[ 8,0,0,0,4,-1,0,0,3,1,4,3,4,4,3,4,0 ],
     @1 of cint[ 10,-2,3,0,4,1,3,3,3,4,4,4,6,3,7,1,7,0,6,0,4 ],
     @1 of cint[ 14,-2,3,0,4,1,3,3,3,4,4,4,6,3,7,1,7,0,6,0,4,0,0,-1,0,0,6,0,7 ],
     @1 of cint[ 12,-2,3,4,0,4,7,-1,0,4,6,3,7,1,7,0,6,0,4,1,3,3,3,4,4 ],
     @1 of cint[ 7,0,0,0,4,-1,0,0,3,1,4,3,4,4,3 ],
     @1 of cint[ 10,0,1,1,0,3,0,4,1,3,2,1,2,0,3,1,4,3,4,4,3 ],
     @1 of cint[ 7,3,1,2,0,1,1,1,6,-1,0,0,4,2,4 ],
     @1 of cint[ 8,0,4,0,1,1,0,3,0,4,1,-1,0,4,0,4,4 ],
     @1 of cint[ 3,0,4,2,0,4,4 ],
     @1 of cint[ 5,0,4,1,0,2,2,3,0,4,4 ],
     @1 of cint[ 5,0,0,4,4,-1,0,0,4,4,0 ],
     @1 of cint[ 7,-2,3,0,0,2,3,4,7,-1,0,0,7,2,3 ],
     @1 of cint[ 4,0,4,4,4,0,0,4,0 ],
     @1 of cint[ 8,-2,1,4,0,3,1,3,3,2,4,3,5,3,7,4,8 ],
     @1 of cint[ 2,2,6,2,0 ],
     @1 of cint[ 8,-2,1,0,0,1,1,1,3,2,4,1,5,1,7,0,8 ],
     @1 of cint[ 4,0,5,1,6,3,5,4,6 ] ]
