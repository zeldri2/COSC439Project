#include <linux/net.h>
#include <linux/in.h>
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

#include <linux/in.h>

//#include <math.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/uaccess.h>

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

#define TCP 0x2
#define UDP 0x1

unsigned char IP[32] = {'\0'};

/* In memory, IPs are stored like 01.0.0.127 == 01 00 00 7f */

/* Convert IP from hex into string format to make it human readable */
char * inet_ntoa(int HexValue) {
	memset(IP, 0, sizeof(IP));

	unsigned char first = (HexValue >> 24) & 0xff;
	unsigned char second = (HexValue >> 16) & 0xff;
	unsigned char third = (HexValue >> 8) & 0xff;
	unsigned char fourth = HexValue &  0xff;

	size_t size = sizeof(IP) / sizeof(IP[0]);
	snprintf(IP, size, "%d.%d.%d.%d", fourth, third, second, first);
	
	return IP;
}

/* Original syscall */
asmlinkage int ( *original_Connect ) (int fd, struct sockaddr __user *uservaddr, int addrlen);

/* Our version of the syscall */
asmlinkage int HookConnect(int fd, struct sockaddr __user *uservaddr, int addrlen) { //file descriptor, structure where we cann uservirtualaddress, address length
	struct sockaddr_in addr; //reference to the structure

	/* ALl of the information our funcitons recieve are from user space. Everything we do in our own space and pass on goes directly to the kernel. To interact with parameters we copy from the user and inject it into kernel space */
	copy_from_user(&addr, uservaddr, sizeof(struct sockaddr_in));

	int IPHEX = addr.sin_addr.s_addr;
	unsigned short PORT = addr.sin_port;
	int PROTO = addr.sin_family;

	char *IpString = inet_ntoa(IPHEX);

	if(PROTO==TCP) {
		printk("TCP CONNECTION STARTED -- TO %s PORT 0x%x", IpString, PORT);
	}
	if(PROTO==UDP) {
		printk("UDP CONNECTION STARTED -- TO %s PORT 0x%x", IpString, PORT);
	}

	return ( *original_Connect ) (fd, uservaddr, addrlen);
}

// Set up hooks.
static int __init SetHooks(void) {
	// Gets Syscall Table **
 	SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table");

	printk(KERN_INFO "Hooks Will Be Set.\n");
	printk(KERN_INFO "System call table at %p\n", SYS_CALL_TABLE);

  // Opens the memory pages to be written
	EnablePageWriting();

  // Replaces Pointer Of Syscall_open on our syscall.
	original_Connect = (void*)SYS_CALL_TABLE[__NR_connect];
	SYS_CALL_TABLE[__NR_connect] = (unsigned long*)HookConnect;
	DisablePageWriting();

	return 0;
}

static void __exit HookCleanup(void) {

	// Clean up our Hooks
	EnablePageWriting();
	SYS_CALL_TABLE[__NR_connect] = (unsigned long*)original_Connect;
	DisablePageWriting();
	printk(KERN_INFO "HooksCleaned Up!");
}

module_init(SetHooks);
module_exit(HookCleanup);

