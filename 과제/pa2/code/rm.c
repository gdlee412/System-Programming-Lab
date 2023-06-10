#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		fprintf(stderr, "rm: missing file operand\n");
		exit(1);
	}

	unlink(argv[1]);
	return 0;
}
