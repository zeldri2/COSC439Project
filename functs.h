#include <linux/module.h>
#include <linux/kernel.h> // Debug Messages like KERN_INFO
#include <linux/init.h> //gives us access to macros
#include <linux/moduleparam.h> //gives us access to the parameter functions 
#include <linux/stat.h>
#include <linux/fs.h> //filesystem
#include <linux/uaccess.h>

int init_module(void); //This function 
int device_open(struct inode * inode, struct file *file);
int device_release(struct inode * inode, struct file *file);
ssize_t device_read(struct file * file, char * buffer, size_t length, loff_t *offset);
ssize_t device_write(struct file * file, const char * buffer, size_t length, loff_t *offset);

#define SUCCESS 0
#define DEVICE_NAME "rootkit"
#define BUF_Len 80
extern int Major;
