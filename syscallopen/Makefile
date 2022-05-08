ccflags-y = -std=gnu99
obj-m += rootkit.o #Create the output file
rootkit-objs := main.o  #The rootkit.o object will conists of these following object files as well. 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules #Creates all the required files we need. Allows us to use linux modules in init.h

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean #Clean the modules
