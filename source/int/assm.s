	.globl	_memcopy
_memcopy:
	.word	0x3F		/* regs 0-5 inc */
	ashl	$2,12(ap),r0	/* make size into bytes */
	movq	4(ap),r1	/* r1 = source, r2 = dest */
	movc3	r0,(r1),(r2)	/* move heap item */
	ret
