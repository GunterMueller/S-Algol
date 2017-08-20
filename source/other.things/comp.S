%ul
!++
!
!	The following raster ops have been added
!
!	20	copy		148	not
!	21	rand		149	nand
!	22	ror		150	nor
!	23	xor		151	xnor
!
!	35	subimage.op	163	subpixel.op
!	36	form.image	164	form.pixel.op
!	37	limit.2		165	limit.4
!
!--
%nonewline

!********* These procedures are the operating system dependent parts ***********
!************************ of the S-algol compiler ******************************


procedure check.open( cfile F ; cstring s -> file )
begin

     procedure error( cstring s,s1 )
     begin
          write "'nCannot ",s," file ",s1,"'n"
          abort
     end
     if F = nullfile do error( "open",s )
     F
end

procedure check.create( cfile F ; cstring s -> file )
begin

     procedure error( cstring s,s1 )
     begin
          write "'nCannot ",s," file ",s1,"'n"
          abort
     end
     if F = nullfile do error( "create",s )
     F
end

let code.file.name := ""

procedure Create.code( -> file )
     begin

     procedure error( cstring s,s1 )
     begin
          write "'nCannot ",s," file ",s1,"'n"
          abort
     end
     let ss = options
     if upb( ss ) < 3 do error( "create","" )
     let s1 = ss( 3 )
     let i := length( s1 ) ; let found := false
     while i >= 1 and ~found do
     if s1( i|1 ) = "/" then found := true
                        else i := i - 1
     let s2 = if found then if i = length( s1 ) then ""
                                                else s1( i+1|length( s1 ) - i )
                       else s1
     let s = ( if length( s2 ) > 2 and s2( length( s2 ) - 1|2 ) = ".S"
               then s2( 1|length( s2 ) - 2 ) else s2 ) ++ ".out"
     code.file.name := s
     check.create( create( s,493 ),s )
end

procedure Delete.code.file
{ let so = Create.code ; close( so ) }

procedure Create.flsum( -> file )
begin
     let s = "listing.SS"
     check.create( create( s,493 ),s )
end

procedure Open.source( -> file )
begin

procedure error( cstring s,s1 )
begin
     write "'nCannot ",s," file ",s1,"'n"
     abort
end
     let ss = options
     if upb( ss ) < 3 do error( "open","" )
     let s = ss( 3 )
     check.open( open( s,0 ),s )
end

procedure Compile.options( -> *cstring ) ; options

procedure Stack.element.size( -> int ) ; 1

procedure Version.no( -> int ) ; -7

procedure Code.v.size( -> int ) ; 12000
	
procedure getenv( cstring name -> string )
begin
     let process.env = environment
     let value := ""
     let i := lwb( process.env )
     let ub = upb( process.env )
     let found := false

     while i <= ub and ~found do
     begin
          let entry = process.env( i )
          let eq.pos = find.substr( entry,"=" )
          if eq.pos = -1 or name ~= entry( 1|eq.pos - 1 ) then i := i + 1 else
          begin
               found := true
               if eq.pos < length( entry ) do value := entry( eq.pos + 1|length( entry ) - eq.pos )
          end
     end

     value
end
      
procedure Open.prelude( -> file )
begin
     let s := getenv( "SPRELUDE" )
     if s = "" do s := "/usr/lib/S/S.prelude"
     check.open( open( s,0 ),s )
end

procedure Open.std.decls(-> file)
begin
     let s := getenv( "SSTAND" )
     if s = "" do s := "/usr/lib/S/S.stand"
     check.open( open( s,0 ),s )
end

!*************************** Options *******************************************

let flsum := false ; let ext := false ; let proc.name := "Main"

let listing := false ; let newlins := true

procedure set.options
begin
     let s = Compile.options
     for i = lwb( s ) to upb( s ) do
     begin
          case s( i ) of
          "ext"       : ext := true
          "flsum"     : flsum := true
          "list"      : listing := true
          "nonewline" : newlins := false
          default     : {}
     end
end

set.options

let fl.sum = if flsum then Create.flsum else nullfile

let si := Open.std.decls

!************************** Compiler Symbols ***********************************

let minus.sy = "-" ; let arrow.sy = "->" ; let bar.sy = "|" ; let eq.sy = "="
let colon.sy = ":" ; let dcolon.sy = "::" ; let assign.sy = ":=" ; let lt.sy = "<"
let le.sy = "<=" ; let gt.sy = ">" ; let ge.sy = ">=" ; let neq.sy =  "~="
let comma.sy = "," ; let lp.sy = "(" ; let rp.sy = ")" ; let star.sy = "*"
let semi.sy = ";" ; let lsb.sy = "[" ; let rsb.sy = "]" ; let lcb.sy = "{"
let rcb.sy = "}" ; let plus.sy = "+" ; let dplus.sy = "++" ; let not.sy = "~"
let squote.sy = "''" ; let dquote.sy = "'"" ; let divide.sy = "/"
let tab.sy = "'t"
let hash.sy = "#" ; let hat.sy = "^" ; let amper.sy = "&"
 
!*************************** Reserved Words *********************************

let if.sy = "if" ; let do.sy = "do" ; let is.sy = "is" ; let or.sy = "or"
let at.sy = "at"
let to.sy = "to" ; let of.sy = "of" ; let by.sy = "by"
let in.sy = "in"
let on.sy = "on"
let let.sy = "let" ; let end.sy = "end" ; let int.sy = "int" ; let and.sy = "and"
let for.sy = "for" ; let div.sy = "div" ; let rem.sy = "rem" ; let upb.sy = "upb"
let lwb.sy = "lwb" ; let eof.sy = "eof" ; let nil.sy = "nil"
let pic.sy = "pic"
let ror.sy = "ror" ; let xor.sy = "xor" ; let nor.sy = "nor" ; let rnot.sy = "not"
let off.sy = "off"
let then.sy = "then" ; let else.sy = "else" ; let case.sy = "case" ; let isnt.sy = "isnt"
let cint.sy = "cint" ; let pntr.sy = "pntr" ; let bool.sy = "bool" ; let real.sy = "real"
let file.sy = "file" ; let peek.sy = "peek" ; let read.sy = "read"
let cpic.sy = "cpic"
let from.sy = "from" ; let text.sy = "text"
let xnor.sy = "xnor" ; let onto.sy = "onto" ; let rand.sy = "rand"
let nand.sy = "nand" ; let copy.sy = "copy"
let begin.sy = "begin" ; let while.sy = "while" ; let write.sy = "write"
let creal.sy = "creal" ; let cpntr.sy = "cpntr" ; let cfile.sy = "cfile"
let cbool.sy = "cbool" ; let readi.sy = "readi" ; let readr.sy = "readr"
let readb.sy = "readb" ; let reads.sy = "reads" ; let abort.sy = "abort"
let float.sy = "float"
let shift.sy = "shift" ; let scale.sy = "scale"
let pixel.sy = "pixel" ; let limit.sy = "limit"
let image.sy = "image"
let vector.sy = "vector" ; let string.sy = "string" ; let repeat.sy = "repeat"
let output.sy = "output" ; let out.16.sy = "out.16" ; let out.32.sy = "out.32"
let colour.sy = "colour" ; let rotate.sy = "rotate"
let cpixel.sy = "cpixel"
let forward.sy = "forward" ; let default.sy = "default" ; let cstring.sy = "cstring"
let eformat.sy = "eformat" ; let fformat.sy = "fformat" ; let gformat.sy = "gformat"
let read.16.sy = "read.16" ; let read.32.sy = "read.32"
let nullfile.sy = "nullfile" ; let out.byte.sy = "out.byte" ; let external.sy = "external"
let pic.pntr.sy = "pic.pntr"
let structure.sy = "structure" ; let procedure.sy = "procedure"
let read.name.sy = "read.name" ; let read.byte.sy = "read.byte"
let read.a.line.sy = "read.a.line"

let literal.sy = "literal" ; let identifier.sy = "identifier"

structure scalar( cstring t.name )
structure VECTOR( pntr elms )
structure proc( pntr args,result ; bool forwd,stand )
structure var( pntr contents )
structure STRUCTURE( pntr fields )
structure field( pntr fieldt ; int f.offset )
structure cons( pntr hd,tl )
structure struct.table( int s.tdm,s.ptrs ; pntr str.next )
structure const( pntr content )
structure str( int str.int ; pntr str.pntr )
structure p.decl( cpntr p.pntr ; cint p.int )
structure s.decl( cpntr s.pntr ; cint s.int )
structure EXTERNAL( cstring e.name ; cpntr e.type ; int e.addr ; pntr e.next )
structure Image( pntr image.elements )
let INT = scalar( int.sy ) ; let CINT = const( INT )
let REAL = scalar( real.sy ) ; let CREAL = const( REAL )
let BOOL = scalar( bool.sy ) ; let CBOOL = const( BOOL )
let PNTR = scalar( pntr.sy ) ; let CPNTR = const( PNTR )
let STRING = scalar( string.sy ) ; let CSTRING = const( STRING )
let FILE = scalar( file.sy ) ; let CFILE = const( FILE )
let PIXEL = scalar( pixel.sy ) ; let CPIXEL = const( PIXEL ) ;
let IMAGE = Image( PIXEL ) ; let CIMAGE = Image( CPIXEL )
let PIC = scalar( pic.sy ) ; let CPIC = const( PIC )
let VOID = scalar( "void" ) ; let ANY = scalar( "undefined" )
let vector.type = VECTOR( ANY )
let image.type = Image( ANY )
let proc.decl.list := nil ; let str.decl.list := nil
let ex.decl.list := nil ; let external.list := nil
let procedure.name := "" ; let procedure.type := nil

procedure eq1( cpntr a,b -> bool )
a = b or a = ANY or b = ANY or
a is VECTOR and b is VECTOR and eq1( a( elms ),b( elms ) ) or
a is Image and b is Image and eq1( a( image.elements ),b( image.elements ) ) or
a is const and b is const and eq1( a( content ),b( content ) )

procedure eq( cpntr a,b -> bool )
a = b or a = ANY or b = ANY or
a is const and eq( a( content ),b ) or
b is const and eq( a,b( content ) ) or
a is VECTOR and b is VECTOR and eq1( a( elms ),b( elms ) ) or
a is Image and b is Image and eq1( a( image.elements ),b( image.elements ) ) or
a is proc and b is proc and eq( a( args ),b( args ) ) and eq( a( result ),b( result ) ) or
a is cons and b is cons and eq( a( hd ),b( hd ) ) and eq( a( tl ),b( tl ) )

