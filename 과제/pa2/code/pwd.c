#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char cwd[200];
	
	if(argc != 1)
	{
		fprintf(stderr, "pwd: too many arguments\n");
	}

	getcwd(cwd, sizeof(cwd));
	printf("%s\n", cwd);

	return 0;
}
