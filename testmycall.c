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
	int n = 4000;

	// allocate memory for buffer to hold result
	if( ( buffer = (char *) malloc(n*sizeof(char)+1) ) == NULL ) 
		perror( "Error allocating buffer\n" );

	// print the size of the buffer
	// printf("buffer size = %d\n", n);

	// perform system call, storing return value in ret
	// result of system call will be stored in buffer
	ret = syscall(__NR_my_syscall, 0, n, buffer);

	// print the results of the system call - contents of buffer
	// printf("buffer = %s\n", buffer);
	printf("  PID TTY          TIME CMD\n");
	printf("%s", buffer);

	// print the return value from system call
	// this should have the number of bytes not copied to userspace
	// printf("ret = %d\n", ret);

	// de-allocate the memory allocated for the buffer
	free(buffer);

	// return 0 upon success
	return 0;
}

