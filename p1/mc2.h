//mastlouis

#ifndef MC2_h
#define MC2_h

typedef struct UserCommand{
	int numArgs;
	//First argument is the command's name
	char** args;
	char* fullName;
	int amperstand;
	int background;
}Ucommand;

typedef struct RunningProcess_{
	pid_t pid;
	//struct rusage startUsage;
	//struct rusage endUsage;
	int finished;
	//int taskNumber;
}RunningProcess;

typedef struct RunningProcessList_{
	int numProcesses;
	RunningProcess **list;
}RunningProcessList;

void menu(int *numNewCommands, Ucommand **addedCommands, RunningProcessList *listOfProcesses);
void runCommand(char* name, char** arguments);
void runBackgroundCommand(char* name, char** arguments, RunningProcessList *listOfProcesses);
void switchBoard(char* commandString, int *numNewCommands, Ucommand **addedCommands, RunningProcessList *listOfProcesses);
int replaceSpaces(char* argString, char** destArray);
void prepForkCommand(char* name, char* message, char* argQuestion);
void addCommand(char* newCommand, int *numNewCommands, Ucommand **addedCommands);
void changeDirectory(char* path);
int parseArgs(char* argsSource, char** argsDest);
void deleteNewLine(char* string);
void displayProcesses(RunningProcessList *listOfProcesses);



#define MAX_ARGS 32
#define LINE_SIZE 128
#define MAX_COMMANDS 128
#define MAX_PROCESSES 100

#endif