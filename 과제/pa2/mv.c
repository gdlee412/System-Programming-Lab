#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
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
	
	if(rename(argv[1], argv[2]))
	{
		perror("mv");
		exit(errno);
	}

	exit(0);
}
