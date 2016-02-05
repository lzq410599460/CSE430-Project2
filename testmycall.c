#include <linux/unistd.h>
#include <stdio.h>
#define __NR_my_syscall 359

int main()
{
	char buffer[512];
	int n = 0;
	n = syscall(__NR_my_syscall, 0, sizeof(buffer), buffer);
	printf("buffer = %s\n", buffer);
	printf("n = %d\n", n);
	return 0;
}

