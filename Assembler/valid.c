#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "valid.h"


/*
    Prints all the errors that were found in the assembly code.
*/
void checkValid(char* fileName, int lineNum, int validResult)
{
    printf("File: %s.as ",fileName);
    if (validResult == DOUBLECOMMA)
    {
        printf("Line %d: Argument is Missing\n",lineNum);
    }
    else if (validResult == NOCOMMA)
    {
        printf("Line %d: Comma between Arguments not found\n",lineNum);
    }
    else if (validResult == INVALIDNUM)
    {
        printf("Line %d: Invalid Argument\n",lineNum);
    }
    else if (validResult == ARGSERROR)
    {
        printf("Line %d: Invalid Amount of Arguments\n",lineNum);
    }
    else if (validResult == OUTOFRANGE)
    {
        printf("Line %d: Argument is out of range\n",lineNum);
    }
    else if (validResult == INVALIDCMD)
    {
        printf("Line %d: Command is Unknown\n",lineNum);
    }
    else if (validResult == INVALIDTYPE)
    {
        printf("Line %d: Invalid type of Argument\n",lineNum);
    }
    else if (validResult == INVALIDSYMBOL)
    {
        printf("Line %d: Invalid Symbol Name\n",lineNum);
    }
    else if (validResult == LONGSYMBOL)
    {
        printf("Line %d: Symbol is Too Long\n",lineNum);
    }
    else if (validResult == STOPERROR)
    {
        printf("Line %d: Unnecessary Characters detected in stop command\n",lineNum);
    }
    
    else if (validResult == QUOTEERROR)
    {
        printf("Line %d: Quote is Missing\n",lineNum);
    }
    else if (validResult == SYMBOLNOTFOUND)
    {
        printf("Line %d: Symbol Not Found\n",lineNum);
    }
    else if (validResult == TOOLONG)
    {
        printf("Line %d: The line is too long\n",lineNum);
    }
    else
    {
        printf("Line %d: Unknown Error\n Valid Result: %d",lineNum, validResult);
    }
}

/*
    Returns 1 if the input line has a comma, 0 otherwise.
*/

int hasComma(char* line)
{
    char* pLine = line;
    while (*pLine != ENTER)
    {
        if (*pLine == COMMA)
        {
            return 1;
        }
        pLine++;
    }
    return 0;
}

/*
    Returns 1 if the input line has a 2 commas in a row, 0 otherwise.
*/
int hasDoubleComma(char* line)
{
	char* pLine = line;
	while (*pLine != ENTER)
	{
	    if (*pLine == COMMA && *(pLine+1) == COMMA)
	    {
	        return 1;
	    }
	    pLine++;
	}
	return 0;
}

/*
    Returns the length of the line
*/
int lineLen(char* line)
{
    char* pLine = line;
    int len;
    len = 0;
    while (*pLine != ENTER)
    {
        pLine++;
        len++;
    }
    return len;
}

/*
    Returns 1 if the input character is alphabet letter in English, 0 otherwise.
*/
int isLetter(char character)
{
    if ((character >= 65 && character <= 90) || (character >= 97 && character <= 122))
    {
        return 1;
    }
    return 0;
}

/*
    Returns 1 if the input character is a digit 0-9, 0 otherwise.
*/
int isDigit(char character)
{
    if (character >= 48 && character <= 57)
    {
        return 1;
    }
    return 0;
}

/*
    Returns 1 if the input string is a number, 0 otherwise
*/
int isNumber(char* num)
{
    int i;
    i = 0;
    if (num[0] == '-' || num[0] == '+')
    {
        i++;
    }
    while(num[i] != '\0' && isDigit(num[i]) == 1)
    {
        i++;
    }
    if (num[i] == '\0' && i > 0)
    {
        return 1;
    }
    return 0;
}

/*
    Finds the number in the string (first argument) and copies it to the num (second argument)
*/
void findNum(char** pLine, char* num)
{
    strcpy(num,"");
    while (**pLine != COMMA && **pLine != ENTER)
    {
        strncat(num,*pLine,1);
        (*pLine)++;
    }
}

/*
    Returns 1 if the input line is empty (empty = no characters except spaces and \n), 0 otherwise.
*/
int emptyLine(char* line)
{
    char* pLine = line;
    while (*pLine != ENTER && *pLine == SPACE)
    {
        pLine++;
    }
    if (*pLine == ENTER)
    {
        return 1;
    }
    return 0;
}
