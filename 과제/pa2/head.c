#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int fd;
	int line_num = 0;
	char c;
	int K = 10;

	if(argc < 1 && argc > 4)
	{
		fprintf(stderr, "head: Operation not permitted\n");
		exit(1);
	}
	else if(argc == 4)
	{
		if(strncmp(argv[1], "-n", 2) != 0)
		{
			fprintf(stderr, "head: Operation not permitted\n");
			exit(1);
		}

		if((fd = open(argv[3], O_RDONLY)) < 0)
		{
			perror("head");
			exit(1);
		}
		K = atoi(argv[2]);
	}
	else if(argc == 2)
	{
		 if((fd = open(argv[1], O_RDONLY)) < 0)
                {
                        perror("head");
                        exit(1);
                }
	}
	else
	{
		fseek(stdin, 0, SEEK_END);
		int size = ftell(stdin);
		rewind(stdin);

		if(argc == 3)
		{
			K = atoi(argv[2]);
		}
		int char_count = 0;
		while(K != line_num && char_count != size)
		{
			scanf("%c", &c);
			printf("%c", c);
			char_count++;
			if(c == '\n')
			{
				line_num++;
			}
		}
		exit(0);
	}

	while(read(fd, &c, 1) != 0)
	{
		int nbytes;
		if((nbytes = write(STDOUT_FILENO, &c, 1)) < 0)
		{
			perror("head");
			exit(1);
		}
		if(c == '\n')
		{
			line_num++;
		}

		if(line_num == K)
		{
			break;
		}
	}
	if(close(fd) < 0)
	{
		perror("head");
		exit(1);
	}

	return 0;
}
