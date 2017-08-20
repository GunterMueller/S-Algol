let o.lwb = 3
let max.Lines = 40

! print an s-code file in mnemonic form
let procv.addr := 0 ; let strv.addr := 0
let pp := true
let end.proc := true
let lf = code( 10 )
let titles = "loc   opode  instruction      params( decimal )'n'n"
let put.titles := false ; let Line := 3
structure item (cstring iname; cint ikind; int icnt)
	! values for ikind -	0  no parameter
	!			1  single byte parameter
	!			2  single word parameter
	!			3  two single byte parameters
	!			4  single longword parameter
	!			5  two single word parameters
	!			6  return ops
	!			7  three single word parameters
	!			8  real literal
	!			9  string literal
 	!			10 single byte and two words
	!			11 single byte,single longword
	!			12 single word and single longword
	!			13 single byte and single word

let hole = item ("undefined",0,0)

procedure Lo( cint x -> int )
b.and( x,255 )

procedure Up( cint x -> int )
Lo( shift.r( x,8 ) )

let hc = @0 of cstring [ "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"]

procedure W( cint v )
if pp do write hc( v div 16 ),hc( v rem 16 )

procedure hex( int n ; cint no.bytes -> string )
begin
	let ans := ""
	for i = 1 to 2 * no.bytes do
	{ ans := hc( b.and( n,15 ) ) ++ ans ; n := shift.r( n,4 ) }
	ans
end

procedure check.page
if pp do
if Line = max.Lines do
begin
	write "'p'n'n"
	if put.titles then { write titles ; Line := 4 }
	else Line := 2
end

procedure newLine( cint n )
if pp do
begin
	if Line + n >= max.Lines then { write "'n'n" ; Line := 2 }
	else { for i = 1 to n do write "'n" ; Line := Line + n }
end

procedure Line.feed( cint i )
if pp do for i = 1 to i do write lf

procedure Wstr(cint v)
if pp do if v < 32 or v > 126 then write "\",hex( v,1 )
	 else write code (v)

procedure Write ( cstring w )
if pp do write w

procedure Write.word (cint v)
begin
W (Up(v) )
W (Lo(v) )
end

procedure Write.word.sp (cint v)
if pp do
	begin
	Write.word (v)
	write "   "
	end

procedure Write.word.dec (cint n)
if pp do
	begin
	Write.word.sp (n)
	write "(" ++ iformat (n) ++ ")" : 7
	end

procedure Right.j( cstring s ; cint n,bytes )
if pp do
begin
	write "":25,s,":":25 - length( s ),hex( n,bytes):0," ( ",iformat( n )," )"
	newLine( 1 )
end

procedure put( cint loc,opcode ; cstring inst )
if pp do
write hex( loc,2),hex( opcode,1):4,"":6,inst,"":17 - length( inst )

procedure put.param( cint n,bytes )
if pp do
begin
	write hex( n,bytes ):bytes * 2 + 2,"(",iformat( n ),")"
end

procedure Seek.back ( cfile f ;  int n )
seek ( f , - n , 1 )

!----------------------------------------------------------------------------
! OP-CODES

