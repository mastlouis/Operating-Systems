//mastlouis

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "mc2.h"
#include <string.h>
#include <errno.h>

/*
* As far as the rest of the code is concerned, the main ongoings
* of main are to repeatedly call the menu, which allows the user
* to repeatedly enter commands.
*/
int main(int argc, const char *argv []){
	int numNewCommands = 0;
	/*
	numNewCommands stores the number of user-added commands and the 
	state of the program. If numNewCommands is zero or positive, the
	program will continue to run. If numNewCommands is negative, the
	program will exit.
	*/
	Ucommand **addedCommands = malloc(sizeof(Ucommand) * MAX_COMMANDS);
	//addedCommands is the list of user-added commands
	RunningProcessList *listOfProcesses = malloc(sizeof(RunningProcessList));
	//listOfProcess is the list of processes running in the background
	//and the number of processes running in the background
	listOfProcesses->list = malloc(sizeof(RunningProcess)*MAX_PROCESSES);
	listOfProcesses->numProcesses = 0;

	printf("%s\n", "===== Mid-Day Commander, v2 =====");

	while(numNewCommands > -1)
		menu(&numNewCommands, addedCommands, listOfProcesses);

	/*
	This line is meant to prevent the program from exiting before all
	child processes have finished running. It's not reliable because
	This program attempts to share data across processes, which I
	confused with threads.
	*/
	//while(listOfProcesses->numProcesses > 0);

	for(int i = 0; i < numNewCommands; i++)
		free(addedCommands[i]);

	free(addedCommands);
	return 0;
}

/*
* This function prints a list of options and allows the user to 
* select one by entering a string corresponding to the option.
* Once this function has gathered and cleaned user input, it calls
* the switchboard function to determine the proper action to take.
*
* int *numNewCommands is a pointer to the number of user-added
* 	commands
* Ucommand **addedCommands is a list of user-added command structs
* RunningProcessList *listOfProcesses is the list of processes
* 	currently running in the background.
*/
void menu(int *numNewCommands, Ucommand **addedCommands, RunningProcessList *listOfProcesses){
	int commandChosen = 0;
	char* commandString = malloc(sizeof(char) * LINE_SIZE);

	printf("G'day, Commander. What command would you like to run?\n");
	printf("%4d. %-7s : Prints out the result of the whoami command\n", 0, "whoami");
	printf("%4d. %-7s : Prints out the result of the last command\n", 1, "last");
	printf("%4d. %-7s : Prints out the result of a listing on the user-specified path\n", 2, "ls");

	for(int i = 0; i < *numNewCommands; i++){
		printf("%4d. %-7s : User added command\n", 3+i, addedCommands[i]->fullName);
	}

	printf("%4c. %s : Adds a new command to the menu.\n", 'a', "add command");
	printf("%4c. %s : Changes process working directory\n", 'c', "change directory");
	printf("%4c. %s : Leave mid-day commander\n", 'e', "exit");
	printf("%4c. %s : Prints working directory\n", 'p', "pwd");
	printf("%4c. %s : Print list of running processes\n", 'r', "running processes");
	
	//Loops until plausibly valid input is given
	while(!commandChosen){
		printf("Option? ");
		if(!fgets(commandString, LINE_SIZE, stdin)){
			strncat(commandString, "e", 2);
		}
		//This condition checks for the end of the file
		if(commandString[0] != '\n'){
			commandString = strtok(commandString, " \n\t");
			if(strtok(NULL, " \n\t") != 0)
				printf("Error: Please only enter one token per command.\n");
			else
				commandChosen = 1;
		}
		else
			printf("Error: Please enter a command.\n");
	}
	deleteNewLine(commandString);
	switchBoard(commandString, numNewCommands, addedCommands, listOfProcesses);
}

