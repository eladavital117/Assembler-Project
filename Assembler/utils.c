#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "utils.h"

/*
    Returns the length of a line
*/
int lineLength(char* line)
{
    int count;
    char* pLine = line;
    count = 0;
    while (*pLine != ENTER && *pLine != '\0' && *pLine != EOF)
    {
        pLine++;
        count++;
    }
    return count;
}

/*
    Converts a 1 digit hexadecimal number to 4 digit binary number
*/
char* hexToBin(char hex)
{
    char *bin = "abcd";
    
    switch(hex) {
        case '0':
            bin = "0000"; break;
        case '1':
            bin = "0001"; break;
        case '2':
            bin = "0010"; break;
        case '3':
            bin = "0011"; break;
        case '4':
            bin = "0100"; break;
        case '5':
            bin = "0101"; break;
        case '6':
            bin = "0110"; break;
        case '7':
            bin = "0111"; break;
        case '8':
            bin = "1000"; break;
        case '9':
            bin = "1001"; break;
        case 'a':
            bin = "1010"; break;
        case 'b':
            bin = "1011"; break;
        case 'c':
            bin = "1100"; break;
        case 'd':
            bin = "1101"; break;
        case 'e':
            bin = "1110"; break;
        case 'f':
            bin = "1111"; break;
    }
    return bin;
}

/*
    Converts decimal number to binary number
    arg bits is the desired length of the binary number, using 2s complement. 
*/

void decToBin(int dec, int bits, char* bin)
{
    int negative;
    int temp;
    int j;
    int foundOne;
    negative = 0;
    if (dec < 0)
    {
        negative = 1;
    }
    temp = dec;
    if (dec < 0)
    {
        temp = -dec;
    }
    
    bin[bits] = '\0';
    j = bits-1;
    while (temp > 0)
    {
        bin[j] = (char)((temp % 2)+48);
        temp = temp / 2;
        j--;
    }
    bin[j] = (char)((temp % 2)+48);
    j--;
    while (j >= 0)
    {
        bin[j] = '0';
        j--;
    }
    
    foundOne = 0;
    if (negative == 1)
    {
        j = bits-1;
        while (j >= 0)
        {
            if (foundOne == 1)
            {
                if (bin[j] == '1')
                {
                    bin[j] = '0';
                }
                else
                {
                    bin[j] = '1';
                }
            }
            else
            {
                if (bin[j] == '1')
                {
                    foundOne = 1;
                }
            }
            j--;
        }
    }
}


/*
    Returns 1 if the input line has a colon, 0 otherwise.
*/
int hasColon(char* line)
{
    char* pLine = line;
    while (*pLine != ENTER)
    {
        if (*pLine == COLON)
        {
            return 1;
        }
        pLine++;
    }
    return 0;
}


/*
    Converts string to int, assuming the string is all numbers.
*/
int strToInt(char* str, int len)
{
	int start;
	int negative;
	int i;
	int result;
	int mult;
	start = 0;
	negative = 0;
	if (str[0] == '+')
	{
		start = 1;
	}
	else if (str[0] == '-')
	{
		start = 1;
		negative = 1;
	}
	
	mult = 1;
	result = 0;
	for(i = len-1; i >= start; i--)
	{
		result += mult*(int)(str[i]-48);
		mult *= 10;
	}
	
	if (negative == 1)
	{
		return -result;
	}
	return result;
}

/*
    Counts the number of commas in a line, returns the number plus 1 to get the amount of arguments in the line.
*/
int countComma(char * line)
{
    char * pLine = line;
    int count;
    count = 0;
    if (hasColon(pLine) == 1)
    {
        while (( * pLine) != COLON)
        {
            pLine++;
        }
    }
    pLine++;
    while (( * pLine) != ENTER)
    {
        if (( * pLine) == COMMA)
        {
            count++;
        }
        pLine++;
    }
    return count + 1;
}

/*
    Conberts 4 digit binary number to 1 digit hexadecimal number
*/
char binToHex(char* bin)
{
    char hex;
    if (strcmp(bin, "0000") == 0){hex = '0';}
    else if (strcmp(bin, "0001") == 0){hex = '1';}
    else if (strcmp(bin, "0010") == 0){hex = '2';}
    else if (strcmp(bin, "0011") == 0){hex = '3';}
    else if (strcmp(bin, "0100") == 0){hex = '4';}
    else if (strcmp(bin, "0101") == 0){hex = '5';}
    else if (strcmp(bin, "0110") == 0){hex = '6';}
    else if (strcmp(bin, "0111") == 0){hex = '7';}
    else if (strcmp(bin, "1000") == 0){hex = '8';}
    else if (strcmp(bin, "1001") == 0){hex = '9';}
    else if (strcmp(bin, "1010") == 0){hex = 'A';}
    else if (strcmp(bin, "1011") == 0){hex = 'B';}
    else if (strcmp(bin, "1100") == 0){hex = 'C';}
    else if (strcmp(bin, "1101") == 0){hex = 'D';}
    else if (strcmp(bin, "1110") == 0){hex = 'E';}
    else if (strcmp(bin, "1111") == 0){hex = 'F';}
    return hex;
}

/*
    Calculates the size in the memory that is required for the asciz command.
*/
int ascizSize(char* line)
{
    char * pLine = line;
    int size;
    size = 0;
    while (( * pLine) != SPACE)
    {
        pLine++;
    }
    while (( * pLine) == SPACE)
    {
        pLine++;
    }
    while ( * pLine != ENTER)
    {
        pLine++;
        size++;
    }
    return size - 1;
}

