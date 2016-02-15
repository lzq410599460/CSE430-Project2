/*   -- Start of my_syscall.c -- */

#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/slab.h>

asmlinkage long sys_my_syscall(int a, int b, char *c)
{
	// declare kernel buffer to be same size as userspace buffer
	// use kmalloc to avoid compiler warning
	char * kbuf = (char *) kmalloc(513, GFP_KERNEL);

	// declare other needed variables
	int len = 0;	 // bytes written to kbuf so far
	int n = 0;	 // return value of copy_to_user
			 // equal to no. of chars not written to userspc
	int i = 0;	 // index to keep track of current process
	int ret = a;	 // count of processes, sent to userspace and back

	// declare task_struct to get linked list of tasks
	struct task_struct *task;

	// declare variables to hold cputimes
	cputime_t jiff, utime, stime;
	int t_in_seconds, hours, rem, mins, secs;

	// declare temp vbl for tty name
	char * tname = (char *) kmalloc(64, GFP_KERNEL);

	// iterate through the tasks until we get to the next process
	for_each_process(task) {

	   // use if-else stmt to either skip to next process or
	   //  else write the info for a single process to kbuf
	   if (i != a) {   // skip to next process
		i++;
	   }
	   else {   // write info for current process to kbuf
		// increment ret = count
		ret++;

		// reset len
		len = 0;

		// initialize the cputime vbls
		utime = 0;
		stime = 0;

		// set the cputime vbls for all threads in thread group
		thread_group_cputime_adjusted(task, &utime, &stime);
		jiff = utime + stime;

		// convert jiffies to seconds
		t_in_seconds = (int) (jiff / HZ);

		// compute cputime in hours:mins:secs format
		hours = t_in_seconds / 3600;
		rem = t_in_seconds % 3600;
		mins = rem / 60;
		secs = rem % 60; 
		
		// PRINT OUTPUT TO KERNEL BUFFER
		// print the pid
		len += sprintf(kbuf+len, "%5d ", (int) task->pid);

		// test the tty for (null) and replace with ?
		sprintf(tname, "%-8s", task->signal->tty->name);
		if (tname[0] == '(' && tname[1] == 'n' && tname[2] == 'u' \
		    && tname[3] == 'l' && tname[4] == 'l' \
		    && tname[5] == ')' ) {
			tname[0] = '?';
			tname[1] = '\0';
		}

		// test the tty for pts and replace with pts/
		if (tname[0] == 'p' && tname[1] == 't' && tname[2] == 's') {
			tname[4] = tname[3];
			tname[3] = '/';
			tname[5] = '\0';
		}

		// print the TTY
		len += sprintf(kbuf+len, "%-8s ", tname);

		// print the cputime
		len += sprintf(kbuf+len, "%02d:%02d:%02d ", hours, mins, secs);

		// print the command description
		len += sprintf(kbuf+len, "%s\n", task->comm);

		// break to stop iterating through processes
		break;

	   } // end else
	} // end for_each_process()

	// limit size of amount copied to size of userspace buffer
	if (len > b)
		len = b;

	// copy the contents of kernel_buffer to userspace buffer c
	n = copy_to_user(c, kbuf, len);

	// free the kernel buffer and tname allocated with kmalloc
	kfree(kbuf);
	kfree(tname);

	// return the count of processes to userspace, to hold 
	//  our place for next iteration
	return ret;
}
/*   -- End of my_syscall.c -- */