/*
* This function takes the user input from the menu function
* and determines the proper action to take. This function
* may handle the input itself or hand off the action to
* prepForkCommand, runCommand, runBackgroundCommand, 
* addCommand, or changeDirectory.
*
* char *commandString is the user-entered string
* int *numNewCommands is a pointer to the number of user-added
* 	commands
* Ucommand **addedCommands is a list of user-added command structs
* RunningProcessList *listOfProcesses is the list of processes
* 	currently running in the background.
*/
void switchBoard(char* commandString, int *numNewCommands, Ucommand **addedCommands, RunningProcessList *listOfProcesses){
	char* newCommandName = malloc(sizeof(char) * LINE_SIZE);
	//int correctInput = 0;


	//int realCommandNumber = 0;
	//char* realString = malloc(sizeof(char) * LINE_SIZE);
	if(atoi(commandString) > 2 && atoi(commandString) < *numNewCommands + 3){
		printf("\n--Command: %s --\n", addedCommands[atoi(commandString)-3]->fullName);
		//Check if the process is meant to be run in the background
		if(addedCommands[atoi(commandString)-3]->background){
			runBackgroundCommand(addedCommands[atoi(commandString)-3]->args[0], addedCommands[atoi(commandString)-3]->args, listOfProcesses);
		}
		else{
			runCommand(addedCommands[atoi(commandString)-3]->args[0], addedCommands[atoi(commandString)-3]->args);
		}
		
	}
	else if(strlen(commandString) == 1){
		switch(commandString[0]){
			case('a'):
				printf("\n-- Add a command --\nCommand to add? ");
				fgets(newCommandName, LINE_SIZE, stdin);
				if(newCommandName[0] != '\n'){
					deleteNewLine(newCommandName);
					addCommand(newCommandName, numNewCommands, addedCommands);
				}
				break;
			case('c'):
				printf("\n-- Change Directory --\nNew Directory?: ");
				//New Command Name is reused to keep resources concise
				fgets(newCommandName, LINE_SIZE, stdin);
				deleteNewLine(newCommandName);
				changeDirectory(newCommandName);
				break;
			case('e'):
				printf("Logging you out, Commander.\n");
				//setting this value negative signals to main to stop looping
				//through the menu
				*numNewCommands = -1;
				break;
			case('p'):
				prepForkCommand("pwd", "\n-- Current Directory --\n", NULL);
				break;
			case('0'):
				prepForkCommand("whoami", "\n-- Who Am I? --\n", NULL);
				break;
			case('1'):
				prepForkCommand("last", "\n-- Last Logins --\n", NULL);
				break;
			case('2'):
				prepForkCommand("ls", "\n-- Directory listing --\n", "Path?");
				break;
			case('r'):
				displayProcesses(listOfProcesses);
				break;
			default:
				printf("Error: illegal command. Please enter a specified command.\n");
				break;
		}
	}
	else{
		printf("Error: illegal command. Please enter a specified command.\n");
	}

	//free(newCommandName);
	//free(realString);
}

/*
* This function is respoinsible for adding a command to the list
* of commands in the menu. It handles removing the & from commands
* where it symbolizes background execution and other aspects of
* the commands.
*
* newCommands is a user-entered new command
* *numNewCommands is a pointer to the number of user-added
* 	commands
* Ucommand **addedCommands is a list of user-added command structs
*/
void addCommand(char* newCommand, int *numNewCommands, Ucommand **addedCommands){
	if(*numNewCommands < MAX_COMMANDS){
		//The MAX_COMMANDS constant is necessary for the array format
		//where the commands are stored
		addedCommands[*numNewCommands] = malloc(sizeof(Ucommand));
		addedCommands[*numNewCommands]->args = malloc(sizeof(char*) * MAX_ARGS);
		addedCommands[*numNewCommands]->fullName= strndup(newCommand, LINE_SIZE);
		for(int i = 0; i < MAX_ARGS; i++){
			addedCommands[*numNewCommands]->args[i] = NULL;
		}
		//The line below gets the number of commands and fills the args
		//array with the command's arguments
		addedCommands[*numNewCommands]->numArgs = parseArgs(newCommand, addedCommands[*numNewCommands]->args);
		if(addedCommands[*numNewCommands]->numArgs > 1 && 
				!strcmp(addedCommands[*numNewCommands]->args[addedCommands[*numNewCommands]->numArgs-1], "&")){
			/*
			The condition above checks whether or not the last token 
			of the entered command is an ampersand. If so, the code
			below removes the ampersand and replaces the poiner in 
			the array of strings with a null pointer.
			*/
			addedCommands[*numNewCommands]->args[addedCommands[*numNewCommands]->numArgs-1] = NULL;
			//Cease counting the no-longer-existant ampersand as an argument
			addedCommands[*numNewCommands]->numArgs--;
			//Indicate that the process is meant to run in the background
			addedCommands[*numNewCommands]->background = 1;
		}
		else{
			addedCommands[*numNewCommands]->background = 0;
		}
		(*numNewCommands)++;

		printf("Okay! Added with ID %d!\n\n", *numNewCommands + 2);
	}
	else{
		printf("Error: Exceeded maximum number of commands.");
	}

}

/*
* This function changes the directory of the running process
* 
* char *path is the user-entered filepath to which to navigate
*/
void changeDirectory(char* path){
	if(chdir(path) == -1)
		printf("Directory change unsuccessful. Error %i\n", errno);
	printf("\n");
}

