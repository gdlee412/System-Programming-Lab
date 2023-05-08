#include "strings.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

// function to find one word
void case1_find(char* filename, char* keyword)
{
	int fd = open(filename, O_RDONLY, 0755);
	int linenum = 1;
	int index = 0;
	int length = stringLength(keyword);
	char buffer[100];
	char c;

	while(1)
	{
		int eof = 0;
		int i = 0;
		while((eof = read(fd, &c, 1)) != 0)
		{
			if((c > '9' && c < 'A') || (c > 'Z' && c < 'a') || (c > 'z') || (c < '0'))
			{
				index++;
				if(c == 39 || (c == '!' || c == '?' || c == '.'))
				{
					buffer[i] = c;
					i++;
					continue;
				}
				break;
			}
			else if(c == '\n')
			{
				break;
			}
			buffer[i] = c;
			i++;
			index++;
		}
		buffer[i] = '\0';
		
		if(stringCompare(buffer, keyword) == 0)
		{
			int digit_count = 0;
			int temp_num = linenum;
			while(temp_num != 0)
			{
				temp_num /= 10;
				digit_count++;
			}
			char num_to_string[digit_count + 1];

			temp_num = linenum;
			for(int i = digit_count - 1; i >= 0; i--)
			{
				num_to_string[i] = '0' + temp_num % 10;
				temp_num /= 10;
			}
			num_to_string[digit_count] = '\0';

			write(STDOUT_FILENO, num_to_string, digit_count);
				
			buffer[0] = ':';
			write(STDOUT_FILENO, buffer, 1);

			digit_count = 0;
			temp_num = index - length - 1;
				
			while(temp_num != 0)
			{
				temp_num /= 10;
				digit_count++;
			}
			if(digit_count == 0)
			{
				digit_count++;
			}
			char index_to_string[digit_count + 1];

			temp_num = index - length - 1;

			for(int i = digit_count - 1; i >= 0; i--)
			{
				index_to_string[i] = '0' + temp_num % 10;
				temp_num /= 10;
			}

			index_to_string[digit_count] = '\0';		
			write(STDOUT_FILENO, index_to_string, digit_count);
			
		
			if(eof != 0)
			{
				buffer[0] = ' ';
				write(STDOUT_FILENO, buffer, 1);
			}
		}

		if(c == '\n')
		{
			index = 0;
			linenum++;
		}

		if(eof == 0)
		{
			buffer[0] = '\n';
			write(STDOUT_FILENO, buffer, 1);
			break;
		}
		
	}
	close(fd);
}

