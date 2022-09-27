#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

int lineLength(char* line);
char* hexToBin(char hex);
void decToBin(int dec, int bits, char* bin);
int hasColon(char* line);
int strToInt(char* str, int len);
int countComma(char * line);
char binToHex(char* bin);
int ascizSize(char* line);
int command(char* line);
void fixLine(FILE* file, char* line);

#endif