/*
* This function prepares a command that will run in a separate process
* from this one but for which this process will wait. These preparations
* include gathering any needed user input and preparing an array of strings
* of arguments.
*
* char* name is the name of the command to execute
* char *message is the message to print before the command executes
* char *argQuestion is the question to ask for the last argument. If no
* 	additional arguments are needed, argQuestion will be null.
*/
void prepForkCommand(char* name, char* message, char *argQuestion){
	//Variable for no additional arguments
	char *basicArgs[2] = {name, NULL};

	//Variables for commands with additional arguments
	int numArgs = 0;
	char **advArgs = (char**) malloc((sizeof(char*) * MAX_ARGS)+3);
	char *theArgs = (char*) calloc(sizeof(char), LINE_SIZE);
	char *lastArg = (char*) malloc(sizeof(char) * LINE_SIZE);

	printf("%s", message);

	//This condition checks whether or not the command requires additional
	//arguments
	if(argQuestion != NULL){
		/*
		Initialize the argument array to null so that the pointer after
		the last argument will always be null.
		*/
		for(int i = 1; i < MAX_ARGS+3; i++){
			advArgs[i] = NULL;
		}
		advArgs[0] = name;

		printf("Arguments? ");
		fgets(theArgs, LINE_SIZE, stdin);
		printf("%s", argQuestion);
		fgets(lastArg, LINE_SIZE, stdin);

		//Checks whether or not the user entered any arguments
		if(theArgs[0] != '\n')
			//The line below copies the arguments in theArgs to advArgs
			//in addition to getting the number of arguments
			numArgs = parseArgs(theArgs, advArgs + 1);
		else
			numArgs = 0;

		//Checks whether or not the user enters anything for the last argument
		if(lastArg[0] != '\n'){
			//The line below is needed because deleteNewLine removes a newline
			//at the end of the string
			deleteNewLine(lastArg);
			//Insert lastArg into the array of arguments
			advArgs[numArgs+1] = lastArg;
		}

		runCommand(name, advArgs);
	}
	else{
		runCommand(name, basicArgs);
	}

}

/*
* runCommand runs the specified command and gathers statistics on the
* command's running time and page faults.
*
* char *name is the command's name
* char **arguments is a list of arguments to feed to execvp
* 	the first argument will be the command's name
* 	the last argument will be a null pointer
*/
void runCommand(char* name, char** arguments){
	int isParent;
	long int secondsElapsed;
	long int uSecondsElapsed;

	struct timeval *startTime = malloc(sizeof(struct timeval));
	struct timeval *endTime = malloc(sizeof(struct timeval));
	//The variable below is for proper use of the getrusage function
	struct timezone *whocares = malloc(sizeof(struct timeval));

	time_t startSeconds;
	suseconds_t startuSeconds;
	time_t endSeconds;
	suseconds_t enduSeconds;

	struct rusage *startUsage = malloc(sizeof(struct rusage));
	struct rusage *endUsage = malloc(sizeof(struct rusage));

	//This code gets the data from before the command is run
	gettimeofday(startTime, whocares);
	startSeconds = startTime->tv_sec;
	startuSeconds = startTime->tv_usec;
	getrusage(RUSAGE_SELF, startUsage);

	isParent = fork();

	//This conditional has the child process run while the parent waits
	if(isParent){
		waitpid(isParent, NULL, 0);
	}
	else{
		execvp(name, arguments);
	}

	//This code gets the data from after the command is run
	gettimeofday(endTime, whocares);
	endSeconds = endTime->tv_sec;
	enduSeconds = endTime->tv_usec;
	getrusage(RUSAGE_SELF, endUsage);

	secondsElapsed = endSeconds - startSeconds;
	uSecondsElapsed = enduSeconds - startuSeconds;
	//This conditional carries a second to microseconds if needed
	if(uSecondsElapsed < 0 && secondsElapsed > 0){
		secondsElapsed--;
		uSecondsElapsed += 1000000;
	}

	printf("\n-- Statistics ---\nElapsed time: ");
	if(secondsElapsed){
		printf("%li seconds and ", secondsElapsed);
	}
	printf("%li milliseconds.\n", uSecondsElapsed);
	printf("Page Faults: %li\n", (endUsage->ru_majflt - startUsage->ru_majflt));
	printf("Page Faults (reclaimed): %li\n\n", (endUsage->ru_minflt - startUsage->ru_minflt));

	free(startTime);
	free(endTime);
	free(whocares);
	free(startUsage);
	free(endUsage);
}

