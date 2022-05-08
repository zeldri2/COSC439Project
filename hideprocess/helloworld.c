#include <stdio.h>
#include <time.h>
#include <unistd.h>

int count = 0;
int main() {
	count++;
        	while(1) {	
		printf("Tick Tock -- Cycle %d \n", count);
		usleep(100000);
	}
}




