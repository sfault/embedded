/*
 * rot13.c: ROT13 cipher test application
 *
 * Authors: Krish Chainani kchainan@andrew.cmu.edu
 *          Akshay Gandhi (akshayrg@andrew.cmu.edu)
 *	    Ara Macasaet (lmacasae@andrew.cmu.edu)
 * Date:    Oct 12, 2013 20:01
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/fileno.h>

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
	int * t = (int *)*argv;
	
	int arg_count = 0;
	for(arg_count=0 ; arg_count<argc ; arg_count++)
	{
		write(STDOUT_FILENO,(void *)*(t+arg_count), 8);
	}
	char *input_string = argv[0];
	int string_length = 0, read_length = 0, write_length = 0;
	char last_checker='\0'; // Last character checker
	while(1) // Infinite loop to exit when buffer is empty or an error was encountered
	{
		string_length=0;
		read_length = read(STDIN_FILENO, input_string, BUF_SIZE); // Assigns the return value of read to read_length
	   	if(read_length < 0) // If an error was encountered, returns 1
	    		return 1;
	   	else
	   	{
	     		write_length = read_length;
                	if ((*input_string == '\n' && last_checker == '\n') || read_length==1) // If only '\n' (0 bytes) was read, returns 0
                		return 0;
	     		while(write_length > 0){ // Traverses the string
				last_checker=*input_string;
					/*If character is between A(a) and M(m) inclusive, it will be converted to the 13th character after it,
						if character is between N(n) and Z(z) inclusive, it will be converted to the 13th character before it*/
	       			if((*input_string >= 65 && *input_string <= 77) || (*input_string >= 97 && *input_string <= 109))
					*input_string+=13;
				else if((*input_string >= 78 && *input_string <= 90) || (*input_string >= 110 && *input_string <= 122))
					*input_string-=13;	
				//counters:
				input_string++;
	      			write_length--;
	     		}
			if(write(STDOUT_FILENO, input_string-read_length, read_length) == -1) // If write returns an error, exits and returns 1
				return 1;   
		}
	}
	return 0;
}

