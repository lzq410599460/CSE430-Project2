
/*
 *  defuse-mod.c - A kernel module to kill fork bombs for project2 step2.
 */
#include <linux/module.h>		/* Needed by all modules */
#include <linux/kernel.h>		/* Needed for KERN_INFO */
#include <linux/init.h>			/* Needed for the macros */
#include <linux/sched.h>	// allows access to task_struct
#include <linux/delay.h>	// allows use of msleep()
#include <linux/kthread.h>	// attempt to avoid implicit macro errors
#include <linux/semaphore.h>	// attempt to avoid down_int errors
#include <linux/list.h>		// used for struct list_head
#include <linux/signal.h>	// used for do_send_sig_info
#include <linux/slab.h>		// added for kmalloc

// declare global variables to allow start and stop kthread
struct task_struct *task_prod;	// producer thread
struct task_struct *task_cons;	// consumer thread

// declare global variable for shared data - array of pids to kill
int pidlist[2000];

// declare global variable for shared data - number of pids in pidlist 
int pidcount=0;

// declare global variable - semaphore
DEFINE_SEMAPHORE(mr_sem);
// DECLARE_MUTEX(mr_sem);
// DECLARE_MUTEX_UNLOCKED(mr_sem);

/*	producer function - detects fork bombs and saves pids in pidlist
*/
int producer(void *data) {

	// set the threshold for number of children in fork bomb
	int threshold = 10;

	// declare task struct for iterating thru all processes
	struct task_struct *task3;

	// declare int for count of children in a task
	int ccount = 0;

	// declare list for iterating thru children
	struct list_head *pos;

	// declare task struct for iterating thru all children
	struct task_struct *tsk2;

	// debug
	printk(KERN_INFO "-- Starting module to check for fork bombs --\n");

	while(!kthread_should_stop()) {

	   // debug
	   printk(KERN_INFO "-- Inside while loop, about to sleep --\n");
	   // most of the time the thread sleeps
	   msleep(5000);

	   // debug
	   printk(KERN_INFO "-- Inside while loop, just woke up --\n");
	   printk(KERN_INFO "-- About to enter foreachprocess --\n");

	   // now thread wakes up and does some work
	   // produce data - find fork bomb pids to kill

	   // take read lock to protect task list
	   // read_lock(&tasklist_lock);

           // iterate through the processes
           for_each_process(task3) {

	     // debug
	     printk(KERN_INFO "-- Inside foreachprocess, counting kids --\n");
	     // check number of children
	     ccount = 0;
	     list_for_each(pos, &task3->children) {
	       tsk2 = list_entry(pos, struct task_struct, sibling);
	       // tsk2 now points to one of current's children
	       ccount++;
	     }
	     printk(KERN_INFO "-- PID = %d --\n", (int) task3->pid);
	     printk(KERN_INFO "-- Number of kids = %d --\n\n", ccount);

	     // write data to buffer - write fork bomb pids to pidlist
	     // if no. of children exceeds threshold, add to pidlist
	     if ( (ccount > threshold) && ((int) task3->pid > 40) ) {
		pidlist[pidcount] = (int)task3->pid; // add pid
		pidcount++;	// increment pidcount
		printk(KERN_INFO "Found forkbomb, PID=%d\n", (int)task3->pid);
	     }

	   } // end of loop to iterate thru all tasks

	   // release tasklist lock
	   // read_unlock(&tasklist_lock);

	   // call up to release the synch semaphore
	   // up(&mr_sem);

	} // end of infinite while loop

	// return 0 for success if we get this far
	return 0;
} // end of producer()

/*	consumer function - kills processes with pids in pidlist
*/
int consumer(void *data) {

	// declare temp variables for iterating thru tasks
	// struct task_struct *task4;
	int i = 0;
	int pid2kill = 0;

	// use infinite loop for consumer
	while(!kthread_should_stop()) {

	   // call down to wait for synch semaphore
	   // down(&mr_sem);

	   // read data from buffer - get pid from pidlist
           for (i=0; i < pidcount; i++) {
	      // find the correct process
	      pid2kill = pidlist[i];
	      // task4 = pid_task(find_vpid((pid_t)pid2kill), PIDTYPE_PID);

	      // process data - kill the pid in pidlist
              // send_sig(SIGKILL, task4, 0);
              printk(KERN_INFO "Killed fork-bomb pid %d\n", pid2kill);

	    } // end of for-loop to loop thru pids

	    // reset the number of pids to kill
	    pidcount = 0;

	    // do_send_sig_info(SIGKILL, SEND_SIG_FORCED, task3, true);
			   
	} // end of infinite while loop

	// return 0 if we get this far
	return 0;

} // end of consumer()

static int __init my_name(void)
{
	// declare data to pass to kernel thread function
	int data = 20;

	// create and run kernel thread running "producer"
	task_prod = kthread_run(&producer, (void*) &data, "producer");

	// create and run kernel thread running "consumer"
	task_cons = kthread_run(&consumer, (void*) &data, "consumer");

	// printk(KERN_INFO "Mark Strickland\n");

	
	/* 
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

static void __exit my_name_exit(void)
{
	// stop the kernel thread started earlier
	kthread_stop(task_prod);
	kthread_stop(task_cons);
}

module_init(my_name);
module_exit(my_name_exit);
	