procedure eq2( cpntr a,b -> bool )
a ~= ANY and b ~= ANY and eq( a,b )

let codeword.size = Code.v.size
let codeword = vector 0 :: codeword.size of 0
let cp := 4 ; let code.base := 0 ; let code.size := 0 ; let coding := false

let small.a = decode( "a" ) ; let small.x := decode( "x" )

procedure init1( -> *c*cstring )
@small.a of c*cstring[
@1 of cstring[ at.sy,and.sy,abort.sy,"" ],
@1 of cstring[ begin.sy,bool.sy,by.sy,"" ],
@1 of cstring[ case.sy,cstring.sy,cint.sy,creal.sy,cfile.sy,cpntr.sy,cbool.sy,cpic.sy,colour.sy,cpixel.sy,copy.sy,"" ],
@1 of cstring[ do.sy,default.sy,div.sy,"" ],
@1 of cstring[ end.sy,else.sy,eformat.sy,eof.sy,external.sy,"" ],
@1 of cstring[ file.sy,for.sy,float.sy,fformat.sy,forward.sy,from.sy,"" ],
@1 of cstring[ gformat.sy,"" ],
@1 of cstring[ "" ],
@1 of cstring[ if.sy,int.sy,is.sy,isnt.sy,in.sy,image.sy,"" ],
@1 of cstring[ "" ],
@1 of cstring[ "" ],
@1 of cstring[ let.sy,lwb.sy,limit.sy,"" ],
@1 of cstring[ "" ],
@1 of cstring[ nand.sy,nor.sy,rnot.sy,"" ],
@1 of cstring[ of.sy,or.sy,output.sy,out.byte.sy,out.16.sy,out.32.sy,onto.sy,"" ],
@1 of cstring[ procedure.sy,pntr.sy,peek.sy,pic.sy,pic.pntr.sy,pixel.sy,"" ],
@1 of cstring[ "" ],
@1 of cstring[ real.sy,repeat.sy,rem.sy,read.sy,readi.sy,read.a.line.sy,
               reads.sy,readr.sy,readb.sy,read.name.sy,read.byte.sy,read.16.sy,read.32.sy,rotate.sy,ror.sy,rand.sy,"" ],
@1 of cstring[ string.sy,structure.sy,shift.sy,scale.sy,"" ],
@1 of cstring[ to.sy,then.sy,text.sy,"" ],
@1 of cstring[ upb.sy,"" ],
@1 of cstring[ vector.sy,"" ],
@1 of cstring[ while.sy,write.sy,"" ],
@1 of cstring[ xor.sy,xnor.sy,"" ] ]

let r.words = init1

let error := false ; let errors.on.line := 0 ; let no.of.errors := 0
let err.pos := 10 ; let output.length = 120 ; let space = " " ; let nl = "'n"
let error.line = vector 0 :: output.length of space
let errors = vector 1 :: 5 of error.line

procedure err.mess( *string s )
begin
     error := true ; coding := false ; cp := 0
     errors.on.line := errors.on.line + 1
     if errors.on.line <= 5 do errors( errors.on.line ) := s
     error.line( err.pos - 1 ) := "^" ++ error.line( err.pos - 1 )
     no.of.errors := no.of.errors + 1
end

procedure err.mess1( cstring s )
err.mess( @1 of string[ s ] )

!*************************** Code Generation ***********************************

let externals := 0

procedure externs( cint n )
externals := b.or( externals,n )

procedure check( cint n,m )
if n > m do
begin
     err.mess1( "instruction operand too big'n" ++ iformat( n ) ++ space ++ iformat( m ) )
     trace
end     

let twoto16 = shift.l( 1,15 ) ; let twoto17 = shift.l( twoto16,1 )
let last.label := 0

procedure setlab( int link )
begin
     last.label := cp
     if coding do
     while link ~= 0 do
     begin
          let a = code.base + link
          let n := shift.l( codeword( a ),8 ) + codeword( a + 1 )
          let spot = link
          link := n
          n := cp - spot - 2
          check( n,twoto17 )
          codeword( a ) := shift.r( n,8 )
          codeword( a + 1 ) := b.and( n,255 )
     end
end
let SI = Open.source ; let so = Create.code

procedure bout( cint n )
out.byte so,n,0

procedure boutl( cint n )
begin
     bout( shift.r( n,24 ) ) ; bout( shift.r( n,16 ) )
     bout( shift.r( n,8 ) ) ; bout( n )
end

let max.seg.size = twoto17
procedure out.list( pntr t -> int )
if coding then
begin
     let size := 0
     while t ~= nil do
     begin
          bout( shift.r( t( s.tdm ),8 ) ) ; bout( t( s.tdm ) )
          bout( shift.r( t( s.ptrs ),8 ) ) ; bout( t( s.ptrs ) )
          t := t( str.next )
          size := size + 4
     end
     size
end else 0

procedure add.byte( cint n )
if coding do
begin
     codeword( cp + code.base ) := n
     cp := cp + 1
     if cp > max.seg.size do err.mess1( "** Segment too big**'n" )
     if cp + code.base >= codeword.size do err.mess1( "** Code vector overflow**'n" )
end

procedure add16( cint n )
begin
     add.byte( shift.r( n,8 ) )
     add.byte( b.and( n,255 ) )
end
procedure gen8( cint n ) ; { check( n,255 ) ; add.byte( n ) }

procedure gen16( cint n ) ; { check( n,twoto16 -1 ) ; add16( n ) }

procedure try.single( cint op,m,n )
if m > 255 or n > 255 then { gen8( op + 128 ) ; gen16( m ) ; gen16( n ) }
                      else { gen8( op ) ; gen8( m ) ; gen8( n ) }
 
procedure try.short( cint op,n )
if n < 256 then { gen8( op ) ; gen8( n ) }
           else { gen8( op + 128 ) ; gen16( n ) }

procedure try.jump( cint op,n )
if n < 254 then { gen8( op ) ; gen8( n + 2 ) }
           else { gen8( op + 128 ) ; gen16( n + 3 ) }

procedure gen.type0( cpntr t -> int )
case t of
CINT,INT,CBOOL,BOOL,CPIXEL,PIXEL : 0
CREAL,REAL          : 128
CSTRING,STRING      : 1
VOID                : 130
default  : 129

procedure gen.type1( cpntr t ; cint base )
gen8( base + gen.type0( t ) )

procedure pointer( cpntr t -> bool )
case t of
CPIC,PIC,
IMAGE,CIMAGE,
CSTRING,STRING,CPNTR,PNTR,CFILE,FILE : true
default : eq( vector.type,t ) or eq( image.type,t )

let st.size = Stack.element.size ; let st.size2 = 2 * st.size
let st.size3 = 3 * st.size ; let st.size4 = 4 * st.size
let mscw.size = 6 * st.size ; let lex.level := 0 ; let newlab = 0 
let ssp := mscw.size ; let psp := 0
let max.ms := ssp ; let max.ps := psp

procedure type.size( cpntr t -> int )
if t is proc or eq2( REAL,t ) then st.size2 else st.size

procedure push.ps
begin
     psp := psp + st.size
     if psp > max.ps do max.ps := psp
end

procedure pop.ps ; psp := psp - st.size

procedure pop.ms ; ssp := ssp - st.size

procedure inc.ms( cint n )
begin
     ssp := ssp + n
     if ssp > max.ms do max.ms := ssp
end

procedure inc.ps( cint n )
begin
     psp := psp + n
     if psp > max.ps do max.ps := psp
end

procedure push.ms ; inc.ms( st.size )

procedure dec.ms( cint n ) ; ssp := ssp - n

procedure dec.ps( cint n ) ; psp := psp - n

procedure dec.stack( cpntr t )
if t ~= VOID do if pointer( t ) then dec.ps( type.size( t ) )
                                else dec.ms( type.size( t ) )

procedure inc.stack( cpntr t )
if t ~= VOID do if pointer( t ) then inc.ps( type.size( t ) )
                                else inc.ms( type.size( t ) )

procedure comp( cstring s ; cpntr t )
begin
     gen.type1( t,case s of
                  eq.sy   : 96
                  neq.sy  : 99
                  lt.sy   : 102
                  le.sy   : 104
                  gt.sy   : 106
                  default : 108 )
     dec.stack( t ) ; dec.stack( t ) ; push.ms
end

procedure arith.op( cpntr t ; cstring s )
begin
     dec.ms( type.size( t ) )
     gen8( ( case s of
             plus.sy   : 110
             star.sy   : 238
             minus.sy  : 111
             divide.sy : 239
             div.sy    : 239
             default   : 112 ) + ( if eq( REAL,t ) then 3 else 0 ) )
end

procedure sub.op( cpntr t1,t2 )
begin
     let arai = eq( vector.type,t2 )
     let ptr = pointer( t1 )
     if ~ptr do pop.ps
     dec.ms( if ptr then if arai then st.size else st.size2
                    else if arai then 0 else st.size )
     if eq2( REAL,t1 ) do push.ms
     gen.type1( t1,if arai then 78 else 66 )
end

procedure sub.ass( cpntr t1,t2 )
begin
     let arai = eq( vector.type,t2 )
     dec.stack( t1 )
     pop.ps
     dec.ms( if arai then st.size else st.size2 )
     gen.type1( t1,if arai then 81 else 69 )
end

procedure is.op( cbool s )
begin
     pop.ps
     gen8( if s then 65 else 193 )
end

procedure neg.op( cpntr t ) ; gen8( 240 + ( if eq( REAL,t ) then 3 else 0 ) )

procedure not.op ; gen8( 115 )

procedure bjump( cint n ) ; try.jump( 1,cp - n )

procedure bjumpt( cint n )
{ pop.ms ; try.jump( 10,cp - n ) }

procedure cjump( cpntr t ; cint n -> int )
begin
     dec.stack( t )
     gen.type1( t,7 )
     gen16( n )
     cp - 2
end
          
procedure fjump( cint n -> int )
{ gen8( 128 ) ; gen16( n ) ; cp - 2 }
          
procedure jumpf( cint n -> int )
{ pop.ms ; gen8( 130 ) ; gen16( n ) ; cp - 2 }
          
procedure jumpff( cint n -> int )
{ pop.ms ; gen8( 131 ) ; gen16( n ) ; cp - 2 }

procedure jumptt( cint n -> int )
{ pop.ms ; gen8( 132 ) ; gen16( n ) ; cp - 2 }

procedure finish.op ; gen8( 125 )

procedure bound( cint n ; cpntr t )
{ push.ms ; pop.ps ; gen8( 84 + 128 * n ) }