// function to find multiple words
void case2_find(char* filename, char* keyword)
{
	int fd = open(filename, O_RDONLY, 0755);
	int linenum = 1;
	char buffer[100];
	char c;
	char* words[10];
	int word_count = 0, i = 0, j = 0;
	int existing_words = 0, word_exist[10] = {0};

	for(i = 0; i < 10; i++)
	{
		words[i] = malloc(stringLength(keyword) + 1);
	}

	i = 0;
	while(keyword[i] != '\0')
	{
		if(keyword[i] == ' ')
		{
			words[word_count][j] = '\0';
			word_count++;
			j = 0;
			i++;
			continue;
		}
		words[word_count][j] = keyword[i];
		j++;
		i++;
	}
	words[word_count][j] = '\0';
	word_count++;
	
	while(1)
	{
		int eof = 0;
		i = 0;
		while((eof = read(fd, &c, 1)) != 0)
		{
			if((c > '9' && c < 'A') || (c > 'Z' && c < 'a') || (c > 'z') || (c < '0'))
			{
				if(c == 39)
				{
					buffer[i] = c;
					i++;
					continue;
				}
				break;
			}
			else if(c == '\n')
			{
				break;
			}
			buffer[i] = c;
			i++;
		}
		buffer[i] = '\0';
			
		for(i = 0; i < word_count; i++)
		{			
			if(word_exist[i] == 0 && stringCompare(words[i], buffer) == 0)
			{
				existing_words++;
				word_exist[i] = 1;
				break;
			}
		}
		
		if(existing_words == word_count)
		{
			int digit_count = 0;
			int temp_num = linenum;
			while(temp_num != 0)
			{
				temp_num /= 10;
				digit_count++;
			}
			char num_to_string[digit_count + 1];

			temp_num = linenum;
			for(int i = digit_count - 1; i >= 0; i--)
			{
				num_to_string[i] = '0' + temp_num % 10;
				temp_num /= 10;
			}
			num_to_string[digit_count] = '\0';

			write(STDOUT_FILENO, num_to_string, digit_count);
			
			if(eof != 0)
			{
				buffer[0] = ' ';
				write(STDOUT_FILENO, buffer, 1);
			}
			existing_words = 0;
			
			if(c != '\n')
			{
				while(read(fd, &c, 1) != 0)
				{
					if(c == '\n')
					{
						break;
					}
				}
			}

		}
		
		if(c == '\n')
		{
			linenum++;
			existing_words = 0;
			for(i = 0; i < word_count; i++)
			{
				word_exist[i] = 0;
			}
		}

		if(eof == 0)
		{
			buffer[0] = '\n';
			write(STDOUT_FILENO, buffer, 1);
			break;
		}
		
	}
	close(fd);
}
// function to find phrase
void case3_find(char* filename, char* keyword)
{
	int fd = open(filename, O_RDONLY, 0755);
	int linenum = 1;
	int length = stringLength(keyword) - 2;
	int i, buff_len;
	char c;
	char buffer[100];

	for(i = 0; keyword[i] != '\0'; i++)
	{
		keyword[i] = keyword[i + 1];
	}	
	keyword[i - 2] = '\0';
		
	while(1)
	{
		int eof;
		i = 0;
		while((eof = read(fd, &c, 1)) != 0)
		{
			if(c == '\n')
			{
				break;
			}
			buffer[i] = c;
			i++;		
		}
		buffer[i] = '\0';
		buff_len = i;	
		
		for(i = 0; buffer[i] != '\0'; i++)
		{
			if(i + length >= buff_len)
			{
				break;
			}
			char temp = buffer[i + length];
			buffer[i + length] = '\0';
			
			if(stringCompare(buffer + i, keyword) == 0)
			{
				if((temp >= 'A' && temp <= 'Z') || (temp >= 'a' && temp <= 'z'))
				{
					buffer[i + length] = temp;
					continue;
				}	
				
				if(i > 0)
				{
					if((buffer[i - 1] >= 'A' && buffer[i - 1] <= 'Z') || (buffer[i - 1] >= 'a' && buffer[i - 1] <= 'z'))
					{
						buffer[i + length] = temp;
						continue;
					}
				}

				int digit_count = 0;
				int temp_num = linenum;
				while(temp_num != 0)
				{
					temp_num /= 10;
					digit_count++;
				}
				char num_to_string[digit_count + 1];

				temp_num = linenum;
				for(int j = digit_count - 1; j >= 0; j--)
				{
					num_to_string[j] = '0' + temp_num % 10;
					temp_num /= 10;
				}
				num_to_string[digit_count] = '\0';

				write(STDOUT_FILENO, num_to_string, digit_count);
				
				buffer[0] = ':';
				write(STDOUT_FILENO, buffer, 1);

				digit_count = 0;
				temp_num = i;
					
				while(temp_num != 0)
				{
					temp_num /= 10;
					digit_count++;
				}
				if(digit_count == 0)
				{
					digit_count++;
				}
				char index_to_string[digit_count + 1];
	
				temp_num = i;

				for(int j = digit_count - 1; j >= 0; j--)
				{
					index_to_string[j] = '0' + temp_num % 10;
					temp_num /= 10;
				}

				index_to_string[digit_count] = '\0';		
				write(STDOUT_FILENO, index_to_string, digit_count);
			
		
				if(eof != 0 && i + length < buff_len)
				{
					buffer[0] = ' ';
					write(STDOUT_FILENO, buffer, 1);
				}

			}
			buffer[i + length] = temp;
		}

		if(c == '\n')
		{
			linenum++;
		}

		if(eof == 0)
		{
			buffer[0] = '\n';
			write(STDOUT_FILENO, buffer, 1);
			break;
		}
		
	}


	close(fd);
}

