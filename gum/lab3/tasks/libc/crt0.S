@ crt0.S: Execution startup routine
@
@ Authors: Krish Chainani kchainan@andrew.cmu.edu
@          Akshay Gandhi akshayrg@andrew.cmu.edu
@	   Ara Macasaet lmacasae@andrew.cmu.edu
@ Date:    Oct 12 2013 12:35

	.file	"crt0.S"
	.text

	.global	_start

_start:
	STMFD	sp!, {r0-r12, lr}
	LDR 	r0, [sp] @ Puts argc to r0
	ADD     r1, sp, #4 @ Points r1 to argv[0]
	
	bl 	main @ Calls main
	b	exit @ Exits