procedure load.st.func( cint n )
{ inc.ms( st.size2 ) ; gen8( 59 ) ; gen8( n ) }

procedure load.32( cint n )
begin
     add.byte( b.and( shift.r( n,24 ),255 ) )
     add.byte( b.and(shift.r( n,16 ),255 ) )
     add.byte( b.and(shift.r( n,8 ),255 ) )
     add.byte( b.and( n,255 ) )
end
procedure forward.op
{ inc.ms( st.size2 ) ; gen8( 62 ) }

procedure external.op( cstring n ; cpntr t )
begin
     let code = cp
     gen8( 45 )
     gen16( ssp ) ; gen16( -1 ) ; gen16( -1 )
     inc.ms( st.size2 )
     ex.decl.list := EXTERNAL( n,t,code,ex.decl.list )
end

let last.retract := -1


procedure return( cpntr t )
if coding do
begin
     if last.retract = cp - 3 then
     begin
          cp := cp - 3
          if last.label = cp + 3 do
          for i = 1 to 3 do add.byte( 0 )
     end else
     if last.retract = cp - 5 do
     begin
          case codeword( cp - 5 ) of
          181,182,183,184,186 :
          begin
               cp := cp - 5
               if last.label = cp + 5 do
               for i = 1 to 5 do add.byte( 0 )
          end
          default : {}
     end
     gen.type1( t,47 )
end

procedure retract( cint ms,ps ; pntr t )
begin
     let ms1 := ms ; let ps1 := ps
     if pointer( t ) then ps1 := ps1 + st.size
                     else if t ~= VOID do ms1 := ms1 + type.size( t )
     if ~( ms1 = ssp and ps1 = psp ) do
     begin
          ssp := ms1 ; psp := ps1
          last.retract := cp
          try.single( 53 + ( case t of
                        INT,BOOL,CINT,CBOOL,PIXEL,CPIXEL : 0
                        REAL,CREAL          : 1
                        STRING,CSTRING      : 2
                        PNTR,CPNTR,IMAGE,CIMAGE          : 3
                        default             : 5 ),ms,ps )
     end
end

let st.id.size = 4 * st.size + 3 * st.size2

procedure mst.external( cint n )
begin
     let disp = mscw.size + ( n + 1 ) * st.size2 + st.id.size
     if n = 3 then { gen8( 61 ) ; gen8( disp ) }     ! MST_EXT
              else try.short( 19,disp )          ! MST_GLOBAL
     ssp := ssp + mscw.size
     externs( case n of
              0       : 4
              1       : 4
              2       : 4
              3       : 8
              default : 16 )
end

procedure apply.op( cint ms,ps )
{ try.short( 44,ms ) ; dec.ms( ms + mscw.size ) ; psp := psp - ps }

procedure load.struct.base
{  bout( shift.r( twoto16,8 ) ) ; bout( twoto16 ) ; code.size := 2 }

procedure load.trademark( cint n )
begin
     try.short( 86,n )
     add16( 0 ) ; add16( 0 )
     str.decl.list := s.decl( str.decl.list,cp )
     inc.ms( st.size )
end

procedure load.proc.name( cstring s )
begin
     gen8( 92 )
     let k = length( s )
     gen16( 0 )
     add.byte( b.and( shift.r( k,8 ),255 ) )
     add.byte( b.and( k,255 ) )
     for i = 1 to k-1 by 2 do
        gen16( decode( s( i|1 ) ) * 256 + decode( s( i+1|1 ) ) )
     if k rem 2 = 1 do gen8( decode( s( k|1 ) ) )
end

let last.segment := 0

procedure make.vector( cpntr t ; cint level,plevel )
begin
     ssp := level
     psp := plevel + st.size
     gen.type1( t,72 )
     gen16( if pointer( t ) then plevel else level )
end

procedure endsegment( cint n,add )
if coding do
begin
     let base = code.base + cp
     let start = ex.decl.list
     while ex.decl.list ~= nil do
     begin
          ex.decl.list( e.addr ) := ex.decl.list( e.addr ) + code.size
          ex.decl.list := ex.decl.list( e.next )
     end
     if start ~= nil do external.list := cons( start,external.list )
     while str.decl.list ~= nil do
     begin
          let i = str.decl.list( s.int )
          let j = base + i - 4 ; let k = -( code.size + i )
          codeword( j ) := shift.r( k,24 )
          codeword( j + 1 ) := shift.r( k,16 )
          codeword( j + 2 ) := shift.r( k,8 )
          codeword( j + 3 ) := k
          str.decl.list := str.decl.list( s.pntr )
     end
     while proc.decl.list ~= nil do
     begin
          let i = proc.decl.list( p.int )
          let j = base + i
          let offset := codeword( j + 3 )
          offset := b.or( offset,shift.l( codeword( j + 2 ),8 ) )
          offset := b.or( offset,shift.l( codeword( j + 1 ),16 ) )
          offset :=  b.or( offset,shift.l( codeword( j ),24 ) )
          let new.offset = offset - ( code.size + i ) - 4
          codeword( j ) := shift.r( new.offset,24 )
          codeword( j + 1 ) := shift.r( new.offset,16 )
          codeword( j + 2 ) := shift.r( new.offset,8 )
          codeword( j + 3 ) :=  new.offset
          proc.decl.list := proc.decl.list( p.pntr )
     end
     gen16( max.ms ) ; gen16( max.ps ) ; cp := cp - 4
     for i = base to base + n - 1 do bout( codeword( i ) )
     add.byte( 45 )
     gen16( add - st.size2 )
     load.32( code.size )
     last.segment := code.size
     code.size := code.size + n
end

procedure iliffe.op( cpntr t ; cint bounds )
begin
     dec.stack( t )
     dec.ms( st.size * bounds * 2 )
     gen.type1( t,75 )
     gen16( bounds )
     push.ps
end

procedure form.structure( cint ms,ps )
begin
     gen8( 64 ) ; gen16( ms + st.size )
     ssp := ms ; psp := ps + st.size
end

procedure for.test( cint n -> int ) ; { gen8( 133 ) ; gen16( n ) ; cp - 2 }

procedure for.step( cint n ) ; try.jump( 6,cp - n )

procedure float.op( cint n ) ; { push.ms ; gen8( 116 + ( n - 1 ) * 128 ) }

procedure reverse.stack( cpntr t ) ; gen8( 123 + ( if pointer( t ) then 128 else 0 ) )

procedure concat.op ; { pop.ps ; gen8( 85 ) }

procedure substr.op ; { dec.ms( st.size2 ) ; gen8( 213 ) }

procedure write.op( cint n )
begin
     pop.ms
     case n of
     0,2     : pop.ms
     3,4     : { pop.ps ; pop.ms }
     default : pop.ps
     gen8( 255 ) ; gen8( n )
end

procedure read.op( cint n )
begin
     case n of
     0,2,3,6,9,10 : { push.ms ; pop.ps }
     7            : pop.ps
     default      : {}
     gen8( 127 )  ; gen8( n )
end

procedure erase.op( cpntr t ) ; { dec.stack( t ) ; gen.type1( t,120 ) }

procedure load.int( cint n )
if n < -128 or n >= 128 then 
begin
     gen8( 218 ) ; load.32( n )
end else { gen8( 90 ) ; gen8( if n < 0 then 256 + n else n ) }

procedure load.lit( cpntr t ; cint n )
if eq2( PNTR,t ) then { push.ps ; gen8( 222 ) } else
if eq2( FILE,t ) then { push.ps ; gen8( 221 ) } else
begin
     push.ms
     if eq2( BOOL,t ) then {  gen8( 91 ) ; gen8( if n < 0 then 256 + n else n ) }
                      else load.int( n )
end

procedure load.string( pntr t )
begin
     let k = t( str.int )
     if k = 0 then gen8( 94 )
              else if k = 1 then { gen8( 93 ) ; gen8( t( str.pntr,str.int ) ) }
     else
     begin
          gen8( 92 )
          gen16( b.or( shift.l( 1,3 ),shift.l( 1,12 ) ) )
          gen16( k )
          for i = 1 to k do
          begin
               t := t( str.pntr )
               gen8( t( str.int ) )
          end
     end
     push.ps
end

procedure load.real( creal n )
begin
     gen8( 219 )
     let ans = fiddle.r( n )
     load.32( ans( 1 ) ) ; load.32( ans( 2 ) )
     inc.ms( st.size2 )
end

procedure gen.load( cint op,add1,add2 )
if add2 > 255 then { gen8( op + 128 ) ; gen8( add1 ) ; gen16( add2 ) }
              else { gen8( op ) ; gen8( add1 ) ; gen8( add2 ) }

structure link( string name ; int addr1,addr2 ; pntr type1,left,right )

procedure mark.stack( cpntr p -> bool )
if coding then
begin
     let ll = p( addr1 ) ; let dd = p( addr2 )
     let t = p( type1,args )
     let params = t ~= nil
     case true of
     p( type1,stand ) : { gen8( if params then 187 else 60 ) ; gen8( dd ) }
                        ! MST_SFN or APPLY_SFN
     ll = lex.level   : try.short( if params then 15 else 52,dd )
                        ! MST_LOCAL or LOCAL_APPLY
     ll = 0           : try.short( if params then 19 else 51,dd )
                        ! MST_GLOBAL or GLOBAL_APPLY
	     default          : gen.load( if params then 27 else 63,lex.level - ll,dd )
				! MST_LOAD or LOAD_APPLY
	     if params do inc.ms( mscw.size )
	     params
	end else p( type1,args ) ~= nil
	     
	procedure load( cint add1,add2 ; cbool r.value ; cpntr type )
	begin
	     if r.value then inc.stack( type ) else dec.stack( type )
	     let ty :=
		  case true of
		  pointer( type )           : 1
		  type is proc              : 2
		  eq( REAL,type )           : 2
		  default                   : 0
	     if ~r.value do ty := ty + 16
	     case true of
	     add1 = lex.level : try.short( 12 + ty,add2 )
	     add1 = 0         : try.short( 16 + ty,add2 )
	     default          : gen.load( 24 + ty,lex.level - add1,add2 )
end

procedure image.op
begin
     dec.ms( st.size3 ) ; push.ps
     gen8( 36 )
end
procedure form.pixel.op( cint n )
begin
     dec.ms( ( n - 1 ) * st.size )
     gen8( 164 ) ; gen8( n )
end

procedure subimage.op
begin
     dec.ms( st.size2 )
     gen8( 35 )
end

