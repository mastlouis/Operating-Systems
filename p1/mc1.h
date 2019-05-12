//mastlouis

#ifndef MC1_h
#define MC1_h

typedef struct UserCommand{
	int numArgs;
	//First argument is the command's name
	char** args;
	char* fullName;
	int amperstand;
	int background;
}Ucommand;

void menu(int *numNewCommands, Ucommand **addedCommands);
void runCommand(char* name, char** arguments);
void switchBoard(char* commandString, int *numNewCommands, Ucommand **addedCommands);
int replaceSpaces(char* argString, char** destArray);
void prepForkCommand(char* name, char* message, char* argQuestion);
void addCommand(char* newCommand, int *numNewCommands, Ucommand **addedCommands);
void changeDirectory(char* path);
int parseArgs(char* argsSource, char** argsDest);
void deleteNewLine(char* string);



#define MAX_ARGS 32
#define LINE_SIZE 128
#define MAX_COMMANDS 128
#define MAX_PROCESSES 100

#endif