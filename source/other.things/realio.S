procedure format( real x ; int ipart,fpart -> string )
begin
     let zero := decode( "0" )
     let z := begin
                   let i := 0 ; let y := rabs( x )
                   while y >= 1.0 do { y := y / 10. ; i := i + 1 }
                   i
              end
     if fpart < 0 or z > ipart then " -> " ++ gformat( x ) ++ " <- "
     else
     begin
          if ipart = 0 do ipart := 1
          let round := 1.0 ; let Scale := 1.0
          for i = 1 to fpart do round := round / 10.
          for i = 1 to z do Scale := Scale / 10.
          let y := ( rabs( x ) + 0.5 * round ) * Scale
          let result := ""
          for i = 1 to z do
          begin
               y := y * 10.
               let k = truncate( y )
               result := result ++ ( if k = 10 then "10" else code( k + zero ) )
               y := y - k
          end
          let k := truncate( y * 10. )
          if k = 10 do { result := result ++ "1" ; y := 0. ; z := 1 }
          result := result ++ "."
          for i = 1 to fpart do
          begin
               y := y * 10.
               let k := truncate( y )
               result := result ++ code( k + zero )
               y := y - k
          end
          if result( 1 | 1 ) = "." do
          begin
               result := "0" ++ result
               ipart := ipart - 1
          end
          result := ( if x < 0.0 then "-" else " " ) ++ result
          for i = 1 to ipart - z do result := " " ++ result
          result
     end
end
procedure eormat( real x ; int ipart,fpart -> string )
if ipart < 0 or fpart < 0 then " -> " ++ gformat( x ) ++ " <- "
else
begin
     let g := 1.0
     for i = 1 to ipart do g := g * 10.
     let h = g / 10.
     let y := rabs( x ) ; let Scale := 0
     if y ~= 0.0 do
     if y >= g then while y >= g do { y := y / 10. ; Scale := Scale + 1 }
     else while y < h do { y := y * 10. ; Scale := Scale - 1 }
     let st1 := fformat( if x > 0 then y else -y,ipart,fpart )
     let st2 := begin
                     if Scale = 0 then "e+00" else
                     begin
                          let Scale. = rabs( Scale )
                          let sign = if Scale < 0 then { Scale := - Scale ; true } else false
                          let result := "" ; let zero = decode( "0" )
                          while Scale > 0 do
                          begin
                               result := code( Scale rem 10 + zero ) ++ result
                               Scale := Scale div 10
                          end
                          "e" ++ ( if sign then "-" else "+" ) ++
                          ( if Scale. < 10 then "0" else "" ) ++ result
                     end
                end
     st1 ++ st2
end
procedure g.format( real r -> string )
if r = 0.0 then "0." else
begin
     let abs.r = rabs( r )
     let fpart = case true of
                 abs.r < 0.1 or abs.r > 1e5 : 0
                 abs.r < 1.                 : 16
                 abs.r < 10.                : 15
                 abs.r < 100.               : 14
                 abs.r < 1000.              : 13
                 abs.r < 10000.             : 12
                 default                    : 11
     let ans := "" ; let e.part := ""
     if fpart = 0 then
     begin
          ans := eformat( r,1,16 )
          let lnth = length( ans )
          e.part := ans( lnth - 3 | 4 )
          ans := ans( 1 | lnth - 4 )
     end
     else ans := fformat( r,17 - fpart, fpart )
     let pos := length( ans )
     while ans( pos | 1 ) = "0" do pos := pos - 1
     ans( 1 | pos  ) ++ e.part
end
procedure writer( file f ; real n ; int width )
begin
     output f,gformat( n ) : if width < 0 then r.w else width
     for i = 1 to s.w do output f," "
end
procedure read.real( file f -> real )
begin
     procedure discard ; { let ch = read( f ) }

	procedure peep ( cfile f -> string )
	if eof ( f ) then code ( 26 ) ! return ctrl Z on eof
	else peek ( f )

     let zero.sy = decode( "0" ) ; let nine.sy = decode( "9" )

     procedure getsign( -> bool )
     case peep( f ) of
     "+"     : { discard ; false }
     "-"     : { discard ; true }
     default : false

     procedure int.str( -> string )
     begin
          let s := "" ; let i := ""
          repeat i := peep( f ) while digit( i ) do s := s ++ read( f )
          s
     end

     procedure int.conv( string s ; bool minus -> int )
     begin
          let n := 0
          for i = 1 to length( s ) do
          begin
               let k = decode( s( i|1 ) ) - zero.sy
               n := n * 10 + ( if minus then -k else k )
          end
          n
     end

     procedure ex( int Scale -> real )
     begin
          let r := float( 1 ) ; let fac := float( 10 )
          while Scale ~= 0 do
          begin
               if Scale rem 2 = 1 do r := r * fac
               Scale := Scale div 2
               if Scale ~= 0 do fac := fac * fac
          end
          r
     end

     procedure real.conv( string s ; int Scale -> real )
     begin
          let n := float( 0 ) ; let no := length( s ) ; let more := true
          while no >= 1 and more do
          if s( no|1 ) = "0" then no := no - 1 else more := false
          for i = 1 to no do n := n * 10 + ( decode( s( i|1 ) ) - zero.sy )
          Scale := Scale + length( s ) - no
          if Scale ~= 0 do n := if Scale < 0 then n / ex( -Scale ) else n * ex( Scale )
          n
     end

	let s := ""
	repeat s := peep( f )
	while s = " " or s = "'n" or s = "'t" do discard
	let n.sign = getsign
	if ~digit( peep( f ) ) do
	begin
		write "Run time error at line ",line.number : 4,"'n",
			 ( if eof ( f ) then "end of file'n" else "Error in readr'n" )
!		trace
		abort
	end
     let esign := true
     while peep( f ) = "0" do discard
     let ipart = int.str
     let dpart = if peep( f ) = "." then { discard ; int.str } else ""
     let epart = if peep( f ) = "e" then
                 begin
                      discard
                      esign := getsign
                      int.str
                 end else ""
     let Scale = int.conv( epart,esign )
     let value := real.conv( ipart,Scale )
     if dpart ~= "" do value := value + real.conv( dpart,Scale - length( dpart ) )
     if n.sign do value := -value
     value
end
                    
