/*   -- Start of mycall.c -- */

#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/string.h>

asmlinkage long sys_my_syscall(int a, int b, char *c)
{
	// declare kernel buffer to be same size as userspace buffer
	char kernel_buffer[4001];

	// declare other needed variables
	int len = 0;
	int n = 0;
	int count = 0;   // count of number of processes

	// put information in kernel_buffer
	// put the length of this information in len

	// declare task_struct to get linked list of tasks
	struct task_struct *task;

	// declare variables to hold cputimes
	// cputime_t t_in_jiffies;
	// unsigned long t_in_seconds;

	// iterate through the tasks, extracting the cputimes
	for_each_process(task) {
		// t_in_jiffies = task->utime + task->stime;
		// t_in_seconds = t_in_jiffies / HZ;
		// strcat(kernel_buffer, (char *) task->pid);
		// strcat(kernel_buffer, "\t");
		// strcat(kernel_buffer, (char *) t_in_seconds);
		// strcat(kernel_buffer, "\n");
		count++;
	}
	sprintf(kernel_buffer, "%d\n", count);
	// kernel_buffer[0] = 'h';
	// kernel_buffer[1] = 'e';
	// len = 2;

	// get size of kernel_buffer
	len = sizeof(kernel_buffer);

	// limit size of amount copied to size of userspace buffer
	if (len > b)
		len = b;

	// copy the contents of kernel_buffer to userspace buffer c
	n = copy_to_user(c, kernel_buffer, len);

	// return the number of bytes NOT copied to userspace
	return n;
}
/*   -- End of mycall.c -- */

