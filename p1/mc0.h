//mastlouis

#ifndef MC0_h
#define MC0_h

void menu();
void runCommand(char* name, char** arguments);
void switchBoard(char* commandString);
int replaceSpaces(char* argString, char** destArray);
void prepForkCommand(char* name, char* message, char* argQuestion);
int parseArgs(char* argsSource, char** argsDest);
void deleteNewLine(char* string);

#define MAX_ARGS 32
#define LINE_SIZE 128

#endif