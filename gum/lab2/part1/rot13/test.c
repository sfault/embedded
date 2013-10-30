#include <errno.h>
#include <bits/fileno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
char input_string[200];
read (0,input_string,20);
write(1,input_string,20);
exit(5);
return 0;
}