procedure OPvec( -> *cpntr)
@0 of cpntr [
	item( "no op",0,0 ),		!    0  
	item( "bjump",1,0 ),		!    1  
	hole,				!    2  
	hole,				!    3  
	hole,				!    4  
	hole,				!    5  
	item( "for.step",1,0 ),		!    6  
	item( "cjump.ib",2,0 ),		!    7
	item( "cjump.s",2,0 ),		!    8
	hole,				!    9
	item( "bjumpt",1,0 ),		!   10
	hole,				!   11  
	item( "local",1,0 ),		!   12  
	item( "plocal",1,0 ),		!   13  
	item( "dlocal",1,0 ),		!   14  
	item( "mst.local",1,0 ),	!   15  
	item( "global",1,0 ),		!   16  
	item( "pglobal",1,0 ),		!   17  
	item( "dglobal",1,0 ),		!   18  
	item( "mst.global",1,0 ),	!   19  
	hole,				!   20  
	hole,				!   21  
	hole,				!   22  
	hole,				!   23  
	item( "load",3,0 ),		!   24  
	item( "pload",3,0 ),		!   25  
	item( "dload",3,0 ),		!   26  
	item( "mst.load",3,0 ),		!   27  
	item( "local.ass",1,0 ),	!   28  
	item( "plocal.ass",1,0 ),	!   29  
	item( "dlocal.ass",1,0 ),	!   30  
	hole,				!   31
	item( "global.ass",1,0 ),	!   32  
	item( "pglobal.ass",1,0 ),	!   33  
	item( "dglobal.ass",1,0 ),	!   34  
	hole,				!   35
	hole,				!   36
	hole,				!   37  
	hole,				!   38
	hole,				!   39  
	item( "load.ass",3,0 ),		!   40
	item( "pload.ass",3,0 ),	!   41  
	item( "dload.ass",3,0 ),	!   42  
	hole,				!   43  
	item( "apply.op",1,0 ),		!   44
	item( "store.closure",12,0 ),	!   45  
	hole,				!   46
	item( "return.ib",6,0 ),	!   47  
	item( "return.s",6,0 ),		!   48  
	hole,				!   49  
	hole,				!   50
	item( "global.apply",1,0 ),	!   51  
	item( "local.apply",1,0 ),	!   52  
	item( "retract.ib",3,0 ),	!   53  
	item( "retract.r",3,0 ),	!   54  
	item( "retract.s",3,0 ),	!   55  
	item( "retract.p",3,0 ),	!   56  
	hole,				!   57  
	item( "retract.v",3,0 ),	!   58  
	item( "load.sfn",1,0 ),		!   59  
	item( "apply.sfn",1,0 ),	!   60  
	item( "mst.external",1,0 ),	!   61  
	item( "forward",0,0 ),		!   62  
	item( "load.apply",3,0 ),	!   63  
	item( "form.structure",2,0 ),	!   64  
	item( "is.op",0,0 ),		!   65  
	item( "subs.ib",0,0 ),		!   66  
	item( "subs.s",0,0 ),		!   67  
	hole,				!   68  
	item( "subsass.ib",0,0 ),	!   69  
	item( "subsass.s",0,0 ),	!   70  
	hole,				!   71  
	item( "makev.ib",2,0 ),		!   72  
	item( "makev.s",2,0 ),		!   73  
	hole,				!   74  
	item( "iliffe.ib",2,0 ),	!   75  
	item( "iliffe.s",2,0 ),		!   76  
	hole,				!   77  
	item( "subv.ib",0,0 ),		!   78  
	item( "subv.s",0,0 ),		!   79  
	hole,				!   80  
	item( "subvass.ib",0,0 ),	!   81  
	item( "subvass.s",0,0 ),	!   82  
	hole,				!   83  
	item( "upb.op",0,0 ),		!   84  
	item( "concat.op",0,0 ),	!   85  
	item( "load.trademark",11,0 ),	!   86  
	hole,				!   87  
	hole,				!   88  
	hole,				!   89
	item( "ll.int",1,0 ),		!   90  
	item( "ll.bool",1,0 ),		!   91  
	item( "ll.string",9,0 ),	!   92  
	item( "ll.char",1,0 ),		!   93  
	item( "ll.nil.string",0,0 ),	!   94  
	hole,				!   95  
	item( "eq.ib",0,0 ),		!   96  
	item( "eq.s",0,0 ),		!   97  
	hole,				!   98  
	item( "neq.ib",0,0 ),		!   99  
	item( "neq.s",0,0 ),		!  100  
	hole,				!  101  
	item( "lt.i",0,0 ),		!  102  
	item( "lt.s",0,0 ),		!  103  
	item( "le.i",0,0 ),		!  104  
	item( "le.s",0,0 ),		!  105  
	item( "gt.i",0,0 ),		!  106  
	item( "gt.s",0,0 ),		!  107  
	item( "ge.i",0,0 ),		!  108  
	item( "ge.s",0,0 ),		!  109  
	item( "plus",0,0 ),		!  110  
	item( "minus",0,0 ),		!  111  
	item( "rem",0,0 ),		!  112  
	item( "fplus",0,0 ),		!  113  
	item( "fminus",0,0 ),		!  114  
	item( "not.op",0,0 ),		!  115  
	item( "float1",0,0 ),		!  116  
	hole,				!  117  
	hole,				!  118  
	hole,				!  119  
	item( "erase.ib",0,0 ),		!  120  
	item( "erase.s",0,0 ),		!  121  
	hole,				!  122  
	item( "rev.ms",0,0 ),		!  123  
	item( "newLine",1,0 ),		!  124  
	item( "finish.op",0,0 ),	!  125  
	hole,				!  126  
	item( "read.op",1,0 ),		!  127  
	item( "fjump",2,0 ),		!  128  
	item( "bjump.l",2,0 ),		!  129  
	item( "jumpf",2,0 ),		!  130  
	item( "jumpff",2,0 ),		!  131  
	item( "jumptt",2,0 ),		!  132  
	item( "for.test",2,0 ),		!  133  
	item( "for.step.l",2,0 ),	!  134  
	item( "cjump.r",2,0 ),		!  135  
	item( "cjump.p",2,0 ),		!  136  
	hole,				!  137
	item( "bjumpt.l",2,0 ),		!  138
	hole,				!  139
	item( "local.l",2,0 ),		!  140  
	item( "plocal.l",2,0 ),		!  141  
	item( "dlocal.l",2,0 ),		!  142  
	item( "mst.local.l",2,0 ),	!  143  
	item( "global.l",2,0 ),		!  144  
	item( "pglobal.l",2,0 ),	!  145  
	item( "dglobal.l",2,0 ),	!  146  
	item( "mst.global.l",2,0 ),	!  147  
	hole,				!  148  
	hole,				!  149  
	hole,				!  150  
	hole,				!  151  
	item( "load.l",13,0 ),		!  152  
	item( "pload.l",13,0 ),		!  153  
	item( "dload.l",13,0 ),		!  154  
	item( "mst.load.l",13,0 ),	!  155  
	item( "local.ass.l",2,0 ),	!  156  
	item( "plocal.ass.l",2,0 ),	!  157  
	item( "dlocal.ass.l",2,0 ),	!  158  
	hole,				!  159  
	item( "global.ass.l",2,0 ),	!  160  
	item( "pglobal.ass.l",2,0 ),	!  161  
	item( "dglobal.ass.l",2,0 ),	!  162  
	hole,				!  163  
	hole,				!  164  
	hole,				!  165  
	hole,				!  166  
	hole,				!  167  
	item( "load.ass.l",13,0 ),	!  168  
	item( "pload.ass.l",13,0 ),	!  169  
	item( "dload.ass.l",13,0 ),	!  170  
	hole,				!  171  
	item( "apply.op.l",4,0 ),	!  172  
	hole,				!  173  
	hole,				!  174
	item( "return.r",6,0 ),		!  175  
	item( "return.p",6,0 ),		!  176  
	item( "return.v",6,0 ),		!  177  
	hole,				!  178
	item( "global.apply.l",2,0 ),	!  179
	item( "local.apply.l",2,0 ),	!  180
	item( "retract.ib",5,0 ),	!  181
	item( "retract.r",5,0 ),	!  182
	item( "retract.s",5,0 ),	!  183
	item( "retract.p",5,0 ),	!  184
	hole,				!  185  
	item( "retract.v",5,0 ),	!  186
	item( "mst.sfn",1,0 ),		!  187
	hole,				!  188
	hole,				!  189
	hole,				!  190
	item( "load.apply",13,0 ),	!  191
	hole,				!  192
	item( "isnt.op",0,0 ),		!  193  
	item( "subs.r",0,0 ),		!  194  
	item( "subs.p",0,0 ),		!  195  
	hole,				!  196  
	item( "subsass.r",0,0 ),	!  197  
	item( "subsass.p",0,0 ),	!  198  
	hole,				!  199  
	item( "makev.r",2,0 ),		!  200  
	item( "makev.p",2,0 ),		!  201  
	hole,				!  202  
	item( "iliffe.r",2,0 ),		!  203  
	item( "iliffe.p",2,0 ),		!  204  
	hole,				!  205  
	item( "subv.r",0,0 ),		!  206  
	item( "subv.p",0,0 ),		!  207  
	hole,				!  208  
	item( "subvass.r",0,0 ),	!  209  
	item( "subvass.p",0,0 ),	!  210  
	hole,				!  211  
	item( "lwb.op",0,0 ),		!  212  
	item( "substr.op",0,0 ),	!  213  
	item( "load.trademark.l",12,0 ),!  214  
	hole,				!  215  
	hole,				!  216  
	hole,				!  217
	item( "ll.int.l",4,0 ),		!  218  
	item( "ll.real",8,0 ),		!  219  
	item( "ll.string.l",2,0 ),	!  220  
	item( "ll.file",0,0 ),		!  221  
	item( "ll.nil.pntr",0,0 ),	!  222  
	hole,				!  223  
	item( "eq.r",0,0 ),		!  224  
	item( "eq.p",0,0 ),		!  225  
	hole,				!  226  
	item( "neq.r",0,0 ),		!  227  
	item( "neq.p",0,0 ),		!  228  
	hole,				!  229  
	item( "lt.r",0,0 ),		!  230  
	hole,				!  231  
	item( "le.r",0,0 ),		!  232  
	hole,				!  233  
	item( "gt.r",0,0 ),		!  234  
	hole,				!  235  
	item( "ge.r",0,0 ),		!  236  
	hole,				!  237  
	item( "times",0,0 ),		!  238  
	item( "div",0,0 ),		!  239  
	item( "neg",0,0 ),		!  240  
	item( "ftimes",0,0 ),		!  241  
	item( "fdivide",0,0 ),		!  242  
	item( "fneg",0,0 ),		!  243  
	item( "float2",0,0 ),		!  244  
	hole,				!  245  
	hole,				!  246  
	hole,				!  247  
	item( "erase.r",0,0 ),		!  248  
	item( "erase.p",0,0 ),		!  249  
	hole,				!  250  
	item( "rev.ps",0,0 ),		!  251  
	item( "newLine.l",2,0 ),	!  252  
	hole,				!  253
	hole,				!  254  
	item( "write.op",1,0 )		!  255  
	     ]

