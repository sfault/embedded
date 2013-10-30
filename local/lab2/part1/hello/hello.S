@ hello.S: Hello world! in assembly
@
@ Authors: Krish Chainani kchainan@andrew.cmu.edu
@          Akshay Gandhi akshayrg@andrew.cmu.edu
@	   Ara Macasaet lmacasae@andrew.cmu.edu
@ Date:    Oct 12 2013 13:43

#include <bits/swi.h>
#include <bits/fileno.h>
	
	.file	"hello.S"
	.text
	.align 2

	.global	_start

_start:

	ldr	r1, =hello_str @ Loads the Hello World! string in r1
	mov	r2, #hello_size @ Stores the size of the string in r1
	mov	r0, #STDOUT_FILENO @ Stores the file descriptor in r0
	swi	WRITE_SWI @ Calls the WRITE_SWI swi handler

	mov	r0, #4 @ Exits with status 4
	swi	EXIT_SWI @ Calls the EXIT_SWI handler

hello_str:               
        .ascii  "Hello world!\n" @ The string Hello world!
        .set    hello_size, .-hello_str @ Gets the size of the string