procedure subpixel.op
begin
     pop.ms
     gen8( 163 )
end

procedure limit.op( cbool to.symb )
begin
     dec.ms( if to.symb then st.size4 else st.size2 )
     if to.symb then gen8( 165 ) else gen8( 37 )
end

procedure Raster.op( cstring s )
begin
     dec.ps( st.size2 )
     gen8( 20 +
            ( case s of
              copy.sy : 0
              rnot.sy : 128
              rand.sy : 1
              nand.sy : 129
              ror.sy  : 2
              nor.sy  : 130
              xor.sy  : 3
              default : 131 ) )   ! xnor.sy
end
let newlin := true ; let line.count := 0

procedure newline
if newlin and coding and newlins do
begin
     try.short( 124,line.count + 1 )
     newlin := false
end

!***************************** Lexical Analysis ********************************

let heading := "Outline Graphics System" ; let eot = "?"
let lines.per.page := 44 ; let lines.on.page := lines.per.page + 1
let block.count := 0 ; let last.block.count := 0
let option := true
let line.pos := 0 ; let page.count := 0
let line.image = vector 0 :: output.length of space

procedure shutdown
if error then write "'n Compilation Fails'nNumber of Error Messages = ",no.of.errors : 3,nl
         else if listing do write "'n**** Program Compiles ****'n"

procedure title
begin
     page.count := page.count + 1
     if listing do
     begin
          if page.count ~= 1 do for i = lines.on.page to lines.per.page + 3 do write nl
          write nl,heading : 30,"page " : 9,page.count : 4,"'n'n"
     end
     lines.on.page := 3
end

let zero.sy = decode( "0" ) ; let nine.sy = decode( "9" ) ; let one.sy = decode( "1" )
let tabs := false

procedure out
begin
     if lines.on.page > lines.per.page do title
     lines.on.page := lines.on.page + 1
end

procedure write.line( cfile F )
begin
     output F,line.count : 5,space,
     if block.count > last.block.count then code( block.count + zero.sy ) else minus.sy : 1,
     if block.count < last.block.count then code( block.count + one.sy ) else minus.sy : 1,"  "
     for i = 0 to line.pos - 1 do output F,line.image( i )
end

procedure output.line
begin
     line.count := line.count + 1
     if flsum do
     begin
          if line.count = 4093 do err.mess1( "Too many lines for flow summary'n" )
          write.line( fl.sum )
     end
     if ~option do
     begin
          newlin := true
          if listing or errors.on.line ~= 0 do
          begin
               out
               write.line( s.o )
               last.block.count := block.count
          end
          if errors.on.line ~= 0 then
          begin
               out
               for i = 0 to err.pos - 1 do write error.line( i )
               write nl
               for i = 0 to err.pos - 1 do error.line( i ) := space
               if errors.on.line > 3 do errors.on.line := 3
               if no.of.errors < 30 do
               for i = 1 to errors.on.line do
               begin
                    out
                    let k = errors( i )
                    for j = lwb( k ) to upb( k ) do write k( j )
               end
               errors.on.line := 0
          end else
          if tabs do
          begin
               for i = 0 to err.pos - 1 do error.line( i ) := space
               tabs := false
          end
     end
     line.pos := 0 ; err.pos := 10
end

let peep := read( si )

procedure inc.pos( cint n )
begin
     err.pos := err.pos + n
     if err.pos > output.length + 1 do err.pos := output.length + 1
     line.pos := line.pos + 1
     if line.pos >= output.length do
     begin
          line.image( output.length ) := nl
          line.pos := line.pos + 1
          output.line
     end
end

procedure next.ch( -> string )
begin
     let ch = peep
     line.image( line.pos ) := ch
     if ch = tab.sy do
     begin
          error.line( err.pos ) := tab.sy
          tabs := true
     end
     inc.pos( 1 )
     if ch = nl do output.line
     peep := if eof( si ) then eot else read( si )
     ch
end

procedure read.char( -> string )
begin
     let ch = next.ch
     if ch = squote.sy then
     case peep of
     "n"     : { let dis = next.ch ; nl }
     "p"     : { let dis = next.ch ; "'p" }
     "o"     : { let dis = next.ch ; "'o" }
     "b"     : { let dis = next.ch ; "'b" }
     "t"     : { let dis = next.ch ; tab.sy }
   dquote.sy : { let dis = next.ch ; dquote.sy }
   squote.sy : { let dis = next.ch ; squote.sy }
     default : { ch }
     else ch
end

let symb := "" ; let the.string := nil
procedure try( cstring s )
if s( 2|1 ) = peep do { let dis = next.ch ; symb := s }

procedure read.string
begin
     let t := str( 0,nil ) ; let start = t
     let s := "" ; let i := 0
     repeat s := peep while s ~= dquote.sy and ~eof( si ) do
     { t( str.pntr ) := str( decode( read.char ),nil ) ; t := t( str.pntr ) ; i := i + 1 }
     let dis = next.ch
     if dis = eot and s ~= dquote.sy do err.mess1( "End of File'n" )
     start( str.int ) := i
     the.string := start
end

let lit.type := nil ; let the.lit := 0 ; let the.name := ""

procedure reserved.word( cstring s -> bool )
begin
     let index = decode( s )
     if index >= small.a and index <= small.x then
     begin
          let s1 = r.words( index )
          let ub = upb( s1 ) ; let i := 1
          while i < ub and s1( i ) ~= s do i := i + 1
          if i = ub then false else { symb := s1( i ) ; true }
     end else false
end

let underline := false
let under = @2 of cstring[
            "__'b'b","___'b'b'b","____'b'b'b'b","_____'b'b'b'b'b","______'b'b'b'b'b'b",
            "_______'b'b'b'b'b'b'b","________'b'b'b'b'b'b'b'b","_________'b'b'b'b'b'b'b'b'b",
            "__________'b'b'b'b'b'b'b'b'b'b","___________'b'b'b'b'b'b'b'b'b'b'b" ]

let at.new.line := false

procedure ul( cstring s )
if underline do
begin
     line.image( line.pos ) := line.image( line.pos - 1 )
     line.image( line.pos - 1 ) := under( length( s ) )
     inc.pos( 0 )
end
let off.value = shift.l( 1,24 ) ; let on.value = off.value + 1

procedure try.name
begin
     let s := read.name( si,peep )
     line.image( line.pos ) := s
     inc.pos( length( s ) )
     peep := if eof( si ) then eot else read( si )
     symb := if reserved.word( s ) then { ul( s ) ; the.name := symb ; symb } else
             case s of
     "true","false"  : begin
                            ul( s )
                            lit.type := BOOL
                            the.lit := if s( 1|1 ) = "t" then 0 else -1
                            literal.sy
                       end
     nil.sy          : { ul( s ) ; lit.type := PNTR ; literal.sy }
     on.sy,off.sy    : begin
                            ul( s )
                            lit.type := PIXEL
                            the.lit := if s = on.sy then on.value else off.value
                            literal.sy
                       end
     nullfile.sy     : { ul( s ) ; lit.type := FILE ; literal.sy }
     default         : begin
                            the.name := s
                            identifier.sy
                       end
end

let maxintstr = iformat( maxint )
let minintstr = { let x = iformat( -maxint - 1 ) ; x( 2|length( x ) - 1 ) }
let real.value := float( 0 ) ; let unary.minus := false

procedure int.conv( cstring s ; cbool minus -> int )
begin
     let n := 0
     for i = 1 to length( s ) do
     begin
          let k = decode( s( i|1 ) ) - zero.sy
          n := n * 10 + ( if minus then -k else k )
     end
     n
end

procedure int.str( -> string )
begin
     let s := ""
     while digit( peep ) do { s := s ++ peep ; let dis = next.ch }
     s
end

procedure le( cstring s,s1 -> bool )
length( s ) < length( s1 ) or length( s ) = length( s1 ) and s <= s1

procedure ex( int the.scale -> real )
begin
     let r := float( 1 ) ; let fac := float( 10 )
     while the.scale ~= 0 do
     begin
          if the.scale rem 2 = 1 do r := r * fac
          fac := fac * fac
          the.scale := the.scale div 2
     end
     r
end

procedure real.conv( cstring s ; int the.scale -> real )
begin
     let n := float( 0 ) ; let no := length( s ) ; let more := true
     while no >= 1 and more do if s( no|1 ) = "0" then no := no - 1 else more := false
     for i = 1 to no do n := n * 10 + ( decode( s( i|1 ) ) - zero.sy )
     the.scale := the.scale + length( s ) - no
     if the.scale ~= 0 do n := if the.scale < 0 then n / ex( -the.scale ) else n * ex( the.scale )
     n
end

procedure number
begin
     symb := literal.sy ; lit.type := INT ; let sign := true
     while peep = "0" do { let dis = next.ch }
     let ipart = int.str
     let dpart = if peep = "." then { let dis = next.ch ; lit.type := REAL ; int.str } else ""
     let epart = if peep = "e" then
                 begin
                      let dis = next.ch
                      lit.type := REAL
                      sign := if peep = minus.sy then { let dis = next.ch ; true }
                                                 else { if peep = plus.sy do { let dis = next.ch } ; false }
                      int.str
                 end else ""
     if lit.type = REAL then
     begin
          let the.scale = int.conv( epart,sign )
          real.value := real.conv( ipart,the.scale )
          if dpart ~= "" do real.value := real.value + real.conv( dpart,the.scale - length( dpart ) )
          if unary.minus do real.value := - real.value
     end else
     if unary.minus and le( ipart,minintstr ) or ~ unary.minus and le( ipart,maxintstr )
     then the.lit := if ipart = "" then 0 else int.conv( ipart,unary.minus )
     else err.mess1( "Integer literal out of range'n" )
     unary.minus := false
end

let recovered := true

forward directive

procedure next.sy
begin
     the.name := ""
     at.new.line := false
     let more := true
     while more do
     case peep of
     space,tab.sy : { let dis = next.ch }
     "%"      : { directive ; at.new.line := true }
     nl       : { let dis = next.ch ; at.new.line := true }
     "!"      : { while next.ch ~= nl and ~eof( si ) do { } ; at.new.line := true }
     default  : more := false

     if letter( peep ) then try.name else
     if digit( peep ) then number else
     case peep of
     colon.sy  : begin
                      symb := next.ch
                      if peep = colon.sy then { let dis = next.ch ; symb := dcolon.sy }
                                         else try( assign.sy )
                 end
     lt.sy     : { symb := next.ch ; try( le.sy ) }
     gt.sy     : { symb := next.ch ; try( ge.sy ) }
     not.sy    : { symb := next.ch ; try( neq.sy ) }
     dquote.sy : { let dis = next.ch ; read.string
                   lit.type := STRING ; symb := literal.sy }
     minus.sy  : { symb := next.ch ; try( arrow.sy ) }
     plus.sy   : { symb := next.ch ; try( dplus.sy ) }
     default   : symb := next.ch
