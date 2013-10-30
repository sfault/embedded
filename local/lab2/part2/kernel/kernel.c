/*
 * errno.c: Defines all the kernel functions
 *
 * Author: Krish Chainani <kchainan>
 *		   Akshay Gandhi <akshayrg>
 *		   Ara Macasaet <lmacase>
 * Date:   Fri, 25 Oct 2013 01:31:00 -0400
 */
 
#include <exports.h>
#include <bits/types.h>
#include <bits/fileno.h>
#include <bits/swi.h>
#include <bits/errno.h>

//definitions
#define EOT 0x4
#define BACKSPACE 0x8
#define DELETE 127
#define NEWLINE 0xa
#define RETURN 0xd
#define START_ADDR 0xa2000000 
#define END_ADDR 0xa3ededf0

//global variables for the stack pointer, link pointer, buffer and buffer size
unsigned stack_p;
unsigned link_r;
void *buf;
ssize_t count;

//function prototypes:
int install_handler (unsigned int *vector, unsigned int *instruction);
int restore (unsigned int *vector, unsigned int *instruction);
ssize_t write_syscall(int fd, void *buf, ssize_t count);
ssize_t read_syscall(int fd, void *buf, ssize_t count);
void exit(int status);
void printer(int a);
int C_SWI_handler(unsigned swi_num, unsigned *regs);

//external assembly functions:
extern void user_switch(int, char**);
extern void swi_assembly();
extern int exit_handler(int);

int main(int argc, char *argv[]) 
{
	install_handler((unsigned int *) 0x8, (unsigned int *) 0xa2fff000);
	user_switch(argc, argv);
	restore((unsigned int *) 0x8, (unsigned int *) 0xa2fff000);
	return 0;
}

/*
* install_handler wires in our own swi_handler by changing the first two addresses of U-boot's SWI_Handler
* to point to our own swi_handler
* 
*/

int install_handler (unsigned int *vector, unsigned int *instruction)
{
	unsigned int *jump_entry;
	jump_entry = (unsigned int *) 0x5c0009c0; //the first instruction
	instruction[0] = *jump_entry; //assigns 0x5c0009c0 to the first instruction of U-boot's SWI_handler

	*jump_entry = 0xe51ff004; //the second instruction
	jump_entry++;
	instruction[1] = *jump_entry; //assigns 0xe51ff004 to the second instruction of U-boot's SWI_handler

	*jump_entry = (unsigned int)swi_assembly; //calls assembly function swi_assembly

	return 0;
}

/*
* 
* restore restores the original instructions of the U-boot SWI handler by reversing what we did at install_handler
* 
*/

int restore(unsigned int *vector, unsigned int *instruction)
{
	unsigned int *jump_entry;

	jump_entry = (unsigned int *) 0x5c0009c0; //the first instruction
	*jump_entry = instruction[0]; //restores the original first instruction of U-boot's SWI_handler

	jump_entry++;
	*jump_entry =  instruction[1]; //restores the original second instruction of U-boot's SWI_handler

	return 0;
}

/*
* 
* read_syscall is for the read system call. It waits for user input character by character and displays the character right after input.
* 
*/

ssize_t read_syscall(int fd, void *buffer, ssize_t count)
{
	unsigned int bufIndex;
	char tempChar;
	char * buf = (char *)buffer;

    // if fd is not 0
	if (fd != STDIN_FILENO)
		return -EBADF;

	//if within the ram space
    if ((int)buffer < START_ADDR || (int) buffer+count > END_ADDR)
            return -EFAULT;

    // reads one character at a time until the buffer size is reached or a new line or EOT was entered
	for (bufIndex = 0; bufIndex < count; ++bufIndex)
	{
		tempChar = getc(); //reads user input

		if (tempChar == EOT) //if end of transmission
			return bufIndex;

		if (tempChar == BACKSPACE || tempChar == DELETE) //if backspace or delete, last entered char will be removed from buffer
		{
			bufIndex--;
			buf[bufIndex] = 0; 
			bufIndex--;
			puts("\b \b"); //put \b \b on prompt (backspace)
			continue;
		}

		buf[bufIndex] = tempChar;
		putc(buf[bufIndex]); //displays character

		if (buf[bufIndex] == NEWLINE || buf[bufIndex] == RETURN) //if newline or carriage return
		{
			puts("\r\n");
			buf[bufIndex] = '\n';
			return bufIndex + 1;
		}
	}
	return bufIndex; //returns number of correctly read characters
}

/*
* 
* write_syscall is for the write system call. It outputs the contents of the buffer up to the nth character where n = buffer size. If the count passed is greater
* than the actual buffer size, it will only print up to the last character in the buffer.
* 
*/

ssize_t write_syscall(int fd, void *buffer, ssize_t count)
{
	unsigned int bufIndex;
	char *buf = (char *) buffer;

	if (fd != STDOUT_FILENO) //if fd is not 1
		return -EBADF;

	if ((int)buffer < START_ADDR || (int)buffer+count > END_ADDR) //if outside the range
            return -EFAULT;

	// output buffer to stdout until count or \0 is reached
	for (bufIndex = 0; bufIndex < count; bufIndex++)
	{
		if(buf[bufIndex] == '\0')
		{
			puts("\r\n");
			return count;
		}
			putc(buf[bufIndex]);
	}
		puts("\r\n");
		return count;
}

/*
* 
* C_SWI_handler checks the SWI number passed and calls the equivalent system call
* 
*/

int C_SWI_handler(unsigned swi_num, unsigned *regs)
{ 
	switch(swi_num) 
	{
		case EXIT_SWI: return exit_handler(*(regs)); //change the parameter

		case READ_SWI: return read_syscall(STDIN_FILENO,(void *)*(regs+1),*(regs+2)); //passes a buffer to put the user input to and a size to the system call read_syscall
						  
		case WRITE_SWI: return write_syscall(STDOUT_FILENO,(void *)*(regs+1),*(regs+2)); //passes a buffer to print out and a size to the stystem call write_syscall
		default: puts("\nInvalid SWI number\n");
		  			return (0x0badc0de);// #define
	}
}