!----------------------------------------------------------------------------

let count := false
let limiting := false
let lower.lim := -1
let upper.lim := maxint

! get user options
procedure Set.options
begin

	procedure iconv( cstring s -> int)
	begin
	let n := 0
	for i = 1 to length (s) do
		{let d = s(i|1)
		 if digit (d) then n := n * 10 + (decode (d) - 48)
		 else
			{write "must give two +ve proc numbers for limits"
			 abort }
		}
	n
	end
	
	let i := o.lwb+1
	let opts = options
	while i <= upb (opts) do
	begin
		let x = opts(i)
		case x of
		"count","COUNT"   : count := true
		"nolist","NOLIST" : pp := false
		"limit","LIMIT": if i + 2 > upb (opts) then 
					{ write "'ngive limits on command'n"; abort }
				else
				begin
					lower.lim := iconv ( opts (i+1))
					upper.lim := iconv ( opts (i+2))
					write "listing from let",lower.lim," to ",upper.lim,"'n'n"
					i := i + 2
					limiting := true
					pp := false
				end
		default: {}
		i := i + 1
	end
end

Set.options



let fi = open ( options(o.lwb) , 0 ) ! 1
if fi = nullfile do {write "unable to open : ",options (o.lwb); abort}
write "'nFILE NAME = " ++ options( o.lwb ) ++ "'n'n"
seek( fi,-128,2 ) ! 128 bytes from end of file

