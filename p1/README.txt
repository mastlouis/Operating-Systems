The following is a description of the Midday Commander's
workings and data structures.

Sections:
1 Control Flow
2 Data Structures

********************************
Section 1: Control Flow
********************************

The main function handles looping through the menu and 
handles termination of the program if the user exits
or if the file runs out. Main repeatedly calls a menu
function.

The menu function takes user input and repeatedly rejects
it until the user enters a plausible command (one token with
one or more characters). Once the menu has the command, it 
hands control to the switchboard function.

The switchboard takes the input or hands control to the proper
function.

PrepForkCommand is one function that the switchboard uses. It
handles gathering and organizing arguments and hands them to
runCommand to run the function.

RunCommand handles the actual forking and executing.
RunBackgroundCommand is similar, but for background commands.
A grandchild process runs while the child process tracks data.

********************************
Section 2: Data Structures
********************************

The user commands are stored as an array of structs. The number of
structs in this array are tracked with an int. Each struct holds
the command's full name (ampersand and all), along with an array
of the arguments needed to execute the command and the number
of arguments.

The RunningProcces struct holds the pid of each process that has
run and an int indicating whether or not the process is still running.
The runningProcessList struct holds an array of running process structs
and the number of these elements.
