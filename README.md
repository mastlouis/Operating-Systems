# Operating-Systems
This repository contains the assignments I completed for my Operating Systems course. The four Projects I completed were as follows:
- [Operating-Systems](#operating-systems)
  - [Project 0: Loadable Kernel Module](#project-0-loadable-kernel-module)
  - [Project 1: Midday Commander](#project-1-midday-commander)
    - [Version 0: Foundation](#version-0-foundation)
    - [Version 1: Extendable Commander](#version-1-extendable-commander)
    - [Version 2: Asynchronous Commander](#version-2-asynchronous-commander)
  - [Project 2: Ancestry and System Calls](#project-2-ancestry-and-system-calls)
    - [procAncestry.c](#procancestryc)
    - [cs3013_project2.c](#cs3013project2c)
    - [theCleanOne.c](#thecleanonec)
  - [Project 3: Locks and Threads](#project-3-locks-and-threads)

## [Project 0](https://github.com/mastlouis/Operating-Systems/tree/master/p0): Loadable Kernel Module

Project 0 involved modifying the operating system of a linux virtual machine. I added a few system calls to the table of system calls, then called them with a program written in C.

The testcalls executable tests the system calls, resulting in textual output.

**Warning**: Please do not execute any of these files, unless you are using a virtual machine. They were designed for 32 bit linux, and they tamper with the operating system. Executing may damage your operating system.

## [Project 1](https://github.com/mastlouis/Operating-Systems/tree/master/p1): Midday Commander

Midday Commander is a parody of the command line interface Midnight Commander. It's an exercise in managing thread and processes to manage asynchronous programming. There are a few versions of the Midday Commander, each of which builds on the last. To build the all versions of the Midday commander simultaneously, run the [Makefile](https://github.com/mastlouis/Operating-Systems/blob/master/p1/Makefile).

**Warning**: Since this program can execute near-arbitrary code on the command line, *users should execute on virtual machines only*.

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

## [Project 2](https://github.com/mastlouis/Operating-Systems/tree/master/p2): Ancestry and System Calls

Project 2 has two parts: procAncestry.c and cs3013_project2.c. The confusing naming is a result of submission criteria. procAncestry's goal is to get the ancestry and siblings of a given process. procAncestry.c makes the procAncestry executable, and this executable uses the kernel code from cs3013_project2.c and its derivatives to work. There is also theCleanOne.c, which tests the system call intercepts.

This project's code is by far the messiest of any of those in this workspace because this assignment was by far the most abstract and complicated. It took my homework partner and I many hours of reading, playing around with code, and talking to our professor to understand.

**Warning**: Do not attempt to make, run, or load any part of this project. Doing so may damage your operating system.

### [procAncestry.c](https://github.com/mastlouis/Operating-Systems/blob/master/p2/exe/procAncestry.c)

procAncestry takes one argument: the pid of the target process. If the program can successfully locate the process, it will print a list of the process's ancestor, child, and sibling processes. This program uses cs3013_project2.c and its derivatives to handle the kernel side of this process. procAncestry.c is the executable that runs on the user side of this process.

### [cs3013_project2.c](https://github.com/mastlouis/Operating-Systems/blob/master/p2/cs3013_project2.c)

cs3013_project2.c and its derivatives hangdle the kernel-side legwork of procAncestry. The kernel side involves using linux's efficient but confusing doubly-liked lists that can retrieve information about the object to which they are a property to iteratively find the siblings and children of the target process. This was by far the hardest part of the project.

### [theCleanOne.c](https://github.com/mastlouis/Operating-Systems/blob/master/p2/theCleanOne.c)

theCleanOne.c is a different version of the testcalls.c file. It makes a version of testcalls that prints to the kernel's log whenever a user opens a file. The file names are a bit of a mess because the Makefile was getting complicated toward the end of this project.

## [Project 3](https://github.com/mastlouis/Operating-Systems/tree/master/p3): Locks and Threads