let block.no := 32767

!----------------------------------------------------------------------------


! thus starts the loop over the blocks in the file
while block.no > 1 and block.no <= 32767 do
begin
	let opvec = OPvec
	let pn := 0
	let ptr := 0

	! file pointer is at end of last 128 unit in block
	for i = 1 to 128 - 24 by 2 do { let x = read.16( fi ) }

	let total = read.32 (fi)
	let start = read.32 (fi)
	let structaddr = read.32(fi)
	let flags = read.16(fi)
	let flsum = read.16(fi)
	let vsn = read.16(fi)
	let block.size = read.32(fi)
	block.no := read.16(fi)
	
	newLine( 2 )
	Right.j( "total code size",total,4 )
	Right.j( "start address",start,4 )
	Right.j( "structure address",structaddr,4 )
	Right.j( "flags",flags,2 )
	Right.j( "flowsum",flsum,2 )
	Right.j( "version number",vsn,2)
	Right.j( "block size",block.size,4 )
	Right.j( "block number",block.no,2 )
	newLine( 2 )
		
	Seek.back( fi,block.size )
	
	let n := 0
		
	procedure Read.byte( -> int ) ; { n := n + 1 ; read.byte( fi ) }
	
	procedure Read.word( -> int ) ; { n := n + 2 ; read.16( fi ) }
	
	procedure Read.long( -> int ) ; { n := n + 4 ; read.32( fi ) }
		
	procedure Read.name( cfile f -> string )
	begin
		procedure Reads( cfile f -> string )
		begin
			if eof( f ) then "" else
			{   let x := read( f )
			    let y := ""
			    repeat x := read( f ) while x ~= "'"" do y := y ++ x
	 		    y }
		end
	
		let name = Reads( f )
		n := n + length( name ) + 2
		name
	end
	
	procedure Write.lit.str
	begin

        	let rest = shift.l( Read.word,16 ) ; let size = Read.word

		if pp do { put.param( size,2 ); write " : '"" }
		for i = 1 to size do Wstr( Read.byte )
		if pp do write "'""
	end
	
	procedure Scan
	while n < structaddr do if end.proc then
	begin
	        end.proc := false
		if limiting do pp := (pn >= lower.lim) and (pn <= upper.lim)
		newLine( 3 )
		let struct.off := Read.word
	
		if b.and( struct.off,shift.l( 1,15 ) ) ~= 0 then
		begin
			struct.off := b.and( struct.off,shift.l( 1,15 ) - 1 )
			Right.j( "structure table offset",struct.off,2 )
			Right.j( "main stack depth",Read.word,2 )
		end 
		else Right.j( "main stack depth",struct.off,2 )
		Right.j( "pointer stack depth",Read.word,2 )
		newLine( 2 )
		pn := pn + 1
		put.titles := true ; if pp do write titles
	end
	else
	begin
		let loc := n
		let opcode = Read.byte
		let param := 0
		let itm = opvec( opcode )
		put( loc,opcode,itm( iname ) )
		if count do itm( icnt ) := itm( icnt ) + 1
		
		case itm( ikind ) of
		0	: {}
		1	: put.param( Read.byte,1 )
		2	: put.param( Read.word,2 )
		3	: { put.param( Read.byte,1 ) ; put.param( Read.byte,1 ) }
		4	: put.param( Read.long,4 )
		5	: { put.param( Read.word,2 ) ; put.param( Read.word,2 ) }
		6	: begin
				end.proc := true
				newLine( 2 )

			  end
		7	: begin
				put.param( Read.word,2 )
				put.param( Read.word,2 )
				put.param( Read.word,2 )
			  end
		8	: { put.param( Read.long,4 ) ; put.param( Read.long,4 ) }
		9	: Write.lit.str 
		10      : begin
				put.param( Read.byte,1 )
				put.param( Read.word,2 )
					put.param( Read.word,2 )
			  end
		11      : begin
				put.param( Read.byte,1 )
				put.param( Read.long,4 )
			  end
		12      : begin
				put.param( Read.word,2 )
				put.param( Read.word,4 )
			  end
	        default : begin
				put.param( Read.byte,1 )
				put.param( Read.word,2 )
			  end
	
		newLine( 1 )
	end
		
	Scan
	
	put.titles := false
	
	if n < total do
	begin
		Write ( "'nStructure table :'n" )
		let ns := 0
		while n < total do
		begin
			Write.word.dec ( ns ) ; Write ( " - " )
			Write.word.dec ( Read.word ) ; Write ( " - " )
			Write.word.dec ( Read.word )
			Line.feed ( 1 )
			ns := ns + 4
		end
		Line.feed ( 2 )
	end
	
	Line.feed( 2 )
	let name = Read.name( fi )
	Write( if name = "" then "Main block ( name null )" else
	           "block name = " ++ name )
	Line.feed( 2 )
	let ext.name := Read.name( fi )
	while ext.name ~= "" do
		{ Write( "'nExternal at    " ) ; Write.word.dec( Read.word ) ; Write( " : " ++ ext.name )
	          ext.name := Read.name( fi ) }
	Write( "'n'nEnd of Block'n'n" )
	if block.no > 1 and block.no < 32767 do
	begin
		while n rem 128 ~= 0 do { let x = read.byte(fi) ; n := n + 1 }
		Seek.back( fi, 128 + n )
	end
	
!----------------------------------------------------------------------------
	
	if count do	! let sorted static count of opcodes if needed
	begin
		write "'n'nStatic count of operators'n'n"
		let total := 0
	
		procedure Dis( *cpntr cn )
		begin
			!Treesort
			structure number( cpntr val ; pntr left,right )
			
			procedure enter( pntr head,temp -> pntr )
			if head = nil then temp else
			if temp( val,icnt ) < head( val,icnt )
			then { head( left ) := enter( head( left ),temp ) ; head }
			else { head( right ) := enter( head( right ),temp ) ; head }
			
			procedure print.tree( pntr node )
			if node ~= nil do
			begin
				print.tree( node( left ) )
				let itm = node( val )
				let n = itm( iname )
				let c = itm( icnt )
				total := total + c
				write n,c:(20 - length(n) ),(if c = 0 then " unused'n" else "'n")
				print.tree( node( right ) )
			end
			
			let head := nil
			for i = lwb( cn ) to upb( cn ) do
			head:= enter( head,number( cn( i ),nil,nil ) )
			print.tree( head )
		end
	
		Dis( opvec )
		write "'n'ntotal = ",total,"'n"
	end
end
