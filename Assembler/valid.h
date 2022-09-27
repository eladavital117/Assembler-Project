#ifndef VALID_H_INCLUDED
#define VALID_H_INCLUDED

void checkValid(char* fileName, int lineNum, int validResult);
int hasComma(char* line);
int hasDoubleComma(char* line);
int lineLen(char* line);
int isLetter(char character);
int isDigit(char character);
int isNumber(char* num);
void findNum(char** pLine, char* num);
int emptyLine(char* line);

#endif
