
int main(int argc, char **argv)
{
char buf[50];
read(STDIN_FILENO,buf,50);
write(STDOUT_FILENO,buf,50);
write(STDOUT,argv[0],argc);
exit(42);
}
