#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define __NR_my_syscall 359

int main()
{
	// declare return value for system call
	int ret = 0;
	// declare buffer to hold result of system call
	char * buffer;
	// set the length of the buffer
	int n = 512;
	
	int exit = 0;
	int count = 0;

	// allocate memory for buffer to hold result
	if( ( buffer = (char *) malloc(n*sizeof(char)+1) ) == NULL ) 
		perror( "Error allocating buffer\n" );

	// print column names
	printf("  PID TTY          TIME CMD\n");

	while(exit == 0){
		// perform system call, storing return value in ret
		// result of system call will be stored in buffer
		ret = syscall(__NR_my_syscall, count, n, buffer);

		if (buffer[0] == '\0') {
			exit = 1;
		}
		else {
			printf("%s", buffer);
			count++;
		}
	}
	
	// de-allocate the memory allocated for the buffer
	free(buffer);

	// return 0 upon success
	return 0;
}

