@ exit.S: exit syscall wrapper
@
@ Authors: Krish Chainani kchainan@andrew.cmu.edu
@          Akshay Gandhi akshayrg@andrew.cmu.edu
@	   Ara Macasaet lmacasae@andrew.cmu.edu
@ Date:    Oct 12 2013 16:35
#include <bits/swi.h>

	.file	"exit.S"
	.text

	.global	exit

exit:
	swi	EXIT_SWI @ Calls the EXIT_SWI swi handler