end

procedure have( cstring s -> bool )
if s = symb then { next.sy ; true } else false

procedure syntax( cstring s )
if recovered do
begin
     err.mess( @1 of string[ "***** Syntax Error ***** ",symb," found where ",s," expected'n" ] )
     recovered := false
end

procedure mustbe( cstring s )
if s = symb then { recovered := true ; next.sy } else
if recovered then syntax( s ) else
begin
     while s ~= symb and symb ~= eot do next.sy
     if symb = s do { recovered := true ; next.sy }
end

procedure separator
if ~have( semi.sy ) and ~at.new.line do
if recovered then syntax( semi.sy ) else
begin
     while ~have( semi.sy ) and ~at.new.line and symb ~= eot do next.sy
     if symb ~= eot do recovered := true
end

!******************************* Type Matching *********************************

procedure display( cpntr t -> string )
if t is scalar then t( t.name ) else
if t is var then display( t( contents ) ) else
if t is const then "c" ++ display( t( content ) ) else
if t is VECTOR then star.sy ++ display( t( elms ) ) else
if t is Image then hash.sy ++ display( t( image.elements ) ) else
if t is STRUCTURE then "structure( " ++ display( t( fields ) ) ++ " )" else
if t is proc then "procedure(" ++ ( if t( args ) ~= nil then space ++ display( t( args ) ) else "" ) ++
                  ( if t( result ) ~= VOID then " -> " ++ display( t( result ) ) else "" ) ++ " )" else
if t is cons then display( t( hd ) ) ++ ( if t( tl ) ~= nil then "," ++ display( t( tl ) ) else "" ) else
if t is field then "field( " ++ display( t( fieldt ) ) ++ " )" else "unknown"

procedure bad.type( cpntr a )
if a ~= ANY do
err.mess( @1 of string[ "**** Type error**** ",display( a )," may not be used in this context'n" ] )

procedure bad.types( cpntr t,t1 )
if t ~= ANY and t1 ~= ANY do
err.mess( @1 of string[ "**** Type error**** ",display( t ),
                        " and ",display( t1 )," are not compatible in this context'n" ] )

procedure int.real( cpntr t -> pntr )
if eq( INT,t ) then INT else
if eq( REAL,t ) then REAL else { bad.types( INT,t ) ; ANY }

procedure match( cpntr a,b )
if ~eq( a,b ) do
if eq( REAL,a ) and eq( INT,b ) then float.op( 1 ) else bad.types( a,b )

procedure coerce( cpntr a,b -> pntr )
if a = ANY or b = ANY then ANY else
if eq( REAL,a ) then { match( REAL,b ) ; REAL } else
if eq( INT,a ) then if eq( REAL,b ) then { float.op( 2 ) ; REAL }
                                    else { match( INT,b ) ; INT }
               else { bad.types( a,b ) ; ANY }

procedure directive
begin
     let dis = next.ch
     option := true
     while peep = space do { let dis = next.ch }
     let s = read.name( si,peep ) ; peep := read( si )
     case s of
     "list"      : listing := true
     "nolist"    : listing := false
     "ul"        : underline := true
     "noul"      : underline := false
     "title"     : begin
                        while peep = space do { let dis = next.ch }
                        if peep = dquote.sy then
                        begin
                             let dis = next.ch
                             read.string
                             heading := ""
                             let t := the.string
                             for i = 1 to t( str.int ) do
                             begin
                                  t := t( str.pntr )
                                  heading := heading ++ code( t( str.int ) )
                             end
                             title
                        end else syntax( dquote.sy )
                   end
     "lines"     : { next.sy ; mustbe( eq.sy ) ; match( INT,lit.type )
                     lines.per.page := the.lit }
     "newline"   : newlins := true
     "nonewline" : newlins := false
     default     : syntax( "option" )
     while next.ch ~= nl do {}
     option := false
end

!************************* Scope and Name Handler ******************************

structure namelist( string names ; pntr types,next )

let prelude := true
let trademark := 0
let env.list := nil
let struct.list := struct.table( 0,0,nil )
let s.str.list := struct.list
let max.stack = 1023 ; let max.trademark = 4088 ; let forwd.count := 0

procedure enter.scope ; env.list := cons( nil,env.list )

procedure exit.scope ; env.list := env.list( tl )

procedure forwds( cpntr head )
if head ~= nil do
begin
     forwds( head( left ) )
     let t1 = head( type1 )
     if t1 is proc and t1( forwd ) do
     err.mess( @1 of string[ "Declaration of forward procedure <",head( name ),"> required'n" ] )
     forwds( head( right ) )
end

procedure name.err( cstring s )
err.mess( @1 of string[ "The name ",s," has already been used'n" ] )

procedure enter( cpntr head,temp ; cbool checked -> pntr )
case true of
head = nil : temp
temp( name ) < head( name ) : { head( left ) := enter( head( left ),temp,checked ) ; head }
temp( name ) > head( name ) : { head( right ) := enter( head( right ),temp,checked ) ; head }
default : if head( type1 ) is proc and head( type1,forwd ) and
             temp( type1 ) is proc then
          begin
               dec.ms( st.size2 )
               head
          end else
          begin
               if ~checked do name.err( head( name ) )
               head
          end

procedure search( cstring n ; cpntr head -> pntr )
begin
     let p := head
     while p ~= nil and n ~= p( name ) do
     p := if n < p( name ) then p( left ) else p( right )
     p
end

procedure declare( cstring n ; cpntr t ; cbool checked )
begin
     let t1 = if t is var then t( contents ) else
              if t is const then t( content ) else t
     let add2 = if pointer( t1 ) then psp else ssp
     if add2 > max.stack do { err.mess1( "Stack declaration limit'n" ) ; ssp := 0 ; psp := 0 }
     env.list( hd ) := enter( env.list( hd ),link( n,lex.level,add2,t,nil,nil ),checked )
end

procedure name.op( cpntr t ; cint n,n1 ; cbool r.value )
begin
     if t is field do load.lit( INT,t( f.offset ) )
     if t is proc and t( stand ) then load.st.func( n1 div st.size2 ) else load( n,n1,r.value,t )
end

procedure search.name( cstring n -> pntr )
begin
     let p2 := env.list ; let p := nil
     while p2 ~= nil and p = nil do
     begin
          p := search( n,p2( hd ) )
          p2 := p2( tl )
     end
     p
end

procedure name.err1( cstring n )
begin
     err.mess( @1 of string[ "**** Undeclared name **** <",n,"> has been used'n" ] )
     declare( n,ANY,true )
end

procedure look.up( pntr p ; cbool r.value -> pntr )
begin
     let p1 := p( type1 )
     if p1 is var do p1 := p1( contents )
     if p1 is const do p1 := p1( content )
     name.op( p1,p( addr1 ),p( addr2 ),r.value )
     p := p( type1 )
     if r.value then if p is var then p( contents )
             else if p is const then p( content ) else p
             else p
end

procedure lookup( cstring n ; cbool r.value -> pntr )
begin
     let p = search.name( n )
     if p = nil then { name.err1( n )  ; ANY } else look.up( p,r.value )
end

procedure check.name( cstring s )
begin
     let t = search( s,env.list( hd ) )
     if t ~= nil do name.err( s )
end

!*************************** Syntax Analysis ***********************************

forward clause( -> pntr )

procedure type( -> pntr )
case symb of
int.sy       : { next.sy ; INT }
pntr.sy      : { next.sy ; PNTR }
string.sy    : { next.sy ; STRING }
cint.sy      : { next.sy ; CINT }
cpntr.sy     : { next.sy ; CPNTR }
cstring.sy   : { next.sy ; CSTRING }
real.sy      : { next.sy ; REAL }
creal.sy     : { next.sy ; CREAL }
bool.sy      : { next.sy ; BOOL }
cbool.sy     : { next.sy ; CBOOL }
file.sy      : { next.sy ; FILE }
cfile.sy     : { next.sy ; CFILE }
pic.sy       : { next.sy ; PIC }
cpic.sy      : { next.sy ; CPIC }
pixel.sy     : { next.sy ; PIXEL }
cpixel.sy    : { next.sy ; CPIXEL }
hash.sy      : { next.sy ; if have( pixel.sy ) then IMAGE
                                else { mustbe( cpixel.sy ) ; CIMAGE } }
star.sy      : { next.sy ; VECTOR( type ) }
default      : if symb = identifier.sy and the.name = "c" then
               begin
                    next.sy
                    if have( star.sy ) then const( VECTOR( type ) ) else
                    begin
                      mustbe( hash.sy )
                      const( if have( pixel.sy ) then IMAGE
                             else { mustbe( cpixel.sy ) ; CIMAGE } )
                    end
               end else { syntax( "type" ) ; ANY }

procedure let.decl
begin
     next.sy
     let n = the.name
     check.name( n )
     mustbe( identifier.sy )
     let eq = have( eq.sy )
     if ~eq do mustbe( assign.sy )
     let t := clause
     if t = VOID or t is field do { bad.type( t ) ; t := ANY }
     declare( n,if eq then const( t ) else var( t ),true )
end

procedure struct.param( -> pntr )
begin
     let start = namelist( star.sy,nil,nil ) ; let t := start
     repeat { let t1 = type
              repeat { t( next ) := namelist( the.name,t1,nil )
                       check.name( the.name )
                       mustbe( identifier.sy )
                       t := t( next ) }
              while have( comma.sy ) }
     while have( semi.sy )
     start( next )
end

procedure struct.decl( -> pntr )
begin
     next.sy
     let n = the.name
     let ans = STRUCTURE( nil )
     declare( n,ans,false )
     mustbe( identifier.sy )
     if have( lp.sy ) then
     begin
          let t2 = struct.param
          let pointers := 0 ; let total := st.size ; let offset := st.size
          let ptypes := cons( nil,nil ) ; let sptypes = ptypes
          let start = namelist( star.sy,nil,nil ) ; let list := start
          let t := t2
          while t ~= nil do
          begin
               ptypes( tl ) := cons( t( types ),nil ) ; ptypes := ptypes( tl )
               if pointer( t( types ) ) then
               begin
                    pointers := pointers + 1
                    declare( t( names ),field( t( types ),offset ),false )
                    offset := offset + st.size
               end else
               begin
                    list( next ) := namelist( t( names ),t( types ),nil )
                    list := list( next )
               end
               total := total + type.size( t( types ) )
               t := t( next )
          end
          t := start( next )
          while t ~= nil do
          begin
               declare( t( names ),field( t( types ),offset ),false )
               offset := offset + type.size( t( types ) )
               t := t( next )
          end
          let ans = STRUCTURE( sptypes( tl ) )
          let p = search( n,env.list( hd ) )
          p( type1 ) := ans
          mustbe( rp.sy )
          struct.table( total,pointers,ans )
     end else struct.table( st.size2,0,ans )
