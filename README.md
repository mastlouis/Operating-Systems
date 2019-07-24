# Operating-Systems
This repository contains the assignments I completed for my Operating Systems course. The four Projects I completed were as follows:
- [Loadable Kernel Module](#Project-0:-Loadable-Kernel-Module)
- [Midday Commander](#Project-1:-Midday-Commander)
  - [Foundation](#Version-0:-Foundation)
  - [Extendable Commander](#Version-1:-Extendable-Commander)
  - [Asynchronous Commander](#Version-2:-Asynchronous-Commander)

## [Project 0](https://github.com/mastlouis/Operating-Systems/tree/master/p0): Loadable Kernel Module

Project 0 involved modifying the operating system of a linux virtual machine. I added a few system calls to the table of system calls, then called them with a program written in C.

## [Project 1](https://github.com/mastlouis/Operating-Systems/tree/master/p1): Midday Commander

Midday Commander is a parody of the command line interface midnight commander. It's an exercise in managing thread and processes to manage asynchronous programming. There are a few versions of the Midday Commander, each of which builds on the last. To build the all versions of the Midday commander simultaneously, run the [Makefile](https://github.com/mastlouis/Operating-Systems/blob/master/p1/Makefile).

### [Version 0](https://github.com/mastlouis/Operating-Systems/blob/master/p1/mc0.c): Foundation

This version of the Midday Commander is a basic foundation. The only valid commands are whoami, last, and ls (where the user has the option to specify a file path).

Test cases for Version 0 and their output can be found in the [testv0](https://github.com/mastlouis/Operating-Systems/tree/master/p1/testv0) folder.

### [Version 1](https://github.com/mastlouis/Operating-Systems/blob/master/p1/mc1.c): Extendable Commander

This version of the Midday Commander allows the user to add their own commands. The user can only add exising bash commands. The user may also add commands with a set of flags (e.g. ls -la). The user is also provided with options to print working directory, change directory, and exit the commander.

Test cases for Version 1 and their output can be found in the [testv1](https://github.com/mastlouis/Operating-Systems/tree/master/p1/testv1) folder.

### [Version 2](https://github.com/mastlouis/Operating-Systems/blob/master/p1/mc2.c): Asynchronous Commander

This version of Midday Commander allows the user to run background commands. This allows the user to continue entering commands while other commands are running. The user can accomplish this by entering the *&* character after a command. The background command will print information about the command when it finishes executing, and this text may interrupt the Midday Commander.

This program accomplishes asynchronysity by spawning a new process for each background command using *fork* and *exec*. When building this program, I confused threads and processes. Each of a process's threads has its own private stack, but they all share one heap. Processes may copy their parent process's data when created, but they overwrite this data when *exec* is called. Processes do not share a stack or heap. When I designed this program, I remembered to share data in the heap rather than in the stack, but I forgot that threads share a heap, not processes. When the user runs the command to list running processes after at least one background command has run, the program will seg fault and terminate. Asynchronous Commander is otherwise functional.

Test cases for Version 2 and their output can be found in the [testv2](https://github.com/mastlouis/Operating-Systems/tree/master/p1/testv2) folder.
