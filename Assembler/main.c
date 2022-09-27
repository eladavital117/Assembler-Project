#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "utils.h"
#include "valid.h"
#include "commands.h"


typedef struct SymbolTable* pSymbol;

typedef struct SymbolTable{
	char symbol[32];
	int value;
	int type;
	pSymbol next;
}SymbolTable;
pSymbol symbolTable = NULL;



/*
    finds the symbol in the line (assuming it has a symbol)
*/
char* findSymbol(char* line)
{
	char* pLine = line;
	char symbol[32];
	char* pSymbol;
	int i;
	i = 0;
	while (*pLine != COLON)
	{
		if (*pLine != SPACE)
		{
			symbol[i] = *pLine;
		}
		pLine++;
		i++;
	}
	symbol[i] = '\0';
	pSymbol = symbol;
	return (pSymbol);
}
/*
    copy the symbol in the line to arg char* symbol
*/
void getSymbol(char* line, char* symbol)
{
    char* pLine;
    strcpy(symbol,"");
    if (hasColon(line) == 0)
    {
        if (command(line) == EXTERN)
        {
            pLine = line;
            while ((*pLine) != SPACE)
            {
                pLine++;
            }
            while ((*pLine) == SPACE)
            {
                pLine++;
            }
            while ((*pLine) != ENTER)
            {
                strncat(symbol,pLine,1);
                pLine++;
            }
        }
    }
    
    else
    {
        pLine = line;
        while ((*pLine) != COLON)
        {
            strncat(symbol,pLine,1);
            pLine++;
        }
    }
    
}
/*
    adds a symbol to the symbol table
*/
void addSymbol(pSymbol* symbolObj, char* symbol, int value, int type, int line)
{
    pSymbol tail = *symbolObj;
    
    int check;
    check = 0;
    if (tail == NULL)
    {
        tail = (pSymbol)malloc(sizeof(SymbolTable));
        strcpy(tail->symbol, symbol);
        tail->value = value;
        tail->type = type;
        tail->next = NULL;
        *symbolObj = tail;
    }
    else
    {
        while (check == 0 && tail->next != NULL)
        {
            if (strcmp(tail->symbol,symbol) != 0)
            {
                tail = tail->next;
            }
        }
    }
    if (check == 0)
    {
        if (strcmp(tail->symbol,symbol) != 0)
        {
            tail->next = (pSymbol)malloc(sizeof(SymbolTable));
            tail = tail->next;
            strcpy(tail->symbol,symbol);
            tail->value = value;
            tail->type = type;
            tail->next = NULL;
        }
    }
}

/*
    build the symbol table for the code in the argument FILE* file
*/
int structSymbolTable(FILE* file, char* fileName)
{
	int count, value, type, address, imageSize, argCount;
	char symbol[32] = "";
	char line[200] = "";
	count = 1;
	value = -1;
	type = -1;
	address = 100;
	fixLine(file,line);
	while(strcmp(line, "EOF") != 0)
	{
	    
		if(strcmp(line, " \n") != 0)
		{
			
			getSymbol(line,symbol);
			type = command(line);
			
			value = address;
			if(type >= ADD && type <= STOP)
			{
			    
				type = 1;
				if(strcmp(symbol,"") != 0)
				{
					addSymbol(&symbolTable, symbol, value, type, count);
				}
				
				address= address+ 4;
			}
			
			if(type == EXTERN)
			{
				addSymbol(&symbolTable, symbol, 0, type, count);
			}
			
			
		}
		count++;
		fixLine(file, line);
		
	}
	
	fclose(file);
	file = fopen(fileName,"r");
	count = 1;
	value = -1;
	type = -1;
	strcpy(symbol, "");
	fixLine(file, line);
	while(strcmp(line, "EOF") != 0)
	{
		if(strcmp(line, " \n") != 0)
		{
			getSymbol(line,symbol);
			type = command(line);
			value = address;
			
			if(type == ASCIZ)
			{
				type = 2;
				address = address + ascizSize(line);
				if( strcmp(symbol,"") != 0)
				{ 
					addSymbol(&symbolTable, symbol, value, type, count);
				}
			}
			
			argCount = countComma(line);
			if(type == DB)
			{
			    
				type = 2;
				address = address + argCount;
				printf("\n");
				if( strcmp(symbol,"") != 0)
				{
					addSymbol(&symbolTable, symbol, value, type, count);
				}
			}
			if(type == DH)
			{
				type = 2;
				address = address + argCount * 2;
				if( strcmp(symbol,"") != 0)
				{
					addSymbol(&symbolTable, symbol, value, type, count);
				}
			}
			if(type == DW)
			{
				type = 2;
				address = address + argCount * 4;
				if( strcmp(symbol,"") != 0)
				{
					addSymbol(&symbolTable, symbol, value, type, count);
				}
			}
		}
		count++;
		fixLine(file, line);
	}
	
	imageSize = (address-100)*8+1+(int)((address-100)/4);
	fclose(file);
	file = fopen(fileName,"r");
	
	return imageSize;
}


struct optcodeTable
{
    int name;
    char type;
    int funct;
    int optcode;
}optcode[27];

