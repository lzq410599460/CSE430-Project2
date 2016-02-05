/*   -- Start of mycall.c -- */

#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/string.h>

asmlinkage long sys_my_syscall(int a, int b, char *c)
{
	char kernel_buffer[512];
	int len = 0;
	int n = 0;
	int count = 0;
	// put information in kernel_buffer
	// put the length of this information in len
	struct task_struct *task;
	cputime_t t_in_jiffies;
	unsigned long t_in_seconds;
	for_each_process(task) {
		t_in_jiffies = task->utime + task->stime;
		t_in_seconds = t_in_jiffies / HZ;
		// strcat(kernel_buffer, (char *) task->pid);
		// strcat(kernel_buffer, "\t");
		// strcat(kernel_buffer, (char *) t_in_seconds);
		// strcat(kernel_buffer, "\n");
		count++;
	}
	strcpy(kernel_buffer, (char *) count);
	len = sizeof(kernel_buffer);
	//
	if (len > b)
		len = b;
	n = copy_to_user(c, kernel_buffer, len);
	return n;
}
/*   -- End of mycall.c -- */
