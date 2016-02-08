/*   -- Start of mycall.c -- */

#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/jiffies.h>
#include <linux/string.h>

asmlinkage long sys_my_syscall(int a, int b, char *c)
{
	// declare kernel buffer to be same size as userspace buffer
	char kbuf[4001];

	// declare other needed variables
	int len = 0;
	int n = 0;
	int count = 0;   // count of number of processes

	// put information in kernel_buffer
	// put the length of this information in len

	// declare task_struct to get linked list of tasks
	struct task_struct *task;

	// declare variables to hold cputimes
	cputime_t jiff, utime, stime;
	int t_in_seconds, hours, rem, mins, secs;

	// iterate through the tasks, extracting the cputimes
	for_each_process(task) {
		utime = 0;
		stime = 0;
		thread_group_cputime_adjusted(task, &utime, &stime);
		jiff = utime + stime;
		t_in_seconds = (int) (jiff / HZ);
		hours = t_in_seconds / 3600;
		rem = t_in_seconds % 3600;
		mins = rem / 60;
		secs = rem % 60; 
		// print the pid
		len += sprintf(kbuf+len, "%5d ", (int) task->pid);
		// print the TTY
		len += sprintf(kbuf+len, "%8s ", task->signal->tty->name);

		// print the cputime
		len += sprintf(kbuf+len, "%02d:%02d:%02d ", hours, mins, secs);

		// print the commands
		len += sprintf(kbuf+len, "%s\n", task->comm);
		count++;
	}
	// sprintf(kernel_buffer, "%d\n", count);

	// get size of kernel_buffer
	// len = sizeof(kbuf);

	// limit size of amount copied to size of userspace buffer
	if (len > b)
		len = b;

	// copy the contents of kernel_buffer to userspace buffer c
	n = copy_to_user(c, kbuf, len);

	// return the number of bytes NOT copied to userspace
	return n;
}
/*   -- End of mycall.c -- */