end

procedure structure.decl
begin
     load.trademark( trademark )
     if trademark >= max.trademark do
     begin
          trademark := 0
          err.mess1( "Too many structures'n" )
     end
     let a = struct.decl
     struct.list( str.next ) := struct.table( a( s.tdm ),a( s.ptrs ),nil )
     struct.list := struct.list( str.next )
     trademark := trademark + 4
end

forward param.type( cbool -> pntr )

procedure get.types( cbool function -> pntr )
begin
     let arg.types := nil ; let restype := VOID
     if ~have( rp.sy ) do
     begin
          let res := function and have( arrow.sy )
          if ~res do
          begin
               arg.types := cons( if function then param.type( false ) else type,nil )
               let t := arg.types
               while have( comma.sy ) do
               begin
                    t( tl ) := cons( if function then param.type( false ) else type,nil )
                    t := t( tl )
               end
               res := if function then have( arrow.sy ) else false
          end
          if res do
          begin
               restype := type
               if restype is const do bad.type( restype )
          end
          mustbe( rp.sy )
     end
     if function then proc( arg.types,restype,false,false )
                 else STRUCTURE( arg.types )
end

procedure external.decl
begin
     next.sy
     let n = the.name
     check.name( n )
     mustbe( identifier.sy )
     let t = if have( lp.sy ) then get.types( true ) else proc( nil,VOID,false,false )
     external.op( n,t )
     declare( n,t,true )
end

procedure forward.decl
begin
     next.sy
     let n = the.name
     check.name( n )
     mustbe( identifier.sy )
     let t = if have( lp.sy ) then get.types( true ) else proc( nil,VOID,true,false )
     t( forwd ) := true
     forwd.count := forwd.count + 1
     forward.op
     declare( n,t,true )
end

procedure param.type( cbool names -> pntr )
case symb of
structure.sy : if names then { push.ms ; struct.decl( str.next ) } else
               begin
                    next.sy
                    if have( lp.sy ) then get.types( false )
                                     else STRUCTURE( nil )
               end
lp.sy        : { next.sy ; get.types( true ) }
default      : type

procedure param.decl( -> pntr )
begin
     let start = cons( nil,nil ) ; let t := start
     repeat { let t1 = param.type( true )
              t( tl ) := cons( t1,nil ) ; t := t( tl )
              if t1 isnt STRUCTURE do
              repeat { let n = the.name
                       inc.stack( t1 )
                       let t = if t1 is proc or t1 is const then t1 else var( t1 )
                       declare( n,t,false )
                       mustbe( identifier.sy ) }
              while have( comma.sy ) do { t( tl ) := cons( t1,nil ) ; t := t( tl ) } }
     while have( semi.sy )
     start( tl )
end

procedure proc.decl
begin
     next.sy
     let n = the.name
     inc.ms( st.size2 )
     let the.type = proc( nil,VOID,false,false )
     declare( n,the.type,false )
     let level = ssp ; let plevel = psp
     let max.m = max.ms ; let max.p = max.ps
     psp := 0 ; max.ps := psp
     let cb = code.base ; let code.p = cp
     code.base := code.base + cp ; cp := 4
     let save.pd = p.decl( proc.decl.list,code.p + 3 )
     last.retract := -1 ; proc.decl.list := nil
     let save.struct = str.decl.list ; str.decl.list := nil
     let save.ex = ex.decl.list ; ex.decl.list := nil
     lex.level := lex.level + 1
     enter.scope
     ssp := st.size2
     declare( n,the.type,false )
     ssp := mscw.size ; max.ms := ssp
     enter.scope
     let nn = proc.name ; proc.name := n
     mustbe( identifier.sy )
     let arg.types := nil ; let restype := VOID
     if have( lp.sy ) do
     begin
          if symb ~= arrow.sy do arg.types := param.decl
          if have( arrow.sy ) do
          begin
               restype := type
               if restype is const do bad.type( restype )
          end 
          mustbe( rp.sy )
     end
     the.type( args ) := arg.types ; the.type( result ) := restype
     let pentry = search( n,env.list( tl,tl,hd ) )
     let ptype = pentry( type1 )
     if ptype is proc and ptype( forwd ) do
     begin
          if eq( ptype( args ),arg.types ) and eq( ptype( result ),restype ) then
          begin
               forwd.count := forwd.count - 1
               ptype( forwd ) := false
          end else name.err( n )
     end
     separator
     if ext do load.struct.base
     load.proc.name( n )
     let save.ext = ext ; ext := false
     match( restype,clause )
     ext := save.ext
     exit.scope ; exit.scope
     return( restype )
     let n1 = cp ; lex.level := lex.level - 1
     code.base := cb ; cp := code.p
     ssp := level ; psp := plevel
     proc.name := nn
     endsegment( n1,pentry( addr2 ) )
     proc.decl.list := save.pd
     str.decl.list := save.struct ; ex.decl.list := save.ex
     max.ms := max.m ; max.ps := max.p
     if ext do procedure.name := n ; procedure.type := the.type
end

forward sequence( -> pntr )

procedure block( -> pntr )
begin
     block.count := block.count + 1
     let last = if symb = lcb.sy then rcb.sy else end.sy
     next.sy
     let t = if symb = last then VOID else
             begin
                  enter.scope
                  let t1 = sequence
                  exit.scope
                  t1
             end
     block.count := block.count - 1
     mustbe( last )
     t
end

forward expr( cint -> pntr )

