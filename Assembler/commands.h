#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

void typeR(char* line, int* rs, int* rt, int* rd);
void typeI(char* line, int* rs, int* immed, int* rt, int address);
int typeJ(char* line, int* reg);
void typeAsciz(char* line, int* output);
void typeDbDhDw(char* line, int* output);

#endif
