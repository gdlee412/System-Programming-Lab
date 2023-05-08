#include "find.h"
#include <stdlib.h>

// if upper case, change to lower
char* toLower(char* destination, char* string)
{
	int i = 0;
	while(string[i] != '\0')
	{
		if(string[i] >= 'A' && string[i] <= 'Z')
		{
			destination[i] = string[i] + 32;
		}
		else
		{
			destination[i] = string[i];
		}	
		i++;
	}
	destination[i] = '\0';
}


// length of string without NULL ('\0')
int stringLength(char* string)
{
	int length = 0;
	while(string[length] != '\0')
	{
		length++;
	}
	return length;
}


// return 0 if the same and 1 if not
// to keep it similar to strncmp function
int stringCompare(char* string1, char* string2)
{
	int unequalflag = 0;
	int i = 0;
	char* low_string1 = malloc(stringLength(string1) + 1);
	char* low_string2 = malloc(stringLength(string2) + 1);

	toLower(low_string1, string1);
	toLower(low_string2, string2);

	while(low_string1[i] != '\0' && low_string2[i] != '\0')
	{		
		// case 1: current char is the same
		if(low_string1[i] == low_string2[i])
		{
			//continue
			i++;
		}
		// case 2: strings do not have the same char
		else if(low_string1[i] != low_string2[i])
		{
			//break with unequal flag set to 1
			unequalflag = 1;
			break;
		}
		else if((low_string1[i] == '\0' && low_string2[i] != '\0') || (low_string1[i] != '\0' && low_string2[i] == '\0'))
		{
			unequalflag = 1;
			break;
		}
		
	}

	if((low_string1[i] == '\0' && low_string2[i] != '\0') || (low_string1[i] != '\0' && low_string2[i] == '\0'))
	{
		unequalflag = 1;
	}


	if(unequalflag == 0)
		return 0;
	return 1;
}

// returns the case number depending on string format
// case 1: single word
// case 2: multiple words
// case 3: phrase
// case 4: at least one word
int caseIdentifier(char* string)
{
	int casenum = 1;
	while(*string != '\0')
	{
		// case 3 contains '"'
		if(*string == '"')
		{
			casenum = 3;
			break;
		}
		// case 4 contains '*'
		else if(*string == '*')
		{
			casenum = 4;
			break;
		}
		// case 2 contains ' '
		// since case 3 always starts with a '"',
		// it does not have to be considered
		else if(*string == ' ')
		{
			casenum = 2;
			break;
		}
		string++;
	}

	return casenum;
}
