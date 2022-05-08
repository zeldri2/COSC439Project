#include "functs.h" //Call in our function prototypes 


#define DRIVER_AUTHOR "Zac and Group 3"
#define DRIVER_DESC "Hello World Driver"
MODULE_AUTHOR("Zachary Eldridge");
MODULE_DESCRIPTION("Simple Hooking of a Syscall");
MODULE_LICENSE("GPL"); //Prevents no license warning upon compilation
MODULE_VERSION("1.0");
MODULE_SUPPORTED_DEVICE("testdevice") //devices are anything thats mapped or located to dev. When you have hardware that says something like /dev/sda etc

int Major; //Device number that the driver device will talk too
static int Device_Open = 0; //We will use this to check if the device is open or not. Will be either 1 or 0 so if its open we can set it to close when we are done
static char msg[BUF_Len]; 
static char *msg_Ptr;

static struct file_operations fops = {
	//declare variables to the functions we defined in functs.h so we can give them values
	.read = device_read,
	.write = device_write,
	.open = device_open,
        .release = device_release
};


int init_module(void) { //check if a device has been registered properly and print if its successful or not. Tells how to create a device to talk to our driver

	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if(Major < 0) { //If failed to load the driver print the below 
		printk(KERN_ALERT "I have failed to load!\n");
		return Major;
	}
        //If does not fail just continue 
	printk(KERN_ALERT "I was assigned a major number %d", Major);
	printk(KERN_ALERT "Please create a device with name \n mknod /dev/%s c %d 0 \n", DEVICE_NAME, Major); //When we create our device and try to interact with it, the driver will wait for input for a read/write operation. mknod is how you create a device. It will grab our device name of root_kit
	return 0;
}

//Open Driver Function, ran when syscall_open is called, it run this function
int device_open(struct inode * inode, struct file *file) {
	//static int counter = 0;
	if(Device_Open) { //if the device is open return that the device is busy
		return -EBUSY;
	}

	//Copy a message into a buffer that will be printed to the screen
	//sprintf(msg, "I was opened %d times", counter++);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);
	return 0;
}

/* Called when a process closes the device file */
int device_release(struct inode * inode, struct file *file) { //ensures when device is done reading then it will release the device so we can use it again. Similar to close() 
	Device_Open--; //Make it ready to the user next caller
	module_put(THIS_MODULE);
	return 0;
}

/* Called when a process, which already opened the dev file, attempts to
   read from it. Function runs when syscall_read is ran
*/
ssize_t device_read(struct file * file, char * buffer, size_t length, loff_t *offset) {
	int bytes_read = 0;
	if(*msg_Ptr == 0) { //if the message is nothing, do nothing and return nothing
		return 0;
	}

	while(length && *msg_Ptr) {
		put_user(* (msg_Ptr++), buffer++ );
		length--;
		bytes_read++;
	}

	return bytes_read;

}

ssize_t device_write(struct file * file, const char * buffer, size_t length, loff_t *offset) {
	int count = 0;
	memset(msg, 0, BUF_Len); //clear our buffer then use msg as the buffer which is whats getting read to, then set it to the size of that buffer using buffer length

	while(length > 0) {
		copy_from_user(msg, buffer, BUF_Len-1); //copy to the msg buffer and use buffer length - 1 so we don't overflow
		length--;
		msg[BUF_Len-1] = 0x00;
	}

	return count; /*Always return something!*/
}