/*
    Gets a line and returns a number representing the command in the line.
*/
int command(char* line)
{
    char* pLine = line;
    char command[40] = "";
    int count;
	if (hasColon(pLine) == 1)
	{
		while (*pLine != COLON && *pLine != SPACE && *pLine != ENTER && *pLine != EOF)
		{
			pLine++;
		}
		pLine++;
	}
	count = 0;
	if (*pLine == DOT)
	{
		pLine++;
		while(*pLine != SPACE && *pLine != ENTER && count < 33){
			strncat(command,pLine,1);
			pLine++;
			count++;
		}
		
		if (count == 33)
		{
		    return -2;
		}
		if (strcmp(command, "asciz") == 0){ return ASCIZ; }
		else if (strcmp(command, "db") == 0){ return DB; }
		else if (strcmp(command, "dw") == 0){ return DW; }
		else if (strcmp(command, "dh") == 0){ return DH; }
		else if (strcmp(command, "extern") == 0){ return EXTERN; }
		else if (strcmp(command, "entry") == 0){ return ENTRY; }
		else { return -1; }
	}
	
	while (*pLine != SPACE && *pLine != ENTER && *pLine != EOF)
	{
		strncat(command, pLine, 1);
		pLine++;
	}
	
	if (strcmp(command, "add") == 0){ return ADD; }
	else if (strcmp(command, "sub") == 0){ return SUB; }
	else if (strcmp(command, "and") == 0){ return AND; }
	else if (strcmp(command, "or") == 0){ return OR; }
	else if (strcmp(command, "nor") == 0){ return NOR; }
	else if (strcmp(command, "move") == 0){ return MOVE; }
	else if (strcmp(command, "mvhi") == 0){ return MVHI; }
	else if (strcmp(command, "mvlo") == 0){ return MVLO; }
	else if (strcmp(command, "addi") == 0){ return ADDI; }
	else if (strcmp(command, "subi") == 0){ return SUBI; }
	else if (strcmp(command, "andi") == 0){ return ANDI; }
	else if (strcmp(command, "ori") == 0){ return ORI; }
	else if (strcmp(command, "nori") == 0){ return NORI; }
	else if (strcmp(command, "bne") == 0){ return BNE; }
	else if (strcmp(command, "beq") == 0){ return BEQ; }
	else if (strcmp(command, "blt") == 0){ return BLT; }
	else if (strcmp(command, "bgt") == 0){ return BGT; }
	else if (strcmp(command, "lb") == 0){ return LB; }
	else if (strcmp(command, "sb") == 0){ return SB; }
	else if (strcmp(command, "lw") == 0){ return LW; }
	else if (strcmp(command, "sw") == 0){ return SW; }
	else if (strcmp(command, "lh") == 0){ return LH; }
	else if (strcmp(command, "sh") == 0){ return SH; }
	else if (strcmp(command, "jmp") == 0){ return JMP; }
	else if (strcmp(command, "la") == 0){ return LA; }
	else if (strcmp(command, "call") == 0){ return CALL; }
	else if (strcmp(command, "stop") == 0){ return STOP; }
	else { return -1; }
}

/*
    Removes extra spaces in the line, leaves only 1 space between the command and the argument/s.
*/
void fixLine(FILE* file, char* line)
{
    int lenCount;
    char c;
    char temp[85] = "";
    int i;
    char* copy;
    lenCount = 0;
	strcpy(line, "");
	
	c = fgetc(file);
	
	
	i = 1;
	
	if (c == ENTER)
	{
		strcpy(line, " \n");
	}
	else if (c == EOF)
	{
		strcpy(line, "EOF");
	}
	else
	{
		while (c != ENTER)
		{
		    if (c == TAB && lenCount < 81)
		    {
		        strncat(temp, " ", 1);
		    }
		    else if (lenCount < 81)
		    {
		        strncat(temp, &c, 1);
		    }
			
			c = fgetc(file);
			lenCount++;
			i++;
		}
		strncat(temp, &c, 1);
		copy = temp;
		while ((*copy) == SPACE)
		{
			copy++;
		}
		if (copy[0] == SEMICOLON)
		{
			strcpy(line, " \n");
		}
		else
		{
			if (hasColon(copy) == 1)
			{
				while ((*copy) != COLON)
				{
					if ((*copy) != SPACE)
					{
						strncat(line, copy, 1);
					}
					copy++;
				}
				strncat(line, copy, 1);
				copy++;
			}
			while ((*copy) == SPACE)
			{
				copy++;
			}
			if (strcmp(copy, "\n") == 0)
			{
				strcpy(line, " \n");
			}
			else
			{
				while((*copy) != ENTER && (*copy) != SPACE)
				{
					strncat(line, copy, 1);
					copy++;
				}
			
				if ((*copy) == ENTER)
				{
					strncat(line, copy, 1);
				}
				else
				{
					strncat(line, copy, 1);
					copy++;
					while ((*copy) != ENTER)
					{
						if((*copy) != SPACE)
						{
							strncat(line, copy, 1);
						}
						copy++;
					}
					strncat(line, copy, 1);
				}
			}
		}
	}
	if (lenCount > 80)
	{
	    strcpy(line,"LONGLINE\n");
	}
}

