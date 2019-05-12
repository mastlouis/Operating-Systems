//Matthew St. Louis, Fay Whittall
//mastlouis, fxwhittall
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/list.h>
//#include <asm-generic/uaccess.h>
#include <asm/errno.h>
#include <linux/slab.h>
#include <linux/pid.h>

#define MAX_ANCESTORS (10)
#define MAX_SIBLINGS (100)
#define MAX_CHILDREN (100)

unsigned long **sys_call_table;

struct ancestry{
  pid_t ancestors[MAX_ANCESTORS];
  pid_t siblings[MAX_SIBLINGS];
  pid_t children[MAX_CHILDREN];
};

int getAllAncestors(struct ancestry *theAncestry, struct task_struct *targetProcess);

asmlinkage long (*ref_sys_cs3013_syscall2)(unsigned short* target_pid, struct ancestry* response);

asmlinkage long new_sys_cs3013_syscall2(unsigned short* target_pid, struct ancestry* response) {
	struct ancestry theAncestry;
  unsigned short thePid = 0;
  struct task_struct processI = *current;
  struct task_struct targetProcess = *current;

  printk("Orig pid address: %p\n", target_pid);
  printk("Original pid: %i\n", *target_pid);

  //theAncestry = (struct ancestry*) kcalloc(1, sizeof(struct ancestry), GFP_KERNEL);
  //thePid = (unsigned short*) kcalloc(1, sizeof(unsigned short), GFP_KERNEL);
  copy_from_user(&theAncestry, response, sizeof(struct ancestry));
  copy_from_user(&thePid, target_pid, sizeof(thePid));
  printk("*******************************************************Kernel pid %d\n", thePid);
  //targetProcess = (struct task_struct *) kcalloc(1, sizeof(struct task_struct), GFP_KERNEL);
  //processI = (struct task_struct *) kcalloc(1, sizeof(struct task_struct), GFP_KERNEL);

  printk("The thing started\n");
  //processI = current;

  for_each_process(processI){
    printk("The pid is: %i", (int) processI->pid);
     if((int) processI->pid == (int) thePid){
       printk("Process found\n");
       targetProcess = processI;
       printk("Process set\n");
     }
  }

  printk("%d, %d\n", (int)targetProcess->pid, (int)thePid);

  if((int)targetProcess->pid != (int)thePid){
    printk("Error! Cannot find the process.\n");
    copy_to_user(response, &theAncestry, sizeof(struct ancestry));
    copy_to_user(target_pid, &thePid, sizeof(unsigned short));

    /*
    kfree(processI);
    kfree(thePid);
    kfree(theAncestry);
    kfree(targetProcess);
    */

    return -1;
  }
  else{
      getAllAncestors(&theAncestry, &targetProcess);
      //getAllChildren(&theAncestry, &targetProcess);
      //getAllSiblings(&theAncestry, &targetProcess);
  }

  copy_to_user(response, &theAncestry, sizeof(struct ancestry));
  copy_to_user(target_pid, &thePid, sizeof(unsigned short));

/*
  kfree(processI);
  kfree(thePid);
  kfree(theAncestry);
  kfree(targetProcess);
  */

  return 0;
}

int getAllAncestors(struct ancestry *theAncestry, struct task_struct *targetProcess){
  int isNull, hasCircled, numCounted;
  struct task_struct processI = targetProcess;

  printk("Obtaining ancestors\n");
  //processI = targetProcess;
  isNull = 0;
  hasCircled = 0;
  numCounted = 0;

  //Should we be asking for the parent pointer or the real_parent pointer?
  while(!isNull && !hasCircled && numCounted < MAX_ANCESTORS){
    printk("Entered loop");
    if(processI->parent == 0){
      printk("We done bois");
      isNull = 1;
    }
    if(processI->parent->pid >= processI->pid){
      printk("Time travel is real");
      hasCircled = 1;
    }
    else{
      printk("No, I am your father");
      theAncestry->ancestors[numCounted] = processI->pid;
      printk("Parent PID: %i", (int) processI->pid);
      processI = processI->parent;
      numCounted++;
    }
  }
  return numCounted;

}

int getAllChildren(struct ancestry *theAncestry, struct task_struct *targetProcess){

  //struct task_struct processI;
  struct task_struct *processJ;
  struct list_head kiddos;
  //int isContinue;
  int childIndex;

  //processI = current;
  kiddos = targetProcess->children;
  //processJ =
  childIndex = 0;

  //while(isContinue && childIndex < MAX_CHILDREN){
    list_for_each_entry(processJ, &(kiddos), sibling){
      if(childIndex < MAX_CHILDREN)

        theAncestry->children[childIndex] = processJ->pid;
      printk("Child PID: %i", (int) processJ->pid);
      childIndex++;
    }
    return childIndex;

}

int getAllSiblings(struct ancestry *theAncestry, struct task_struct *targetProcess){

  //struct task_struct processI;
  struct task_struct *processJ;
  struct list_head pledges;
  //int isContinue;
  int childIndex;

  //processI = current;
  pledges = targetProcess->sibling;
  //processJ =
  childIndex = 0;

  //Use copy from user

  //while(isContinue && childIndex < MAX_CHILDREN){
  list_for_each_entry(processJ, &(pledges), sibling){
    if(childIndex < MAX_CHILDREN)
      theAncestry->children[childIndex] = processJ->pid;
    printk("PID: %i", (int) processJ->pid);
    childIndex++;
  }
  return childIndex;

}

// void getAllAncestors(struct ancestry *theAncestry){
//   int isNull, hasCircled, numCounted;
//   struct task_struct *processI;

//   processI = current->children;
//   if(processI != NULL)

//   isNull = 0;
//   hasCircled = 0;
//   numCounted = 0;

//   if(processI)

//   while(!isNull && !hasCircled && numCounted < MAX_ANCESTORS){

//     if(processI->parent == 0){
//       isNull = 1;
//     }
//     if(processI->parent->pid <= processI->pid){
//       hasCircled = 1;
//     }
//     else{
//       theAncestry->ancestors[numCounted] = processI->pid;
//       numCounted++;
//     }
//   }

// }

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;

  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
	     (unsigned long) sct);
      return sct;
    }

    offset += sizeof(void *);
  }

  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
  */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work.
       Cancel the module loading step. */
    return -1;
  }

  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];
  //ref_sys_open = (void *)sys_call_table[__NR_open];
  //ref_sys_close = (void *)sys_call_table[__NR_close];
  //ref_sys_read = (void *)sys_call_table[__NR_read];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  //sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
  //sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
  //sys_call_table[__NR_read] = (unsigned long *)new_sys_read;

  enable_page_protection();

  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;

  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  //sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
  //sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
  //sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
