#include "functs.h"

static void HelloExit(void) {
   printk(KERN_INFO "ROOTKIT_DEBUG: GOODBYE WORLD \n");
}

//Calls the functions
module_exit(HelloExit);
