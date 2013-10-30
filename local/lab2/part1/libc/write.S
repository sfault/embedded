@ write.S: write sycall wrapper
@
@ Authors: Krish Chainani kchainan@andrew.cmu.edu
@          Akshay Gandhi akshayrg@andrew.cmu.edu
@	   Ara Macasaet lmacasae@andrew.cmu.edu
@ Date:    Oct 12 2013 17:38

#include <bits/swi.h> 
	
	.file	"write.S"
	.text
	.global	write

write:
	swi	WRITE_SWI @ Calls the WRITE_SWI swi handler

	CMP	r0, #0 @ Compares r0 with zero to check for negative value
	movge	pc, lr @ If positive or zero, returns

		@ If negative, will do the following
	RSB	r0, r0, #0 @ Subtracts r0 from 0 to get the negative equivalent
	LDR 	r3, =errno @ Loads the address of the global variable errno in errno.c to r3
	STR	r0, [r3] @ Sets the global variable errno to the contents of r0
	MVN	r0, #0 @ Stores -1 in r0
	mov	pc, lr @ Returns
