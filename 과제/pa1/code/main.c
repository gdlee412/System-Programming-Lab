#include "strings.h"
#include "find.h"

#include <unistd.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char* argv[]) 
{
	int fd;

	if(argc != 2)
	{
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY, 0755)) < 0)
	{
		exit(1);
	}
	if(close(fd) < 0)
	{
		exit(1);
	}
	
	while (1)
	{
		int i = 0;
		char c;
		char *keyword;
		while(read(STDIN_FILENO, &c, 1) != 0 && c != '\n')
		{
        		keyword[i] = c;
			i++;
		}
		keyword[i] = '\0';
		
		if(stringCompare(keyword, "PA1EXIT") == 0)
		{
			break;
		}
			
		int casenum = caseIdentifier(keyword);
	        
		case_filter(casenum, argv[1], keyword);	
	}


	return 0;
}
