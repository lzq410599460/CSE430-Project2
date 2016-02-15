#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define __NR_my_syscall 359

int main()
{
	// declare return value for system call - holds process count
	int ret = 0;
	// declare buffer to hold result of system call
	char * buffer;
	// set the length of the buffer
	int n = 512;
	
	int exit = 0;  // flag for exit condition of while loop
	int countb4 = 0;  // count of processes before syscall
	int countafter = 0; // count of processes after syscall
	int j = 0;  // iteration index for re-initializing buffer

	// allocate memory for buffer to hold result
	if( ( buffer = (char *) malloc(n*sizeof(char)+1) ) == NULL ) 
		perror( "Error allocating buffer\n" );

	// debug statement
	// print the size of the buffer
	// printf("buffer size = %d\n", n);

	// print the first line of output - just like 'ps -e'
	printf("  PID TTY          TIME CMD\n");

	// use while loop to iterate through all of the processes
	//  exit after we've run thru all processes
	while (exit == 0) {
		// reset buffer content
		for (j=0; j < n+1; j++) {
			buffer[j] = '\0';
		}

		// store countb4
		countb4 = ret;

		// perform system call, storing return value in ret
		// result of system call will be stored in buffer
		ret = syscall(__NR_my_syscall, ret, n, buffer);
		
		// store countafter
		countafter = ret;

		// compare countb4 with countafter
		// if different, then we have added more content to buffer
		//  which needs to be printed
		// if same, then time to stop - iterated thru all processes
		if (countb4 == countafter) {
			// debug statement
			// printf("stopped because count did not increment\n");
			exit = 1;   // exit the while loop
		}
		else {
			// print the buffer - one process worth
			printf("%s", buffer);
		}
	}  // end of while loop
	
	// debug statement
	// print the return value from system call
	// printf("ret = %d\n", ret);

	// de-allocate the memory allocated for the buffer
	free(buffer);

	// return 0 upon success
	return 0;
}

