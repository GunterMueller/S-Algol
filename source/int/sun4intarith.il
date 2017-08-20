	.inline	_ILintAdd,2		! inline code for integer +
	addcc	%o0,%o1,%o0		! includes overflow trap
	tvs	0x2
	nop
	.end

	.inline	_ILintTimes,2		! inline code for integer *
	call	.mul,2			! includes overflow trap
	nop
	tnz	0x2
	nop
	.end

	.inline	_ILintMinus,2		! inline code for integer -
	subcc	%o0,%o1,%o0		! includes overflow trap
	tvs	0x2
	nop
	.end