/*
* runBackgroundCommand runs the specified command in a child process
* while allowing the main process to continue running. This function
* also gathers statistics on the command's running time and page faults.
*
* char *name is the command's name
* char **arguments is a list of arguments to feed to execvp
* 	the first argument will be the command's name
* 	the last argument will be a null pointer
* RunningProcessList *listOfProcesses is a list of processes that are running
* 	in the background of the main process.
*/
void runBackgroundCommand(char* name, char** arguments, RunningProcessList *listOfProcesses){
	int taskNumber = ++(listOfProcesses->numProcesses);
	int isParent;
	int isChildParent;
	listOfProcesses->list[taskNumber] = malloc(sizeof(RunningProcess));
	long int secondsElapsed;
	long int uSecondsElapsed;

	struct timeval *startTime = malloc(sizeof(struct timeval));
	struct timeval *endTime = malloc(sizeof(struct timeval));
	//The variable below is for proper use of the getrusage function
	struct timezone *whocares = malloc(sizeof(struct timeval));

	time_t startSeconds;
	suseconds_t startuSeconds;
	time_t endSeconds;
	suseconds_t enduSeconds;

	struct rusage *startUsage = malloc(sizeof(struct rusage));
	struct rusage *endUsage = malloc(sizeof(struct rusage));

	//This code gets the data from before the command is run
	gettimeofday(startTime, whocares);
	startSeconds = startTime->tv_sec;
	startuSeconds = startTime->tv_usec;
	getrusage(RUSAGE_SELF, startUsage);
	
	isParent = fork();

	//This code will only execute for the child process
	if(!isParent){
		isChildParent = fork();
		//The condition calls exec on the grandchild will running
		//the child
		if(!isChildParent){
			execvp(name, arguments);
		}
		else{
			/*
			Print the grandchild process's pid
			This is meant to be under the command headding,
			But the scheduler may have other plans
			*/
			printf("[%i] %i\n", taskNumber, isChildParent);
			listOfProcesses->list[taskNumber-1]->pid = isChildParent;
			listOfProcesses->list[taskNumber-1]->finished = 0;

			waitpid(isChildParent, NULL, 0);

			//This code gets the data from after the command is run
			gettimeofday(endTime, whocares);
			endSeconds = endTime->tv_sec;
			enduSeconds = endTime->tv_usec;
			getrusage(RUSAGE_SELF, endUsage);

			secondsElapsed = endSeconds - startSeconds;
			uSecondsElapsed = enduSeconds - startuSeconds;
			//This conditional carries a second to microseconds if needed
			if(uSecondsElapsed < 0 && secondsElapsed > 0){
				secondsElapsed--;
				uSecondsElapsed += 1000000;
			}

			printf("-- Job Complete [%d] --\n", taskNumber);
			printf("Process ID: %d\n", isChildParent);
			//printf("[ Output ]\n");

			printf("\n-- Statistics ---\nElapsed time: ");
			if(secondsElapsed){
				printf("%li seconds and ", secondsElapsed);
			}
			printf("%li milliseconds.\n", uSecondsElapsed/1000);
			printf("Page Faults: %li\n", (endUsage->ru_majflt - startUsage->ru_majflt));
			printf("Page Faults (reclaimed): %li\n\n", (endUsage->ru_minflt - startUsage->ru_minflt));

			//Indicate to the main process that the grandchild process has finished
			listOfProcesses->list[taskNumber-1]->finished = 1;
			free(startTime);
			free(endTime);
			free(whocares);
			free(startUsage);
			free(endUsage);
			exit(EXIT_SUCCESS);
		}
	}
}

/*
* This function takes a string and an array of strings, cuts
* up the string into tokens (by replacing white space with null
* terminators) and returns the number of tokens found. This
* function also fills argsDest wih a pointer to each of the
* tokens
*
* char *argsSource is the source string to tokenize and modify
* char **argsDest is the destination array of strings to fill
* 	with pointers to the tokens. This function assumes that the
* 	array is large enough to hold the tokens in the string.
*/
int parseArgs(char* argsSource, char** argsDest){
	char* nextArg = strtok(argsSource, " \n\t");
	int numArgs = 0;
	if(nextArg != NULL){
		numArgs++;
		argsDest[0] = nextArg;
		while((nextArg = strtok(NULL, " \n\t")) != NULL){
			argsDest[numArgs++] = nextArg;
		}
	}

	return numArgs;
}

/*
* This function takes a string and replaces its last character
* with a null terminator if the string's last character is a
* new line
*
* char *string is the string to potentially modify
*/
void deleteNewLine(char* string){
	if(string[strlen(string)-1] == '\n')
		string[strlen(string)-1] = '\0'; 
}

/*
* This function displays the processes that are currently running
* in the background of the main process.
*
* RunningProcessList *listOfProcesses is the list of processes
* 	that are running in the background of the main process.
*/
void displayProcesses(RunningProcessList *listOfProcesses){
	printf("-- Background Processes --\n");
	for(int i = 0; i < listOfProcesses->numProcesses; i++){
		//Checkes whether or not each process in the array is finished.
		if(!listOfProcesses->list[i]->finished)
			printf("[%i] %i\n", i, listOfProcesses->list[i]->pid);
	}
}