/*
    Builds an optcode table, makes it easier later to access the type,funct and     optcode of the command.
*/
void structOptcodeTable(struct optcodeTable* optcode)
{
    int i;
    for (i = 0; i < 27; i++)
    {
        if(i <= NOR)
        {
            optcode[i].name = i;
            optcode[i].type = R;
            optcode[i].funct = i + 1;
            optcode[i].optcode = 0;
        }
        
        else if (i <= MVLO)
        {
            optcode[i].name = i;
            optcode[i].type = R;
            optcode[i].funct = i - 4;
            optcode[i].optcode = 1;
        }
        
        else if (i <= SH)
        {
            optcode[i].name = i;
            optcode[i].type = I;
            optcode[i].funct = 0;
            optcode[i].optcode = i + 2;
        }
        
        else if (i < STOP)
        {
            optcode[i].name = i;
            optcode[i].type = J;
            optcode[i].funct = 0;
            optcode[i].optcode = i + 7;
        }
        
        else if(i == STOP)
		{
			optcode[i].name = i;
			optcode[i].type = 'J';
			optcode[i].funct = 0;
			optcode[i].optcode = 63;
		}
        
        else
        {
            optcode[i].name = i;
            optcode[i].type = J;
            optcode[i].funct = 0;
            optcode[i].optcode = i + 2;
        }
    }
}




/*
    Gets a symbol and finds its address in the symbol table, if it is not in the table, returns -1;
*/
int symbolAddress(char* symbol)
{
    pSymbol p = symbolTable;
    while (p != NULL)
    {
        if (strcmp(symbol,p->symbol) == 0)
        {
            return p->value;
        }
        p = p->next;
    }
    return -1;
}

/*
    Returns 1 if the symbol is valid, 0 otherwise.
*/
int checkSymbol(char* symbol)
{
    if (command(symbol) != -1)
    {
        return 0;
    }
    else
    {
        char* pSymbol = symbol;
        if (isLetter(*pSymbol) == 0)
        {
            return 0;
        }
        pSymbol++;
        while (*pSymbol != ENTER)
        {
            if (isLetter(*pSymbol) == 0 && isDigit(*pSymbol) == 0)
            {
                return 0;
            }
            pSymbol++;
        }
        return 1;
    }
}

