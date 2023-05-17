#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
	int STDIN;
	char input_char;
	
	if(argc == 2)
	{	
		if((STDIN = open(argv[1], O_RDONLY)) < 0)
		{
			perror("cat");
			exit(1);
		}
	}
	else if(argc == 1)
	{
		fseek(stdin, 0, SEEK_END);
		int size = ftell(stdin);
		rewind(stdin);
			
		int count = 0;
		while(count != size)
		{
			scanf("%c", &input_char);
			printf("%c", input_char);
			count++;
		}
		exit(0);
	}

	while(read(STDIN, &input_char, 1) != 0)
	{
		printf("%c", input_char);
	}
	
	if(close(STDIN) < 0)
	{
		perror("cat");
		exit(1);
	}

	exit(0);
}
