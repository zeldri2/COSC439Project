#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>
#include <asm/unistd.h>
#include <asm/paravirt.h>

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

asmlinkage int (*original_open)(int dirfd, const char *pathname, int flags); //This is the original read function, 
asmlinkage int HookOpen(int dirfd, const char *pathname, int flags) { //Our version of the Open Function. The const char *pathname is used to get each individual letter from the path name, store it in directory variable. 
        char letter;
        int i = 0;
        char directory[255];
        char OurFile[14] = "breakpoints"; //file we will be looking for

        while(letter != 0 || i < 6) { //If (letter == 0x41 || letter < 0x7a) to prevent bad chars from entering our string buffer
                //This macro copies a single variable from user space to kernel mode
                //This will copy pathname[i]
                get_user(letter, pathname+i);
                directory[i] = letter;
                i++;
        }

        if(strcmp(OurFile, directory) == 0) { //strcmp is string compare, and compare it with out file breakpoints
                printk(KERN_INFO "File Accessed!!! %s", directory);
        }
        memset(directory, 0, 255);

        // Jump to original OpenAt()
        return (*original_open)(dirfd, pathname, flags);
}

//Set up Hooks
static int __init SetHooks(void) {
        //Gets Syscall Table **
        SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table"); //Get a pointer to the array of the functions

        printk(KERN_INFO "Hooks Will Be Set. \n");
        printk(KERN_INFO "System call table at %p\n", SYS_CALL_TABLE);

        //Opens the memory pages to be written. Enables us to overwrite the syscall that we need to.
        EnablePageWriting();

        //Replaces Pointer of Syscall_open on our syscall
        original_open = (void*)SYS_CALL_TABLE[__NR_openat]; //openat syscall being replaced with out syscall. 
        SYS_CALL_TABLE[__NR_openat] = (unsigned long*)HookOpen; //Original read syscall equal to our hook read. This is used to replace the function.
        DisablePageWriting(); //Make our pages safe again

        return 0;
}

//When we unload the module, we will call HookCleanup 
static void __exit HookCleanup(void) {
        // Clean up our hooks/
        EnablePageWriting();
        SYS_CALL_TABLE[__NR_openat] = (unsigned long*)original_open; //Call this to fix our read, put original function back where it needs too
        DisablePageWriting(); //Make page writing secure again

        printk(KERN_INFO "HooksCleaned Up!");

}

module_init(SetHooks);
module_exit(HookCleanup);
