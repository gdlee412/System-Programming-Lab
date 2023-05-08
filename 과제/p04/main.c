#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	char filename[] = "Aladdin_num.txt";
	int STDIN, STDOUT;
	int line_num = 1;
	char num_to_string[50];
	char input_char;
	char line_number_format[] = " | ";

	if(argc != 2)
	{
		perror("required file name");
		exit(1);
	}

	if((STDIN = open(argv[1], O_RDONLY, 0755)) < 0 || (STDOUT = open(filename, O_RDWR | O_CREAT, 0755)) < 0)
	{
		perror("open");
		exit(1);
	}

	num_to_string[0] = '0' + line_num;
	write(STDOUT, num_to_string, 1);
	write(STDOUT, line_number_format, sizeof(line_number_format) - 1);
	line_num++;

	while(read(STDIN, &input_char, 1) != 0)
	{
		write(STDOUT, &input_char, 1);
		if(input_char == '\n')
		{
			int digit_count = 0;
			int temp_num = line_num;
			while(temp_num != 0)
			{
				temp_num /= 10;
				digit_count++;
			}

			temp_num = line_num;
			for(int i = digit_count - 1; i >= 0; i--)
			{
				num_to_string[i] = '0' + temp_num % 10;
				temp_num /= 10;
			}

			num_to_string[digit_count] = '\0';

			write(STDOUT, num_to_string, digit_count);
			write(STDOUT, line_number_format, sizeof(line_number_format) - 1);
			line_num++;
		}
	}
	
	if(close(STDIN) < 0 || close(STDOUT) < 0)
	{
		perror("close");
		exit(1);
	}

	exit(0);
}
