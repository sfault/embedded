@ crt0.S: Execution startup routine
@
@ Authors: Group Member 1 <email address>
@          Group Member 2 <email address>
@ Date:    The current time & date

	.file	"crt0.S"
	.text

	.global	_start

_start:
	LDR r0, [sp] @ Puts argc to r0
	ADD r1, sp, #4 @ Points r1 to argv[0]
	
	bl 	main @ Calls main
	b	exit @ Exits

