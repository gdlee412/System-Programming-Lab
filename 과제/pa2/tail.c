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
	int total_line_count = 0;
	char c;
	int K = 10;

	if(argc < 1 && argc > 4)
	{
		fprintf(stderr, "tail: Operation not permitted\n");
		exit(1);
	}
	else if(argc == 4)
	{
		if(strncmp(argv[1], "-n", 2) != 0)
		{
			fprintf(stderr, "tail: Operation not permitted\n");
			exit(1);
		}
		if((fd = open(argv[3], O_RDONLY)) < 0)
		{
			perror("tail");
			exit(1);
		}
		K = atoi(argv[2]);
	}
	else if(argc == 2)
	{
		 if((fd = open(argv[1], O_RDONLY)) < 0)
                {
                        perror("tail");
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
		while(char_count != size)
		{
			scanf("%c", &c);
			char_count++;
			if(c == '\n')
			{
				total_line_count++;
			}
		}
		rewind(stdin);

		char_count = 0;
		while(char_count != size && line_num != total_line_count)
		{
			scanf("%c", &c);
			char_count++;
			if(line_num >= total_line_count - K)
			{
				printf("%c", c);
			}
			if(c == '\n')
			{
				line_num++;
			}
		}
		exit(0);

	}

	while(read(fd, &c, 1) != 0)
	{
		if(c == '\n')
		{
			total_line_count++;
		}
	}

	lseek(fd, 0, SEEK_SET);

	while(read(fd, &c, 1) != 0)
	{
		if(line_num >= total_line_count - K)
		{
			write(STDOUT_FILENO, &c, 1);
		}

		if(c == '\n')
		{
			line_num++;
		}
	}

	if(close(fd) < 0)
	{
		perror("tail");
		exit(1);
	}

	return 0;
}
