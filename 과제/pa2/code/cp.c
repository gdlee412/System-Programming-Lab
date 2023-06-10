#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int STDIN, STDOUT;
	char input_char;

	if(argc == 1)
	{
		fprintf(stderr, "cp: missing file operand\n");
		exit(1);
	}
	else if(argc == 2)
	{
		fprintf(stderr, "cp: missing destination file operand after '%s'\n", argv[1]);
		exit(1);
	}


	if((STDIN = open(argv[1], O_RDONLY)) < 0 || (STDOUT = open(argv[2], O_RDWR | O_CREAT, 0644)) < 0)
	{
		perror("cp");
		exit(1);
	}


	while(read(STDIN, &input_char, 1) != 0)
	{
		write(STDOUT, &input_char, 1);
	}
	
	if(close(STDIN) < 0 || close(STDOUT) < 0)
	{
		perror("cp");
		exit(1);
	}

	exit(0);
}
