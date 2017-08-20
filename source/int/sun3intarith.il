	.inline	_ILintAdd,8		| inline code for integer +
	movl	sp@+,d0			| includes overflow trapping
	addl	sp@+,d0
	trapv
	.end

	.inline	_ILintTimes,8		| inline code for integer *
	movl	sp@+,d0			| includes overflow trapping
	mulsl	sp@+,d0
	trapv
	.end

	.inline	_ILintMinus,8		| inline code for integer -
	movl	sp@+,d0			| includes overflow trapping
	subl	sp@+,d0
	trapv
	.end