procedure rotate.pic( -> pntr )
begin
     let level = ssp ; let plevel = psp
     let ttt = lookup( "trnsfrm.strc",true )
     next.sy
     load.lit( INT,3 )
     match( PIC,clause )
     mustbe( by.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     load.real( 0 )
     reverse.stack( REAL )
     form.structure( level,plevel )
     PIC
end

procedure colour.pic( -> pntr )
begin
     let level = ssp ; let plevel = psp
     let ttt = lookup( "culr.strc",true )
     next.sy
     match( PIC,clause )
     mustbe( in.sy )
     match( STRING,clause )
     form.structure( level,plevel )
     PIC
end

procedure transform.pic( -> pntr )
begin
     let level = ssp ; let plevel = psp
     let scaled = ( symb = scale.sy )
     next.sy
     let ttt = lookup( "trnsfrm.strc",true )
     load.lit( INT,if scaled then 1 else 2 )
     match( PIC,clause )
     mustbe( by.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     mustbe( comma.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     form.structure( level,plevel )
     PIC
end

procedure text.pic( -> pntr )
begin
     let level = ssp ; let plevel = psp
     let ttt = lookup( "scrbl.strc",true )
     next.sy
     match( STRING,clause )
     mustbe( from.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     mustbe( comma.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     mustbe( to.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     mustbe( comma.sy )
     match( REAL,clause )
     reverse.stack( REAL )
     form.structure( level,plevel )
     PIC
end

procedure image.exp( -> pntr )
begin
     next.sy
     match( INT,clause )
     mustbe( by.sy )
     match( INT,clause )
     mustbe( of.sy )
     match( PIXEL,clause )
     image.op
     IMAGE
end

procedure limit.exp( -> pntr )
begin
     next.sy
     let t = clause
     match( image.type,t )
     let to.symb = have( to.sy )
     if to.symb do
     begin
       match( INT,clause )
       mustbe( by.sy )
       match( INT,clause )
     end
     if have( at.sy ) then
     begin
       match( INT,clause )
       mustbe( comma.sy )
       match( INT,clause )
     end else { load.lit( INT,0 ) ; load.lit( INT,0 ) }
     limit.op( to.symb )
     t
end
procedure standard.func( -> pntr )
begin
     procedure arg1( cpntr t )
     begin
          next.sy
          mustbe( lp.sy )
          match( t,clause )
          mustbe( rp.sy )
     end

     procedure format( cint n,ms -> pntr )
     begin
          mst.external( n )
          next.sy
          mustbe( lp.sy )
          match( REAL,clause )
          if n ~= 2 do for i = 1 to 2 do { mustbe( comma.sy ) ; match( INT,clause ) }
          mustbe( rp.sy )
          apply.op( ms,0 )
          push.ps
          STRING
     end

     procedure do.io
     begin
          next.sy
          if symb = lp.sy then { next.sy ; match( FILE,clause ) ; mustbe( rp.sy ) }
                          else match( FILE,lookup( "s.i",true ) )
     end

     procedure vec.bound( cint n -> pntr )
     begin
          next.sy
          mustbe( lp.sy )
          let t := clause ; if t is const do t := t( content )
          match( vector.type,t )
          if t is VECTOR do bound( n,t( elms ) )
          mustbe( rp.sy )
          INT
     end

     case symb of
     abort.sy  : { next.sy ; finish.op ; VOID }
     upb.sy    : vec.bound( 0 )
     lwb.sy    : vec.bound( 1 )
     float.sy  : { arg1( INT ) ; float.op( 1 ) ; REAL }
     eof.sy    : { do.io ; read.op( 6 ) ; BOOL }
     read.sy   : { do.io ; read.op( 5 ) ; STRING }
     peek.sy   : { do.io ; read.op( 4 ) ; STRING }
     reads.sy  : { do.io ; read.op( 1 ) ; STRING }
     readi.sy  : { do.io ; read.op( 0 ) ; INT }
     readr.sy  : { mst.external( 4 ) ; do.io ; apply.op( 0,st.size ) ; inc.ms( st.size2 ) ; REAL }
     readb.sy  : { do.io ; read.op( 2 ) ; BOOL }
    read.16.sy : { do.io ; read.op( 9 ) ; INT }
    read.32.sy : { do.io ; read.op( 10 ) ; INT }
    eformat.sy : format( 1,st.size2 + 2 * st.size )
    fformat.sy : format( 0,st.size2 + 2 * st.size )
    gformat.sy : format( 2,st.size2 )
read.a.line.sy : { do.io ; read.op( 8 ) ; STRING }
  read.byte.sy : { do.io ; read.op( 3 ) ; INT }
  read.name.sy : begin
                      next.sy
                      mustbe( lp.sy )
                      let t = clause
                      if eq2( FILE,t ) then { mustbe( comma.sy ) ; match( STRING,clause ) }
                                       else { match( STRING,t ) ; match( FILE,lookup( "s.i",true ) )
                                              reverse.stack( STRING ) }
                      read.op( 7 )
                      mustbe( rp.sy )
                      STRING
                 end
  pic.pntr.sy  : { arg1( PIC ) ; PNTR }
     default   : { syntax( "expression" ) ; next.sy ; ANY }
end

procedure args.list( cpntr t -> pntr )
begin
     let level = ssp - st.size ; let plevel = psp
     let t1 := t( fields )
     if t1 = nil then load.lit( INT,0 ) else
     begin
          mustbe( lp.sy )
          repeat{
                  match( t1( hd ),clause )
                  if eq2( REAL,t1( hd ) ) do reverse.stack( REAL )
                  t1 := t1( tl ) }
          while t1 ~= nil and have( comma.sy )
          if t1 ~= nil do syntax( "field" )
          mustbe( rp.sy )
     end
     form.structure( level,plevel )
     PNTR
end

procedure proc.call( pntr p -> pntr )
begin
     let t := p( type1 )
     let t1 := t( args )
     if mark.stack( p ) do
     begin
          let level = ssp ; let plevel = psp
          if t1 ~= nil do
          begin
               mustbe( lp.sy )
               repeat
               begin
                    let t2 = t1( hd )
                    case true of
                    t2 is proc      : { match( t2,lookup( the.name,true ) ) ; next.sy }
                    t2 is STRUCTURE : begin
                                           let t3 = lookup( the.name,true )
                                           if ~( t3 is STRUCTURE and eq( t2( fields ),t3( fields ) ) ) do
                                           bad.types( t2,t3 )
                                           next.sy
                                      end
                    default         : match( t2,clause )
                    t1 := t1( tl )
               end while t1 ~= nil and have( comma.sy )
               if t1 ~= nil do syntax( "parameter" )
               mustbe( rp.sy )
          end
          apply.op( ssp - level,psp - plevel )
     end
     t := t( result )
     inc.stack( t )
     t
end

procedure le1( cint n,p )
begin
     if n > p do
     begin
          let err = error ; let code1 = coding ; let cp1 = cp
          err.mess1( "** Warning higher precedence expression'n" )
          error := err ; coding := code1 ; cp := cp1
     end
     next.sy
end

procedure ass.err( cpntr t )
err.mess( @1 of string[ "Type ",display( t )," may not be assigned to'n" ] )

procedure unary.op( cint n -> pntr )
case symb of
identifier.sy : begin
                      let n1 = the.name
                      let p = search.name( n1 )
                      if p = nil do name.err1( n1 )
                      next.sy
                      if p = nil then ANY else
                      begin
                           let t := p( type1 )
                           if t is proc then proc.call( p ) else
                           if t is STRUCTURE then args.list( look.up( p,true ) ) else
                           if symb = assign.sy then
                           begin
                                t := if t is var then t( contents )
                                                 else { ass.err( t ) ; ANY }
                                next.sy
                                match( t,clause )
                                t := look.up( p,false )
                                VOID
                           end else look.up( p,true )
                      end
                 end
literal.sy    : begin
                      let t = lit.type
                      if t = REAL then load.real( real.value ) else
                        if t = STRING then load.string( the.string )
                                      else load.lit( t,the.lit )
                      next.sy
                      t
                 end
begin.sy,lcb.sy: block
lp.sy         : begin
                      next.sy
                      let t = clause
                      mustbe( rp.sy )
                      t
                 end
lsb.sy        : begin
                      next.sy
                      let level = ssp ; let plevel = psp
                      let t = lookup( "poin.strc",true )
                      match( REAL,clause )
                      reverse.stack( REAL )
                      mustbe( comma.sy )
                      match( REAL,clause )
                      reverse.stack( REAL )
                      mustbe( rsb.sy )
                      form.structure( level,plevel )
                      PIC
                 end
vector.sy     : begin
                      let bounds := 0 ; let t := VECTOR( nil ) ; let start = t
                      next.sy
                      repeat{ match( INT,clause )
                              mustbe( dcolon.sy )
                              match( INT,clause )
                              bounds := bounds + 1 }
                      while have( comma.sy ) do t := VECTOR( t )
                      mustbe( of.sy )
                      let t1 = clause ; start( elms ) := t1
                      if t1 = VOID do bad.type( t1 )
                      iliffe.op( t1,bounds )
                      t
                 end
          "@"  : begin
                      let level = ssp ; let plevel = psp
                      next.sy
                      match( INT,clause ) ; mustbe( of.sy )
                      let t = type
                      mustbe( lsb.sy )
                      repeat match( t,clause ) while have( comma.sy )
                      mustbe( rsb.sy )
                      make.vector( t,level,plevel )
                      VECTOR( t )
                 end
     not.sy    : { le1( n,3 ) ; match( BOOL,expr( 3 ) ) ; not.op ; BOOL }
     plus.sy   : { le1( n,4 ) ; int.real( expr( 4 ) ) }
     minus.sy  : begin
                      unary.minus := true
                      le1( n,4 )
                      let neg = unary.minus ; unary.minus := false
                      let t1 = int.real( expr( 4 ) )
                      if neg do neg.op( t1 )
                      t1
                 end
     shift.sy,
     scale.sy  : transform.pic
     rotate.sy : rotate.pic
     colour.sy : colour.pic
     text.sy   : text.pic
     image.sy  : image.exp
     limit.sy  : limit.exp
     rand.sy,
     rnot.sy,
     xor.sy,
     xnor.sy,
     nor.sy,
     nand.sy,
     ror.sy,
     copy.sy   : begin
                      let s = symb
                      next.sy
                      match( image.type,clause )
                      mustbe( onto.sy )
                      match( IMAGE,clause )
                      Raster.op( s )
                      VOID
           end
     default   : standard.func

procedure exp.list( cpntr t -> pntr )
begin
     procedure subscript( pntr t1,t2 -> pntr )
     if eq2( PNTR,t1 ) and t2 is field then t2( fieldt ) else
     if eq2( vector.type,t1 ) and eq2( INT,t2 ) then
     begin
          if t1 is const do t1 := t1( content )
          t1( elms )
     end else { bad.types( t1,t2 ) ; ANY }

     let t1 := t ; let t2 := t
     mustbe( lp.sy )
     repeat t1 := subscript( t2,clause )
     while have( comma.sy ) do { sub.op( t1,t2 ) ; t2 := t1 }
     mustbe( rp.sy )
     if symb = assign.sy then
     begin
          if t1 is const do ass.err( t1 )
          next.sy
          match( t1,clause )
          sub.ass( t1,t2 )
          VOID
     end else { sub.op( t1,t2 ) ; if t1 is const then t1( content ) else t1 }
end

procedure rel.type( cpntr t,t1 -> pntr )
if eq2( INT,t ) then coerce( t,t1 ) else
if eq( REAL,t ) or eq( STRING,t ) then { match( t,t1 ) ; t }
                                  else { bad.types( t,t1 ) ; ANY }

procedure expr( cint n -> pntr )
begin
     newline
     let more := true ; let s := "" ; let t := unary.op( n )

     procedure le2( cint n,p -> bool )
     if n <= p then { s := symb ; next.sy ; true }
               else { more := false ; false }

     procedure and.or.op( cint n1 )
     if le2( n,n1 ) do
     begin
          match( BOOL,t )
          let l = if n1 = 0 then jumptt( newlab ) else jumpff( newlab )
          match( BOOL,expr( n1 + 1 ) )
          setlab( l )
          t := BOOL
     end

     procedure join( cint m )
     if eq( PIC,t ) then
     begin
          next.sy
          let level = ssp ; let plevel = psp - st.size
          match( PIC,t )
          let ttt = lookup( "oprtn.strc",true )
          match( PIC,expr( 5 ) )
          load.lit( INT,m )
          form.structure( level,plevel )
          t := PIC
     end else more := false

     while more and ~at.new.line do
     case symb of
     or.sy         : and.or.op( 0 )
     and.sy        : and.or.op( 1 )
     eq.sy,neq.sy  : if le2( n,2 ) do
                     begin
                          if eq2( INT,t ) then t := coerce( t,expr( 3 ) )
                                          else match( t,expr( 3 ) )
                          comp( s,t )
                          t := BOOL
                     end
     le.sy,lt.sy,
     ge.sy,gt.sy   : if le2( n,2 ) do
                     begin
                          t := rel.type( t,expr( 3 ) )
                          comp( s,t )
                          t := BOOL
                     end
     is.sy,isnt.sy : if le2( n,2 ) do
                     begin
                          match( PNTR,t )
                          t := lookup( the.name,true )
                          if t isnt STRUCTURE do bad.type( t )
                          mustbe( identifier.sy )
                          is.op( s ~= isnt.sy )
                          t := BOOL
                     end
     dplus.sy      : if le2( n,3 ) do
                     begin
                          match( STRING,t )
                          match( STRING,expr( 4 ) )
                          concat.op
                           t := STRING
                     end
  plus.sy,minus.sy : if le2( n,3 ) do
                     { t := coerce( t,expr( 4 ) ) ; arith.op( t,s ) }
          amper.sy : if eq( PIXEL,t ) then
                        if le2( n,2 ) do
                        begin
                             let n := 1
                             repeat { match( PIXEL,expr( 3 ) ) ; n := n + 1 }
                             while have( amper.sy )
                             form.pixel.op( n )
                        end
                     else join( 1 )
      hat.sy       : join( 2 )
     star.sy       : if le2( n,4 ) do
                     { t := coerce( t,expr( 5 ) ) ; arith.op( t,s ) }
     divide.sy     : if le2( n,4 ) do
                     begin
                          match( REAL,t )
                          match( REAL,expr( 5 ) )
                          arith.op( REAL,s )
                          t := REAL
                     end
     rem.sy,div.sy : if le2( n,4 ) do
                     begin
                          match( INT,t )
                          match( INT,expr( 5 ) )
                          arith.op( t,s )
                          t := INT
                     end
     lp.sy         : t := if eq2( STRING,t ) then
                     begin
                          next.sy ; match( INT,clause ) ; mustbe( bar.sy )
                          match( INT,clause ) ; mustbe( rp.sy ) ; substr.op
                          STRING
                     end else
                     if eq( image.type,t ) then
                     begin
                          next.sy ; match( INT,clause ) ; mustbe( bar.sy )
                          match( INT,clause ) ; mustbe( rp.sy ) ; subimage.op
                          t
                     end else
                     if eq( PIXEL,t ) then
                     begin
                          next.sy ; match( INT,clause ) ; mustbe( rp.sy )
                          subpixel.op
                          PIXEL
                     end else exp.list( t )
     default       : more := false
     t
end

procedure if.clause( -> pntr )
begin
     next.sy
     match( BOOL,clause )
     let l = jumpf( newlab )
     if have( do.sy ) then
     begin
          match( VOID,clause )
          setlab( l )
          VOID
     end else
     begin
          mustbe( then.sy )
          let t = clause ; let m = fjump( newlab )
          dec.stack( t )
          setlab( l )
          mustbe( else.sy )
          let t1 = clause
          if eq2( INT,t ) and eq2( REAL,t1 ) then
          begin
               let n = fjump( newlab )
               setlab( m )
               float.op( 1 ) ; dec.stack( INT )
               setlab( n )
               REAL
          end else { match( t,t1 ) ; setlab( m ) ; t }
     end
end

procedure for.clause( -> pntr )
begin
     next.sy
     let level = ssp ; let plevel = psp
     let n = the.name
     mustbe( identifier.sy )
     mustbe( eq.sy )
     match( INT,clause )
     enter.scope
     let sp = ssp
     mustbe( to.sy )
     match( INT,clause )
     if have( by.sy ) then match( INT,clause ) else load.lit( INT,1 )
     env.list( hd ) := enter( env.list( hd ),link( n,lex.level,sp,CINT,nil,nil ),true )
     mustbe( do.sy )
     let l = cp ; let m = for.test( newlab )
     match( VOID,clause )
     for.step( l )
     setlab( m )
     exit.scope
     ssp := level ; psp := plevel
     VOID
end

procedure while.clause( -> pntr )
begin
     next.sy
     let l = cp
     match( BOOL,clause )
     let m = jumpf( newlab )
     mustbe( do.sy )
     match( VOID,clause )
     bjump( l )
     setlab( m )
     VOID
end

procedure repeat.clause( -> pntr )
begin
     next.sy
     let l = cp
     match( VOID,clause )
     mustbe( while.sy )
     match( BOOL,clause )
     if have( do.sy ) then
     begin
          let m = jumpf( newlab )
          match( VOID,clause )
          bjump( l )
          setlab( m )
     end else bjumpt( l )
     VOID
end

procedure case.clause( -> pntr )
begin
     next.sy
     let t := clause ; let t2 := ANY ; let t1 := ANY
     if t = VOID do bad.type( t )
     let xit := newlab ; let xit1 := newlab ; let mixed.mode := false ; let deflt := false

     procedure match.it
     begin
          if deflt then erase.op( t ) else dec.stack( t )
          t1 := clause
          if eq2( t2,INT ) and eq2( t1,REAL ) then mixed.mode := true else
          if eq2( REAL,t2 ) then t1 := coerce( t2,t1 ) else match( t2,t1 )
          if mixed.mode then xit1 := fjump( xit1 )
                        else if ~deflt do xit := fjump( xit )
          if ~deflt do dec.stack( t1 )
     end

     mustbe( of.sy )
     while ~have( default.sy ) and symb ~= eot do
     begin
          let l := newlab
          repeat {
                  if eq2( INT,t ) then t := coerce( t,clause ) else match( t,clause )
                  l := cjump( t,l ) }
          while have( comma.sy )
          let m = fjump( newlab )
          mustbe( colon.sy )
          setlab( l )
          match.it ; inc.stack( t )
          setlab( m )
          t2 := t1
          separator
     end
     mustbe( colon.sy )
     deflt := true
     match.it
     setlab( xit )
     if mixed.mode do { float.op( 1 ) ; dec.stack( INT ) ; setlab( xit1 ) }
     t1
end

procedure write.clause( cbool wrt -> pntr )
begin
     newline
     next.sy
     if wrt then match( FILE,lookup( "s.o",true ) )
            else { match( FILE,clause ) ; mustbe( comma.sy ) }
     repeat
     begin
          let t = clause
          if have( colon.sy ) then match( INT,clause )
                              else load.lit( INT,-1 )
          if eq( REAL,t ) then { mst.external( 3 ) ; apply.op( st.size2 + st.size,0 ) } else
          write.op( case t of
                    STRING,CSTRING : 1
                    INT,CINT       : 0
                    BOOL,CBOOL     : 2
                    default :{ bad.type( t ) ; 0 } )
     end while have( comma.sy )
     erase.op( FILE )
     VOID
end

procedure clause( -> pntr )
case symb of
identifier.sy,literal.sy : expr( -1 )
if.sy      : if.clause
repeat.sy  : repeat.clause
while.sy   : while.clause
for.sy     : for.clause
case.sy    : case.clause
write.sy   : write.clause( true )
output.sy  : write.clause( false )
out.byte.sy,
out.16.sy,
out.32.sy  : begin
                  let s = symb
                  newline
                  next.sy
                  match( FILE,clause )
                  for i = 1 to if s = out.32.sy then 1 else 2 do
                       { mustbe( comma.sy ) ; match( INT,clause ) }
                  write.op( case s of
                            out.byte.sy : 3
                            out.16.sy   : 4
                            default     : 5 )
                  VOID
             end
default    : expr( -1 )

procedure sequence( -> pntr )
begin

     procedure more( -> bool )
     symb ~= rcb.sy and symb ~= end.sy and symb ~= eot

     let type := VOID
     let level = ssp ; let plevel = psp
     let save = forwd.count ; forwd.count := 0
     while more do
     begin
          match( VOID,type )
          case symb of
          procedure.sy : proc.decl
          structure.sy : structure.decl
          forward.sy   : forward.decl
          external.sy  : external.decl
          default      : begin
                              if forwd.count > 0 do { forwd.count := 0 ; forwds( env.list( hd ) ) }
                              if symb = let.sy then let.decl else type := clause
                         end
          if more do separator
     end
     if forwd.count > 0 do forwds( env.list( hd ) )
     forwd.count := save
     if ~prelude do retract( level,plevel,type )
     type
end

procedure declare1( cstring s ; cpntr t )
begin
     let t1 = if t is var then t( contents ) else t( content )
     inc.stack( t1 )
     declare( s,t,true )
end

procedure read.prelude
begin
     if ~ext do load.struct.base
     coding := true
     load.proc.name( proc.name )
     si := Open.prelude
     peep := read( si )
     next.sy
     let t = sequence
     prelude := false
     close( si )
end
	
procedure read.stand.F
begin
     let flsave = flsum ; flsum := false
     let lis = listing

     inc.ms( 5 * st.size2 )
     let save = ssp ; ssp := -2
     next.sy
     while ~eof( si ) do
     begin
          let n = the.name
          mustbe( identifier.sy )
          let t = if have( lp.sy ) then get.types( true ) else proc( nil,VOID,false,true )
          t( stand ) := true
          inc.ms( st.size2 )
          if n ~= "DUMMY" do declare( n,t,true )
     end
     ssp := save
     close( si )
     read.prelude
     si := SI
     peep := read( si )
     option := false ; coding := true
     line.count := 0 ; line.pos := 0
     err.pos := 10 ; flsum := flsave
     listing := lis
end

procedure init
begin
     s.w := 0
     declare1( "i.w",var( INT ) )
     declare1( "s.w",var( INT ) )
     declare1( "r.w",var( INT ) )
     declare1( "maxint",const( INT ) )
     declare1( "epsilon",const( REAL ) )
     declare1( "pi",const( REAL ) )
     declare1( "maxreal",const( REAL ) )
     declare1( "s.o",var( FILE ) ) ; declare1( "s.i",var( FILE ) )
     inc.stack( image.type ) ; declare( "screen",const( IMAGE ),true )
     inc.stack( image.type ) ; declare( "cursor",const( IMAGE ),true )
     read.stand.F
end

!************************* Main Program *****************************

let version = Version.no ; let block.no := 0 ; let null = "'"'""

procedure output.name.type( cstring s ; cpntr s1 -> int )
begin
     output so,dquote.sy,s,space
     let s2 = display( s1 )
     output so,s2,dquote.sy
     length( s ) + length( s2 ) + 3
end

procedure output.externals( -> int )
begin
     let size := 0
     while external.list ~= nil do
     begin
          let t := external.list( hd )
          while t ~= nil do
          begin
               size := size + output.name.type( t( e.name ),t( e.type ) ) + 2
               bout( shift.r ( t( e.addr ) + 1,8 ) ) ; bout( t( e.addr ) + 1 )
               t := t( e.next )
          end
          external.list := external.list( tl )
     end
     output so,null
     size + 2
end

procedure output.st.info( cint total,code.total )
begin
     let trailer.size = 24
     let i = 128 - ( total + trailer.size ) rem 128
     for j = 1 to i do output so,space

     boutl( code.total )
     boutl( last.segment )
     boutl( code.size )
     bout( shift.r( externals,8 ) ) ; bout( externals )
     bout( if flsum then shift.r( line.count + 1,8 )  else 0 )
     bout( if flsum then line.count + 1 else 0 )
     bout( shift.r( version,8 ) ) ; bout( version )
     boutl( total + i + trailer.size )
     bout( shift.r( block.no,8 ) ) ; bout( block.no )
end

enter.scope
init
next.sy
if page.count = 0 do title
if ext then
while symb = procedure.sy do
begin
     enter.scope
     proc.decl
     exit.scope
     let code.total = code.size + out.list( s.str.list( str.next ) )
     s.str.list := struct.table( 0,0,nil ) ; struct.list := s.str.list
     let total = code.total + output.name.type( procedure.name,procedure.type ) +
                   output.externals
          block.no := block.no + 1
          output.st.info( total,code.total )
          externals := 0 ; line.count := 0 ; trademark := 0
end else
begin
     match( VOID,sequence )
     exit.scope
     finish.op
     let temp.cp := cp ; cp := 0
     endsegment( temp.cp,0 )
     let code.total = code.size + out.list( s.str.list( str.next ) )
     output so,null
     let total = code.total + output.externals + 2
     if total = code.total + 4 do block.no := -1
     output.st.info( total,code.total )
end
if symb ~= eot do { syntax( eot ) ; while ~eof( si ) do next.sy }
line.image( line.pos ) := nl ; line.pos := line.pos + 1
output.line ; close( so )
shutdown
if flsum do close( fl.sum )
if error do Delete.code.file
?