// function to find one word
void case4_find(char* filename, char* keyword)
{
	int fd = open(filename, O_RDONLY, 0755);
	int linenum = 1;
	int i, j = 0, k = 0, buff_len;
	int length;
	char c;
	char* words[2];
	int words_len[2];
	char buffer[100];
	char buffer2[100];

	for(i = 0; i < 2; i++)
	{
		words[i] = malloc(stringLength(keyword) + 1);
	}

	i = 0;
	while(keyword[i] != '\0')
	{
		if(keyword[i] == '*')
		{
			words[k][j] = '\0';
			words_len[k] = stringLength(words[k]);
			k++;
			j = 0;
			i++;
			continue;
		}
		words[k][j] = keyword[i];
		j++;
		i++;
	}
	words[k][j] = '\0';
	words_len[k] = stringLength(words[k]);
	length = words_len[0] + words_len[1];

	while(1)
	{
		int eof;
		i = 0;
		while((eof = read(fd, &c, 1)) != 0)
		{
			if(c == '\n')
			{
				break;
			}
			buffer[i] = c;
			i++;		
		}
		buffer[i] = '\0';
		buff_len = i;	
			
		for(i = 0; buffer[i] != '\0'; i++)
		{
			if(i + length >= buff_len)
			{
				break;
			}
			char temp = buffer[i + words_len[0]];
			
			if(i > 0 && ((buffer[i - 1] >= 'A' && buffer[i - 1] <= 'Z') || (buffer[i - 1] >= 'a' && buffer[i - 1] <= 'z')))
			{
				continue;
			}

			if((temp >= 'A' && temp <= 'Z') || (temp >= 'a' && temp <= 'z'))
			{
				continue;
			}
			buffer[i + words_len[0]] = '\0';

			if(stringCompare(buffer + i, words[0]) == 0)
			{
				buffer[i + words_len[0]] = temp;
				k = 0;
				for(j = i + words_len[0]; buffer[j] != '\0'; j++)
				{
					buffer2[k] = buffer[j];
					k++;
				}
				buff_len = k;
				for(j = 0; buffer2[j] != '\0'; j++)
				{
					if(j + words_len[1] >= buff_len)
					{
						break;
					}
					char token = buffer2[j + words_len[1]];

					if(j > 0 && ((buffer2[j - 1] >= 'A' && buffer2[j - 1] <= 'Z') || (buffer2[j - 1] >= 'a' && buffer2[j - 1] <= 'z')))
					{
						continue;
					}

					if(j < buff_len && ((token >= 'A' && token <= 'Z') || (token >= 'a' && token <= 'z')))
					{
						continue;
					}
 
					buffer2[j + words_len[1]] = '\0';
					if(stringCompare(buffer2 + j, words[1]) == 0)
					{	
						int success = 0;
						
						if(j < 3)
						{
							continue;
						}

						for(k = i + words_len[0]; k < j; k++)
						{
							if((buffer2[k] >= 'A' && buffer2[k] <= 'Z') || (buffer2[k] >= 'a' && buffer2[k] <= 'z') || (buffer2[k] >= '0' && buffer2[k] <= '9'))
							{
								success = 1;
								break;
							}
						}

						if(success = 1)
						{		
							int digit_count = 0;
							int temp_num = linenum;
							while(temp_num != 0)
							{
								temp_num /= 10;
								digit_count++;
							}
							char num_to_string[digit_count + 1];

							temp_num = linenum;
							for(int j = digit_count - 1; j >= 0; j--)
							{
								num_to_string[j] = '0' + temp_num % 10;
								temp_num /= 10;
							}
							num_to_string[digit_count] = '\0';

							write(STDOUT_FILENO, num_to_string, digit_count);
							
							if(eof != 0)
							{
								buffer[0] = ' ';
								write(STDOUT_FILENO, buffer, 1);
								buffer[i + 1] = '\0';
								buffer[j + 1] = '\0';
							}
							break;
						}
					}
					buffer2[j + words_len[1]] = token;
				}

			}
			buffer[i + words_len[0]] = temp;
		}

		if(c == '\n')
		{
			linenum++;
		}

		if(eof == 0)
		{
			buffer[0] = '\n';
			write(STDOUT_FILENO, buffer, 1);
			break;
		}
		
	}

	close(fd);
}

// filters into the 4 different case functions
void case_filter(int casenum, char* filename, char* keyword)
{
	switch(casenum)
	{
		case 1: 
			case1_find(filename, keyword);
			break;
		case 2:
			case2_find(filename, keyword);
			break;
		case 3: 
			case3_find(filename, keyword);
			break;
		case 4: 
			case4_find(filename, keyword);
			break;
	}
}

