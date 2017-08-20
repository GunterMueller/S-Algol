while ~eof do
begin
	let s = read.a.line( s.i )
	case true of
	s = ""  : {}
	s = "	" : write "'n"
	s( 1|1 ) = "#" : {}
	default     : write s,"'n"
end
?
