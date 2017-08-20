	.globl _memcopy
|	copy limit is 64K
_memcopy:
	movl	a7,a0
	movl	a2,a7@-
	movl	a0@(4),a1
	movl	a0@(8),a2
	movl	a0@(12),d0
	cmpl	a1,a2
	bcc	down
	subqw	#1,d0
loop:
	movl	a1@+,a2@+
	dbra	d0,loop
	movl	a7@+,a2
	rts
down:
	movl	d0,d1
	asll	#2,d1
	addl	#4,d1
	addl	d1,a1
	addl	d1,a2
	subqw	#1,d0
loop2:
	movl	a1@-,a2@-
	dbra	d0,loop
	movl	a7@+,a2
	rts