/*
    Returns 1 if the input line has no errors in it, otherwize - returns the number of the error.
*/
int isValid(char* line, int lineNum)
{
    char* pLine = line;
    char temp[81] = "";
    char num[50] = "";
    int cmd;
    if (strcmp(line,"LONGLINE\n") == 0) {return TOOLONG;}
    if (strcmp(line," \n") == 0){return 1;}
    else if (emptyLine(line) == 1){return 1;}
    else if (lineLen(line) > 80){ return LONGLINE;}
    cmd = command(line);
    if (cmd == -1) {return INVALIDCMD;}
    if (cmd == -2) {return LONGSYMBOL;}
    if (hasDoubleComma(line) == 1){ return DOUBLECOMMA;}
    else if (hasColon(line) == 1)
    {
        pLine = line;
        while(*pLine != COLON)
        {
            strncat(temp,pLine,1);
            pLine++;
        }
        strncat(temp,"\n",1);
        if (lineLen(temp) > 32){return LONGSYMBOL;}
        else if (checkSymbol(temp) == 0){return INVALIDSYMBOL;}
    }
    while (*pLine != SPACE && *pLine != ENTER && *pLine != TAB){pLine++;}
    if (cmd >= ADD && cmd <= NOR)
    {
        if (*pLine == ENTER) {return ARGSERROR;}
        pLine++;
        if (*pLine == DOLLAR)
        {
            pLine++;
            if (*pLine == ENTER) {return ARGSERROR;}
            findNum(&pLine,num);
            if (*pLine == ENTER){return ARGSERROR;}
            pLine++;
            if (*pLine == ENTER){return ARGSERROR;}
            if (isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
            {
                if (*pLine == DOLLAR)
                {
                    pLine++;
                    if (*pLine == ENTER){return ARGSERROR;}
                    findNum(&pLine, num);
                    if (*pLine == ENTER){return ARGSERROR;}
                    pLine++;
                    if (*pLine == ENTER) {return ARGSERROR;}
                    if (isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31) 
                    {
                        if (*pLine == DOLLAR)
                        {
                            pLine++;
                            if (*pLine == ENTER) {return ARGSERROR;}
                            findNum(&pLine,num);
                            if (isNumber(num) == 1 &&  atoi(num) >= 0 && atoi(num) <= 31)
                            {
                                if (*pLine == ENTER) {return 1;}
                                return ARGSERROR;
                            }
                            else if (isNumber(num) == 0) {return INVALIDNUM;}
                            else {return OUTOFRANGE;}  
                        }
                        else {return INVALIDTYPE;}
                    }
                    else if (isNumber(num) == 0) {return INVALIDNUM;}
                    else {return OUTOFRANGE;}
                }
                else {return INVALIDTYPE;}
            }
            else if (isNumber(num) == 0){return INVALIDNUM;}
            else {return OUTOFRANGE;} 
            
        }
        else{return INVALIDTYPE;}
    }
    else if (cmd >= MOVE && cmd <= MVLO)
    {
        if(*pLine == ENTER){return ARGSERROR;}
		pLine++;
		if( *pLine == DOLLAR )
		{
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			findNum(&pLine, num);
			if(*pLine == ENTER){return ARGSERROR;}
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			if(isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
			{
				if(*pLine == DOLLAR)
				{
					pLine++;
					if(*pLine == ENTER){return ARGSERROR;}
					findNum(&pLine, num);
					if(isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
					{
						if(*pLine == ENTER){return 1;}
						else{return ARGSERROR;}
					}
					else
					{
						if(isNumber(num) == 0){return INVALIDNUM;} 
						else{return OUTOFRANGE;}
					}
				}
				else{return INVALIDTYPE;}
			}
			else
			{
				if(isNumber(num) == 0){return INVALIDNUM;} 
				else{return OUTOFRANGE;}
			}
		}
		else{return INVALIDTYPE;}
    }
    else if ((cmd >= ADDI && cmd <= NORI) || (cmd >= LB && cmd <= SH))
    {
        if(*pLine == ENTER){return ARGSERROR;}
		pLine++;
		if( *pLine == DOLLAR )
		{
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			findNum(&pLine, num);
			if(*pLine == ENTER){return ARGSERROR;}
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			if(isNumber(num) == 1 && atoi(num) >= -32768 && atoi(num) <= 32767)
			{
				if(*pLine != DOLLAR)
				{
					findNum(&pLine, num);
					if(*pLine == ENTER){return ARGSERROR;}
					pLine++;
					if(*pLine == ENTER){return ARGSERROR;}
					if(isNumber(num) == 1 && atoi(num) >= -32768 && atoi(num) <= 32767)
					{
						if(*pLine == DOLLAR)
						{
							pLine++;
							if(*pLine == ENTER){return ARGSERROR;}
							findNum(&pLine, num);
							if(isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
							{
								if(*pLine == ENTER){return 1;}
								else{return ARGSERROR;}
							}
							else if(isNumber(num) == 0){return INVALIDNUM;}                else {return OUTOFRANGE;}
						}
						else{return INVALIDTYPE;}
					}
					else{return INVALIDNUM;}
				}
				else{return INVALIDTYPE;}
			}
			else if(isNumber(num) == 0){return INVALIDNUM;}
			else {return OUTOFRANGE;}
		}
		else{return INVALIDTYPE;}
    }
    else if (cmd >= BNE && cmd <= BGT)
    {
        if(*pLine == ENTER){return ARGSERROR;}
		pLine++;
		if(*pLine == DOLLAR)
		{
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			findNum(&pLine, num);
			if(*pLine == ENTER){return ARGSERROR;}
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			if(isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
			{
				if(*pLine == DOLLAR)
				{
					pLine++;
					if(*pLine == ENTER){return ARGSERROR;}
					findNum(&pLine, num);
					if(*pLine == ENTER){return ARGSERROR;}
					pLine++;
					if(*pLine == ENTER){return ARGSERROR;}
					if(isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
					{
						if(hasComma(pLine) == 1){return ARGSERROR;}
						if(checkSymbol(pLine) == 1){return 1;}
						else{return INVALIDSYMBOL;}
					}
					else if(isNumber(num) == 0){return INVALIDNUM;}
					else {return OUTOFRANGE;}
				}
				else{return INVALIDTYPE;}
			}
			else if(isNumber(num) == 0){return INVALIDNUM;}
			else {return OUTOFRANGE;}
		}
		else{return INVALIDTYPE;}
    }
    else if (cmd == JMP)
    {
        if(*pLine == ENTER){return ARGSERROR;}
		pLine++;
		if( *pLine == DOLLAR )
		{
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			findNum(&pLine, num);
			if(isNumber(num) == 1 && atoi(num) >= 0 && atoi(num) <= 31)
			{
				if(*pLine == ENTER){return 1;}
				else{return ARGSERROR;}
			}
			else
			{
				if(isNumber(num) == 0) {return INVALIDNUM;}
				else {return OUTOFRANGE;}
			}
		}
		if(hasComma(pLine) == 1){return ARGSERROR;}
		else if(checkSymbol(pLine) == 1){return 1;}
		return INVALIDSYMBOL;
    }
    else if (cmd == EXTERN || cmd == LA || cmd == CALL)
    {
        if (*pLine == ENTER) {return ARGSERROR;}
        pLine++;
        if (*pLine == ENTER) {return ARGSERROR;}
        if (hasComma(pLine) == 1) {return ARGSERROR;}
        if (checkSymbol(pLine))
        {
            return 1;
        }
        return INVALIDSYMBOL;
    }
    else if (cmd == STOP)
    {
        if (*pLine == ENTER)
        {
            return 1;
        }
        return STOPERROR;
    }
    else if (cmd == ASCIZ)
    {
        int i;
        i = 0;
		if(*pLine == ENTER){return ARGSERROR;}
		pLine++;
		if(*pLine == ENTER){return ARGSERROR;}
		if(*pLine == QUOTE)
		{
			pLine++;
			if(*pLine == ENTER){return ARGSERROR;}
			while(*pLine != ENTER && ((*pLine != QUOTE) || *(pLine+1) != ENTER))
			{
				i++;
				pLine++;
			}
			if (*pLine == ENTER) {return TOOLONG; }
			
			if(i > 80){return TOOLONG;}
			if(*pLine == QUOTE)
			{
				pLine++;
				if(*pLine == ENTER){return 1;}
				else{return ARGSERROR;}
			}
			else{return QUOTEERROR;}
		}
		else{return QUOTEERROR;}
    }
    else if (cmd == DB)
    {   
        if (*pLine == ENTER){return ARGSERROR;}
        pLine++;
        if (*pLine == ENTER){return ARGSERROR;}
        while (*pLine != ENTER)
        {
            findNum(&pLine,num);
            if (isNumber(num) == 0 || strcmp(num,"") == 0){return INVALIDNUM;}
            if (atoi(num) > 127 || atoi(num) < -128) {return OUTOFRANGE;}
            if (*pLine != COMMA && *pLine != ENTER) {return NOCOMMA;}
            if (*pLine == COMMA && *(pLine+1) == ENTER) {return ARGSERROR;}
            if (*pLine == COMMA) {pLine++;}
        }
        return 1;
    }
    else if (cmd == DH)
    {
        if (*pLine == ENTER){return ARGSERROR;}
        pLine++;
        if (*pLine == ENTER){return ARGSERROR;}
        while (*pLine != ENTER)
        {
            findNum(&pLine,num);
            if (isNumber(num) == 0){return INVALIDNUM;}
            if (atoi(num) > 32767 || atoi(num) < -32768) {return OUTOFRANGE;}
            if (*pLine != COMMA && *pLine != ENTER) {return NOCOMMA;}
            if (*pLine == COMMA && *(pLine+1) == ENTER) {return ARGSERROR;}
            if (*pLine == COMMA) {pLine++;}
        }
        return 1;
    }
    else if (cmd == DW)
    {
        if (*pLine == ENTER){return ARGSERROR;}
        pLine++;
        if (*pLine == ENTER){return ARGSERROR;}
        while (*pLine != ENTER)
        {
            findNum(&pLine,num);
            if(isNumber(num) == 0){return INVALIDNUM;}
            if(atoi(num) < (-2147483647-1) || atoi(num) > (2147483646+1)) {return OUTOFRANGE;}
            if (*pLine != ENTER && *pLine != COMMA){return NOCOMMA;}
            if (*pLine == COMMA && *(pLine+1) == ENTER) {return ARGSERROR;}
            if (*pLine == COMMA) {pLine++;}
        }
    }
    
    else if (cmd == ENTRY)
    {
        char symbol[33] = "";
		if(*pLine == ENTER){return ARGSERROR;}
		pLine++;
		if(*pLine == ENTER){return ARGSERROR;}
		if(hasComma(pLine) == 1){return ARGSERROR;}
		if(checkSymbol(pLine) == 1)
		{
			while(*pLine != ENTER)
			{
				strncat(symbol,pLine,1);
				pLine++;
			}
			if(symbolAddress(symbol) == -1){return SYMBOLNOTFOUND;}
			return 1;
		}
		return INVALIDSYMBOL;
    }
    
    return 1;
}



void typeR(char* line, int* rs, int* rt, int* rd)
{
    int templen;
	char* pLine = line;
	int b[3] = {0,0,0};
	int cmd = command(pLine);
	int cnt = 0;
	char temp[10] = "";
	structOptcodeTable(optcode);
	templen = 0;
	
	if (hasColon(line) == 1)
	{
		while (*pLine != COLON)
		{
			pLine++;
		}
		pLine++;
	}
	while (*pLine != SPACE)
	{
		pLine++;
	}
	pLine++;
	
	while (*pLine != ENTER && *pLine != '\0')
	{
		if (*pLine != DOLLAR && *pLine != ENTER && *pLine != '\0')
		{	
			while(*pLine != COMMA && *pLine != ENTER && *pLine != '\0')
			{
				strncat(temp, pLine, 1);
				templen++;
				pLine++;
			}
			b[cnt] = strToInt(temp, templen);
			templen = 0;
			strcpy(temp, "");
			cnt++;
		}
		pLine++;
	}
		
	if (optcode[cmd].name > NOR)
	{
		b[2] = b[1];
		b[1] = 0;
	}
	*rs = b[0];
	*rt = b[1];
	*rd = b[2];
}

void typeI(char* line, int* rs, int* immed, int* rt, int address)
{
	
	char* pLine = line;
	int b[3] = {0,0,0};
	int cmd = command(pLine);
	int cnt = 0;
	char temp[10] = "";
	char symbol[32] = "";
	int templen;
	pSymbol p;
	templen = 0;
	structOptcodeTable(optcode);
	if (hasColon(line) == 1)
	{
		while (*pLine != COLON)
		{
			pLine++;
		}
		pLine++;
	}
	while (*pLine != SPACE)
	{
		pLine++;
	}
	pLine++;
	
	if (optcode[cmd].name <= NORI || optcode[cmd].name >= LB)
	{
		while (*pLine != ENTER && *pLine != '\0')
		{
			if (*pLine != DOLLAR && *pLine != ENTER && *pLine != '\0')
			{	
				while(*pLine != COMMA && *pLine != ENTER && *pLine != '\0')
				{
					strncat(temp, pLine, 1);
					templen++;
					pLine++;
				}
				b[cnt] = strToInt(temp, templen);
				templen = 0;
				strcpy(temp, "");
				cnt++;
				if (cnt == 1)
				{
					if (*pLine == COMMA)
					{
						pLine++;
					}
					while(*pLine != COMMA && *pLine != ENTER && *pLine != '\0')
					{
						strncat(temp, pLine, 1);
						templen++;
						pLine++;
					}
					b[1] = strToInt(temp, templen);
					templen = 0;
					strcpy(temp, "");
					cnt++;
				}
			
			}
			pLine++;
		}
		
		*rs = b[0];
		*immed = b[1];
		*rt = b[2];
	}
	
	else
	{
	    strcpy(symbol,"");
		while (*pLine != ENTER && *pLine != '\0' && cnt < 2)
		{
			if (*pLine != DOLLAR && *pLine != ENTER && *pLine != '\0')
			{	
				while(*pLine != COMMA && *pLine != ENTER && *pLine != '\0' && cnt < 2)
				{
					strncat(temp, pLine, 1);
					templen++;
					pLine++;
				}
				b[cnt] = strToInt(temp, templen);
				templen = 0;
				strcpy(temp, "");
				cnt++;
			}
			pLine++;
		}
		*rs = b[0];
		*rt = b[1];
		while (*pLine != COMMA && *pLine != ENTER && *pLine != '\0')
		{
		    strncat(symbol, pLine, 1);
		    pLine++;
		}
		p = symbolTable;
		*immed = 0;
		while (p != NULL)
		{
		    if (strcmp(symbol, p->symbol)==0)
		    {
		        *immed = p->value - address;
		    }
		    p = p->next;
		}
		
	}
}

int typeJ(char* line, int* reg)
{
	char* pLine = line;
    int templen;
    char temp[10] = "";
    char symbol[32] = "";
    int immed;
    pSymbol p;
    templen = 0;
    immed = 0;
    *reg = 0;
	while (*pLine != SPACE)
	{
	    pLine++;
	}
	pLine++;
	
	if (*pLine == DOLLAR)
	{
	    *reg = 1;
	    pLine++;
	    while(*pLine != COMMA && *pLine != ENTER && *pLine != '\0')
	    {
		    strncat(temp, pLine, 1);
		    templen++;
		    pLine++;
	    }
	    immed = strToInt(temp, templen);
	}
	else
	{
	    while (*pLine != SPACE && *pLine != ENTER && *pLine != '\0')
	    {
	        strncat(symbol, pLine, 1);
	        pLine++;
	    }
	    p = symbolTable;
	    while (p != NULL)
	    {
	        if (strcmp(symbol,p->symbol) == 0)
	        {
	            immed = p->value;
	        }
	        p = p->next;
	    }
	}
	return immed;
}

void typeAsciz(char* line, int* output)
{
    int i;
    char* pLine = line;
    i = 0;
    while (*pLine != ENTER && *pLine != EOF && *pLine != SPACE)
    {
        pLine++;
    }
    pLine++;
    pLine++;
    while (*(pLine+1) != EOF && (*pLine != QUOTE || *(pLine+1) != ENTER))
    {
        output[i] = (int)(*pLine);
        pLine++;
        i++;
    }
    output[i] = 0;
    
}

void typeDbDhDw(char* line, int* output)
{
    char* pLine = line;
    char arg[32] = "";
    int argCount = countComma(line);
    int i;
    int len;
    while (*pLine != ENTER && *pLine != EOF && *pLine != SPACE)
    {
        pLine++;
    }
    pLine++;
    len = 0;
    for (i = 0; i < argCount; i++)
    {
        while (*pLine != ENTER && *pLine != EOF && *pLine != SPACE && *pLine != COMMA)
        {
            strncat(arg, pLine, 1);
            pLine++;
            len++;
        }
        output[i] = strToInt(arg, len);
        len = 0;
        strcpy(arg,"");
        pLine++;
    }
}


/*************************/

/* Reads the input file or files and transform them to binary code and
   creates the output files if the code is valid, otherwise prints errors.
*/
void readFile(FILE* file1, FILE* file2, FILE* file3, FILE* file4, int imageSize, char* fileName)
{
    
    int allCodeValid;
    
    char c;
    char line[81] = "";
    int lineNum;
    int lineLen;
    
    int i;
    
    int address = 100;
    int IC = 0;
    char *image = (char*) malloc(imageSize * sizeof(char));
    char *hexImage = (char*) malloc(imageSize * sizeof(char));
    char *externals = (char*) malloc(imageSize * sizeof(char));
    char *enternals = (char*) malloc(imageSize * sizeof(char));
    char *enternalsArr = (char*) malloc(imageSize * sizeof(char));
    
    char bin[500] = "";
    char tempByte[33] = "";
    char tempByte2[33] = "";
    char* pImage = image;
    int extLen;
    pSymbol p = symbolTable;
    char currentEntry[81] = "";
    
    char* pLine;
	char* pLine2;
	char* pLineEntry;
	char extTemp[81] = "";
	char addressStr[5] = "";
	int cmd;
	char valStr[6];
	char regc;
	int rs, rt, rd, immed, optc, reg, funct;
	int output[32];
	int k;
	int argCount;
	pSymbol pp;
	
	int i1;
    int address1;
    char address1str[5];
    char temp1[5] = "";
    char temp2;
    char ext[100] = "";
    char ent[100] = "";
    char ob[100] = "";
    
    FILE* obf;
    char tempStr1[10];
    
    allCodeValid = 1;
    lineLen = 0;
    lineNum = 1;
    i = 0;
    strcpy(image,"");
    strcpy(hexImage,"");
    
    strcpy(externals,"");
    strcpy(enternals,"");
    strcpy(enternalsArr,"");
    
    
    while ((c = fgetc(file1)) != EOF)
    {
        if (c != ENTER)
        {
            if (c == TAB && lineLen < 81)
            {
                strncat(line," ",1);
            }
            else if (lineLen < 81)
            {
                strncat(line,&c,1);
            }
            lineLen++;
            i++;
        }
        else
        {
            if (lineLen > 81)
            {
                allCodeValid = 0;
                
            }
            else
            {
                fixLine(file2, line);
            	pLine = line;
            	pLine2 = line;
            	pLineEntry = line;
            	strcpy(extTemp,"");
            	strcpy(addressStr,"");
            	cmd = command(pLine);
            	
            	if (*line != SEMICOLON && (cmd <= STOP || cmd == ENTRY) && isValid(line,lineNum) == 1)
                {   
                
                    if (cmd == ENTRY)
                	{
                	    while (*pLineEntry != ENTER && *pLineEntry != SPACE && *pLineEntry != EOF && *pLineEntry != '\0')
                	    {
                	        pLineEntry++;
                	    }
                	    pLineEntry++;
                	    strcpy(currentEntry,"");
                	    while (*pLineEntry != ENTER && *pLineEntry != SPACE && *pLineEntry != EOF && *pLineEntry != '\0')
                	    {
                	        strncat(currentEntry,pLineEntry,1);
                	        pLineEntry++;
                	    }
                	    structOptcodeTable(optcode);
                	    pp = p;
                	    while (p != NULL)
                	    {
                	        if (strcmp(p->symbol,currentEntry) == 0)
                	        {
                	            if (strcmp(enternals,"") != 0)
                	            {
                	                strncat(enternals,"\n",1);
                	            }
                	            strcat(enternals,p->symbol);
                	            
                	            if (p->value < 1000)
                	            {
                	                sprintf(valStr," 0%d",p->value);
                	            }
                	            else
                	            {
                	                sprintf(valStr," %d",p->value);
                	            }
                	            strcat(enternals,valStr);
                	            
                	        }
                	        p = p->next;
                	    }
                	    p = pp;
                	}
                    
                    
                    
                    rs = 0;
                    rt = 0;
                    rd = 0;
                    immed = 0;
                    reg = 0;
                    optc = optcode[cmd].optcode;
                    funct = optcode[cmd].funct;
                    extLen = 0;
                    while (*pLine2 != SPACE && *pLine2 != EOF && *pLine2 != '\0' && *pLine2 != ENTER)
                    {
                        pLine2++;
                    }
                    pLine2++;
                    while (*pLine2 != SPACE && *pLine2 != EOF && *pLine2 != '\0' && *pLine2 != ENTER)
                    {
                        strncat(extTemp, pLine2, 1);
                        extLen++;
                        pLine2++;
                	}
                	pp = p;
                	while (p != NULL)
                    {
                        if (strcmp(p->symbol,extTemp) == 0 && p->type == EXTERN)
                        {
                            if (address < 1000)
                            {
                                sprintf(addressStr,"0%d",address);
                            }
                            else
                            {
                                sprintf(addressStr,"%d",address);
                            }
                            if (strcmp(externals,"") != 0)
                            {
                                strncat(externals,"\n",1);
                            }
                            strncat(externals,extTemp,extLen);
                            strncat(externals," ",1);
                            strncat(externals,addressStr,4);
                        }
                        
                        p = p->next;
                    }
                    p = pp;
                	
                	if (cmd >= ADD && cmd <= MVLO)
                	{
                	    typeR(pLine, &rs, &rt, &rd);
                	    strcpy(tempByte,"");
                	    decToBin(optc,6,bin);
                	    strncat(tempByte, bin, 6);
                	    decToBin(rs,5,bin);
                	    strncat(tempByte, bin, 5);
                	    decToBin(rt,5,bin);
                	    strncat(tempByte, bin, 5);
                	    decToBin(rd,5,bin);
                	    strncat(tempByte, bin, 5);
                	    decToBin(funct,5,bin);
                	    strncat(tempByte, bin, 5);
                	    decToBin(0,6,bin);
                	    strncat(tempByte, bin, 6);
                	    
                	    tempByte[33] = '\0';
                	    strncat(tempByte2, tempByte+24, 8);
                	    strncat(tempByte2, tempByte+16, 8);
                	    strncat(tempByte2, tempByte+8, 8);
                	    strncat(tempByte2, tempByte+0, 8);
                	    strncat(image, tempByte2, 32);
                	    strcpy(tempByte,"");
                	    strcpy(tempByte2,"");
                	    address += 4;
                	    IC += 4;
                	}
                	else if (cmd >= ADDI && cmd <= SH)
                	{
                	    typeI(pLine, &rs, &immed, &rt, address);
                	    decToBin(optc,6,bin);
                	    strncat(tempByte, bin, 6);
                	    decToBin(rs,5,bin);
                	    strncat(tempByte, bin, 5);
                	    decToBin(rt,5,bin);
                	    strncat(tempByte, bin, 5);
                	    decToBin(immed,16,bin);
                	    strncat(tempByte, bin, 16);
                	    
                	    tempByte[33] = '\0';
                	    strncat(tempByte2, tempByte+24, 8);
                	    strncat(tempByte2, tempByte+16, 8);
                	    strncat(tempByte2, tempByte+8, 8);
                	    strncat(tempByte2, tempByte+0, 8);
                	    strncat(image, tempByte2, 32);
                	    strcpy(tempByte,"");
                	    strcpy(tempByte2,"");
                	    IC += 4;
                	    address += 4;
                	}
                	else if (cmd == STOP)
                	{
                	    immed = 0;
                	    reg = 0;
                	    decToBin(optc,6,bin);
                	    strncat(tempByte,bin,6);
                	    strncat(tempByte,"0",1);
                	    decToBin(immed,25,bin);
                	    strncat(tempByte,bin,25);
                	    tempByte[33] = '\0';
                	    strncat(tempByte2, tempByte+24, 8);
                	    strncat(tempByte2, tempByte+16, 8);
                	    strncat(tempByte2, tempByte+8, 8);
                	    strncat(tempByte2, tempByte+0, 8);
                	    strncat(image, tempByte2, 32);
                	    strcpy(tempByte,"");
                	    strcpy(tempByte2,"");
                	    
                	    IC += 4;;
                	    address += 4;
                	}
                    else if (cmd >= JMP && cmd <= STOP)
                    {
                        immed = typeJ(pLine, &reg);
                	    
                	    decToBin(optc,6,bin);
                	    strncat(tempByte, bin, 6);
                	    
                	    regc = (char)(reg+48);
                	    strncat(tempByte,&regc,1);
                	    decToBin(immed,25,bin);
                	    strncat(tempByte, bin, 25);
                	    
                	    tempByte[33] = '\0';
                	    strncat(tempByte2, tempByte+24, 8);
                	    strncat(tempByte2, tempByte+16, 8);
                	    strncat(tempByte2, tempByte+8, 8);
                	    strncat(tempByte2, tempByte+0, 8);
                	    strncat(image, tempByte2, 32);
                	    strcpy(tempByte,"");
                	    strcpy(tempByte2,"");
                	    IC += 4;
                	    address += 4;
                    }
                }
                
                else if (*line != SEMICOLON && (cmd <= STOP || cmd == ENTRY) && isValid(line,lineNum) != 1)
                {
                    checkValid(fileName,lineNum,isValid(line,lineNum));
                    allCodeValid = 0;
                }
            }
        	lineLen = 0;
            strcpy(line,"");
            i = 0;
            lineNum ++;
        }
    }
    lineNum = 1;
    while ((c = fgetc(file3)) != EOF)
    {
        if (c != ENTER)
        {
            if (c == TAB && lineLen < 81)
            {
                strncat(line," ",1);
            }
            else if (lineLen < 81)
            {
                strncat(line,&c,1);
            }
            lineLen++;
            i++;
        }
        else
        {
            if (lineLen > 81)
            {
                allCodeValid = 0;
                printf("File: %s Line: %d Line is Too Long\n", fileName, lineNum);
            }
            else
            {
            	fixLine(file4, line);
            	pLine = line;
            	cmd = command(pLine);
                if (*line != SEMICOLON && cmd > STOP && cmd <= DH && isValid(line,lineNum) == 1)
                {   
                    structOptcodeTable(optcode);
                    
                    if(cmd == ASCIZ)
		            {
		                
		                for (k = 0; k < 32; k++)
		                {
		                    output[k] = -1;
		                }
		                typeAsciz(line, output);
		                for (k = 0; k < 32 && output[k] != -1; k++)
		                {
		                    decToBin(output[k],8,bin);
		                    strncat(image, bin, 8);
		                    address++;
		                }
		            }
		            else if(cmd == DB)
		            {
		                for (k = 0; k < 32; k++)
		                {
		                    output[k] = 0;
		                }
			            typeDbDhDw(line, output);
			            
			            
			            argCount = countComma(line);
		                for (k = 0; k < argCount; k++)
		                {
		                    decToBin(output[k],8,bin);
		                    strncat(image, bin, 8);
		                    address++;
		                }
		            }
		            else if(cmd == DH)
		            {
		                for (k = 0; k < 32; k++)
		                {
		                    output[k] = 0;
		                }
			            typeDbDhDw(line, output);
			            argCount = countComma(line);
		                for (k = 0; k < argCount; k++)
		                {
		                    decToBin(output[k],16,bin);
		                    strcpy(tempByte,"");
		                    tempByte[33] = '\0';
		                    strncat(tempByte,bin+8,8);
		                    strncat(tempByte,bin,8);
		                    strncat(image, tempByte, 16);
		                    address += 2;
		                }
		            }
		            else if(cmd == DW)
		            {
		                for (k = 0; k < 32; k++)
		                {
		                    output[k] = 0;
		                }
			            typeDbDhDw(line, output);
			            argCount = countComma(line);
		                for (k = 0; k < argCount; k++)
		                {
		                    decToBin(output[k],32,bin);
		                    strcpy(tempByte,"");
                    	    strcpy(tempByte2,"");
		                    strncat(tempByte, bin, 32);
		                    tempByte[33] = '\0';
		                    
                    	    strncat(tempByte2, tempByte+24, 8);
                    	    strncat(tempByte2, tempByte+16, 8);
                    	    strncat(tempByte2, tempByte+8, 8);
                    	    strncat(tempByte2, tempByte+0, 8);
                    	    strncat(image, tempByte2, 32);
                    	    strcpy(tempByte,"");
                    	    strcpy(tempByte2,"");
		                    address += 4;
		                }
		            }    	
                }
                else if (*line != SEMICOLON && cmd > STOP && cmd <= DH && isValid(line,lineNum) != 1)
                {
                    checkValid(fileName,lineNum,isValid(line,lineNum));
                    allCodeValid = 0;
                }
            }
            lineLen = 0;
            strcpy(line,"");
            i = 0;
            lineNum++;
        }
        
    }
    
    strcpy(ext,fileName);
    strcpy(ent,fileName);
    strcpy(ob,fileName);
    address1 = 100;
    strncat(ext,".ext",4);
    strncat(ent,".ent",4);
    strncat(ob,".ob",3);
    
    if (allCodeValid == 1)
    {
        if (strcmp(enternals,"") != 0)
        {
            FILE* entf = fopen(ent,"w");
            fputs(enternals,entf);
            fclose(entf);
        }
    
        if (strcmp(externals,"") != 0)
        {
            FILE* extf = fopen(ext,"w");
            fputs(externals,extf);
            fclose(extf);
        }
        obf = fopen(ob,"w");
        sprintf(tempStr1,"     %d %d\n", IC, (address-100)/4);
        strcat(hexImage,tempStr1);
        for (i1 = 0; i1 < imageSize; i1+=4)
        {
            if (i1 == 0)
            {
                if (address1 < 1000)
                {
                    sprintf(address1str,"0%d ",address1);
                    strncat(hexImage,address1str,5);
                }
                else
                {
                    sprintf(address1str,"%d ",address1);
                    strncat(hexImage,address1str,4);
                }
                address1 += 4;
            }
            if (*pImage != '\0')
            {
                if (i1 % 32 == 0 && i1 > 0)
                {
                    strncat(hexImage,"\n",1);
                    if (address1 < 1000)
                    {
                        strncat(hexImage,"0",1);
                    }
                    sprintf(address1str,"%d ",address1);
                    strncat(hexImage,address1str,4);
                    address1 += 4;
                }
                else if (i1 % 8 == 0 && i1 > 0)
                {
                    strncat(hexImage," ",1);
                }
                strncat(temp1, pImage, 4);
                temp2 = binToHex(temp1);
                if (temp2 > 40 && temp2 < 100)
                {
                    strncat(hexImage,&temp2,1);
                }
                strcpy(temp1,"");
            }
            pImage += 4;
        }
        fputs(hexImage,obf);
    }
    free(image);
    free(externals);
    free(enternals); 
    free(enternalsArr);
    free(hexImage);
}

/*
    Loops through the input files, create tables and calls "ReadFile" for each file.
*/
int main(int argc, char **argv)
{
	int i;
	int j;
    FILE* file1;
	FILE* file2;
	FILE* file3;
	FILE* file4;
	pSymbol p;
	pSymbol pp;
	char fileName[100] = "";
	for (i = 1; i < argc; i++)
	{
	    int imageSize;
	    int symbolCount;
	    symbolCount = 0;
	    imageSize = 0;
	    strcpy(fileName,"");
		file1 = fopen(argv[i],"r");
		file2 = fopen(argv[i],"r");
		file3 = fopen(argv[i],"r");
		file4 = fopen(argv[i],"r");
	    for (j = 0; argv[i][j] != '.'; j++)
	    {
	        fileName[j] = argv[i][j];
	    }
	    
	    imageSize = structSymbolTable(file1,argv[i]);
	    
		p = symbolTable;
		pp = p;
		
	    while(p != NULL)
	    {
		    p = p->next;
	    }
	    readFile(file1, file2, file3, file4, imageSize, fileName);
	    if (symbolCount > 0)
	    {
	        pp->next = NULL;
	        pp->value = -1;
	        strcpy(pp->symbol,"EOF");
	        pp->type = -1;
	    }
	}
	fclose(file1);
	fclose(file2);
	fclose(file3);
	fclose(file4);
    return 0;
}
