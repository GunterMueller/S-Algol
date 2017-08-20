	xdef memcopy
	;	copy limit is 64K
memcopy:
	move.l a7,a0
	movem.l a2,-(a7)
	move.l 4(a0),a1
	move.l 8(a0),a2
	move.l 12(a0),d0
	cmp.l a1,a2
	bcc down
	subq.w #1,d0
loop:
	move.l (a1)+,(a2)+
	dbra d0,loop
	movem.l (a7)+,a2
	rts
down:
	move.l d0,d1
	asl.l #2,d1
	add.l #4,d1
	add.l d1,a1
	add.l d1,a2
	subq.w #1,d0
loop2:
	move.l -(a1),-(a2)
	dbra d0,loop
	movem.l (a7)+,a2
	rts

