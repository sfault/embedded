/*
 * kernel.c: Kernel main (entry) function
 *
 * Authors: Krish Chainani kchainan@andrew.cmu.edu
 *          Akshay Gandhi (akshayrg@andrew.cmu.edu)
 *	    Ara Macasaet (lmacasae@andrew.cmu.edu)
 * Date:    Oct 25, 2013 09:07
 */

#include <exports.h>
#include <bits/types.h>
#include <bits/fileno.h>
#include <bits/swi.h>
#include <bits/errno.h>

#define EOT 0x4
#define BACKSPACE 0x8
#define DELETE 127
#define NEWLINE 0xa
#define RETURN 0xd

//global variables
unsigned stack_p;
unsigned link_r;

//function prototypes:
extern void user_switch(int, char**);
extern void swi_assembly();
int install_handler (unsigned int *vector, unsigned int *instruction);
int restore (unsigned int *vector, unsigned int *instruction);
ssize_t write_syscall(int fd, void *buf, ssize_t count);
ssize_t read_syscall(int fd, void *buf, ssize_t count);
void exit(int status);
void printer(int a);
int C_SWI_handler(unsigned swi_num, unsigned *regs);
extern int exit_handler(int);
//global variables:
void *buf;
ssize_t count;

int main(int argc, char *argv[]) 
{
  install_handler((unsigned int *) 0x8, (unsigned int *) 0xa2fff000);
  puts("Handler installed");
  user_switch(argc, argv);
  puts("user mode entered");
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

        // check for argument errors
   	  if (fd != STDIN_FILENO)
                return -EBADF;
       /* if (!valid_addr(buffer, count, RAM_START_ADDR, RAM_END_ADDR))
                return -EFAULT;*/
                
        // reading procedure
        for (bufIndex = 0; bufIndex < count; ++bufIndex) {
                // read char from stdin
                tempChar = getc();
                // interpret special characters
                if (tempChar == EOT)
                        return bufIndex;
                if (tempChar == BACKSPACE || tempChar == DELETE) {
                        bufIndex--;
                        buf[bufIndex] = 0; 
                        bufIndex--;
                        puts("\b \b");
                        continue;
                }
                // store and display read character
                buf[bufIndex] = tempChar;
                putc(buf[bufIndex]);
                // interpret newline / carriage return characters
                if (buf[bufIndex] == NEWLINE || buf[bufIndex] == RETURN)
                {
                        puts("\r\n");
			buf[bufIndex] = '\n';
 //printf("Size: %u", bufIndex+1);

                        return bufIndex + 1;
                }
        }
	//printf("Size: %u", bufIndex);
        return bufIndex;
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
	//printf("Count: %u", count);
	//printf("Buf: %s", buf);         
        // check for argument errors
        if (fd != STDOUT_FILENO)
                return -EBADF;
       /* if (!valid_addr(buffer, count, RAM_START_ADDR, RAM_END_ADDR) &&
                !valid_addr(buffer, count, FLASH_START_ADDR, FLASH_END_ADDR))
                return -EFAULT;*/
        
        // output buf to stdout
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
void exit(int status)
{
  //puts("exit");
}


void printer(int a)
{
  printf("%d",a);
}

/*
 * 
 * C_SWI_handler checks the SWI number passed and calls the equivalent system call
 * 
 */


int C_SWI_handler(unsigned swi_num, unsigned *regs)
{ printf("Swi number: %d", swi_num);
  switch(swi_num) 
  {
    case EXIT_SWI: //SWI 0x90000001
      printf("Entered EXIT_SWITCH");
      return exit_handler(*(regs)); //change the parameter

    case READ_SWI: //SWI 0x90000003
	//printf("Read r1 %s", *(regs+1));
      return read_syscall(STDIN_FILENO,(void *)*(regs+1),*(regs+2)); //passes a buffer to put the user input to and a size to the system call read_syscall
      
    case WRITE_SWI: //SWI 0x90000004
	//printf("Write r1 %s",*(regs+1));
      write_syscall(STDOUT_FILENO,(void *)*(regs+1),*(regs+2)); //passes a buffer to print out and a size to the stystem call write_syscall
     //printf("Write r1 %s",*(regs+1));
	return 1; // change return 
    default: //invalid SWI number
      puts("\nInvalid SWI number\n");
      return (0x0badc0de);// #define
  }
}
