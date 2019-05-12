//Matthew St. Louis, Fay Whittall
//mastlouis, fxwhittall
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
#define __NR_cs3013_syscall3 379

#define MAX_ANCESTORS (10)
#define MAX_SIBLINGS (100)
#define MAX_CHILDREN (100)

struct ancestry{
  pid_t ancestors[MAX_ANCESTORS];
  pid_t siblings[MAX_SIBLINGS];
  pid_t children[MAX_CHILDREN];
};

long testCall2 (unsigned short* target_pid, struct ancestry* response) {
	return (long) syscall(__NR_cs3013_syscall2);
}

int main (int argc, char** argv) {
	unsigned short target_pid;
	struct ancestry *response;

	if(argc > 1){
		printf("Arugment received: %s\n", argv[1]);
		target_pid = (unsigned short)atoi(argv[1]);
	}
	else{
		printf("Argument is wrong");
		return 1;
	}

	if(target_pid < 1){
		printf("Your pid is wrong, and you should feel bad.\n");
		return 1;
	}

	printf("Hasn't died yet.\n");
//	target_pid = getpid();

//	printf("New Target pid: %i\n", target_pid);
  printf("The pid entered %p\n", &target_pid);

	response = malloc(sizeof(struct ancestry));
	//printf("The return values of the system calls are:\n");
	//printf("\tcs3013_syscall1: %ld\n", testCall1());
	printf("\tcs3013_syscall2: %ld\n", testCall2(&target_pid,response));
	//printf("\tcs3013_syscall3: %ld\n", testCall3());
	return 0;


}
