
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
#include <linux/kfifo.h>	// added for kfifo
#include <linux/pid_namespace.h>	// added for find_task_by_vpid
#include <linux/pid.h>		// added for PIDTYPE_PID

// declare global variables to allow start and stop kthread
struct task_struct *task_prod;	// producer thread
struct task_struct *task_cons;	// consumer thread

// declare global variable for shared data - fifo queue of pids to kill
// int pidlist[2000];
// struct kfifo fifo;
// int ret1 = 0;
// kfifo_alloc(&fifo, PAGE_SIZE, GFP_KERNEL);
// if (ret1)
// 	return ret1;
// fifo now represents a PAGE-SIZE-sized queue
DEFINE_KFIFO(fifo, int, PAGE_SIZE);

// declare global variable for shared data - number of pids in pidlist 
// int pidcount=0;

// declare global variable - semaphore
// DEFINE_SEMAPHORE(mr_sem);
// DECLARE_MUTEX(mr_sem);
// DECLARE_MUTEX_UNLOCKED(mr_sem);

// GPL license to get access to certain macros
MODULE_LICENSE("GPL");

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

	// declare unsigned int for enqueuing pids
	unsigned int pid2q;

	// debug
	// printk(KERN_INFO "-- Starting module to check for fork bombs --\n");

	while(!kthread_should_stop()) {

	   // debug
	   // printk(KERN_INFO "-- Inside while loop, just woke up --\n");
	   // printk(KERN_INFO "-- About to enter foreachprocess --\n");

	   // now thread wakes up and does some work
	   // produce data - find fork bomb pids to kill

	   // take read lock to protect task list
	   // read_lock(&tasklist_lock);

           // iterate through the processes
           for_each_process(task3) {

	     // debug
	     // printk(KERN_INFO "-- In foreachprocess, counting kids --\n");
	     // check number of children
	     ccount = 0;
	     list_for_each(pos, &task3->children) {
	       tsk2 = list_entry(pos, struct task_struct, sibling);
	       // tsk2 now points to one of current's children
	       ccount++;
	     }
	     // debug
	     // printk(KERN_INFO "-- PID = %d --\n", (int) task3->pid);
	     // printk(KERN_INFO "-- Number of kids = %d --\n\n", ccount);

	     // write data to buffer - write fork bomb pids to kill q
	     // if no. of children exceeds threshold, add to kill q
	     if ( (ccount > threshold) && ((int) task3->pid > 40) ) {
		// pidlist[pidcount] = (int)task3->pid; // add pid
		// pidcount++;	// increment pidcount
		printk(KERN_INFO "Prod: forkbomb @ PID=%d\n", (int)task3->pid);

		// add children to fifo queue 
	        list_for_each(pos, &task3->children) {
	          tsk2 = list_entry(pos, struct task_struct, sibling);
	          // tsk2 now points to one of current's children
		  pid2q = (int) tsk2->pid;
	          kfifo_in(&fifo, &pid2q, sizeof(pid2q));
	        }
		// add original (parent) fork-bomb pid to fifo queue
		pid2q = (int) task3->pid;
		kfifo_in(&fifo, &pid2q, sizeof(pid2q));
	     }
	     else {
	        // placeholder
	     }

	   } // end of loop to iterate thru all tasks

	   // release tasklist lock
	   // read_unlock(&tasklist_lock);

	   // call up to release the synch semaphore
	   // up(&mr_sem);

	   // debug
	   // printk(KERN_INFO "-- About to sleep --");

	   // sleep for a while
	   msleep_interruptible(3000);

	} // end of infinite while loop

	// reset kfifo
	// kfifo_reset(&fifo);
	INIT_KFIFO(fifo);

	// return 0 for success if we get this far
	return 0;
} // end of producer()

/*	consumer function - kills processes with pids in kill q
*/
int consumer(void *data) {

	// declare temp variables for iterating thru tasks
	struct task_struct *task4;
	// int i = 0;
	unsigned int pid2kill;
	int ret2;

	// use infinite loop for consumer
	// debug
	// printk(KERN_INFO "-- Cons: entering infinite while loop --\n");
	set_current_state(TASK_INTERRUPTIBLE);
	while(!kthread_should_stop()) {

	   // debug
	   // printk(KERN_INFO "-- Cons: in while loop, going to sleep --\n");
	   // sleep for a while
	   // msleep(5000);

	   // call down to wait for synch semaphore
	   // down(&mr_sem);

	   // read data from buffer - get pid from pidlist
           // for (i=0; i < pidcount; i++) {

	   // debug
	   // printk(KERN_INFO "-- Cons: entering kfifo while loop --\n");

	   while(!kfifo_is_empty(&fifo)) {
	   // while(kfifo_peek(&fifo, 0) != 0) {

	      // debug
	      // printk(KERN_INFO "-- Cons: about to call kfifo_out --\n");
	      // read from queue, one integer at a time
	      ret2 = kfifo_out(&fifo, &pid2kill, sizeof(pid2kill));
	      // if (ret2 != sizeof(pid2kill))
		// return -EINVAL;
	      // find the correct process
	      // pid2kill = pidlist[i];

	      // debug
	      // printk(KERN_INFO "-- Cons: about to get task from pid --\n");
	      task4 = pid_task(find_vpid((pid_t)pid2kill), PIDTYPE_PID);

	      // debug
	      // printk(KERN_INFO "-- Cons: entering if-else to kill pid  --\n");
	      // make sure you found a valid task struct - if not, skip
	      if (task4 == NULL) {
		;
	      }
	      else {
	        // debug
		// printk(KERN_INFO "--Cons: about to call send_sig --\n");
	        // process data - kill the pid in pidlist
                send_sig(SIGKILL, task4, 0);
                printk(KERN_INFO "Killing fork-bomb pid %d\n", pid2kill);
	        msleep(80);    // wait for process to die
	      }

	    } // end of loop to dequeue pids

	    // reset the number of pids to kill
	    // pidcount = 0;

	    // do_send_sig_info(SIGKILL, SEND_SIG_FORCED, task3, true);

	    // debug
	    // printk(KERN_INFO "-- Cons: in inf-while loop, going to sleep --\n");
	   
	    // sleep for a while
	    // msleep_interruptible(5000);
	    schedule();
	    set_current_state(TASK_INTERRUPTIBLE);
			   
	} // end of infinite while loop

	// reset kfifo
	// kfifo_reset(&fifo);
	// INIT_KFIFO(fifo);

	// from sleeping kernel article
	__set_current_state(TASK_RUNNING);

	// return 0 if we get this far
	return 0;

} // end of consumer()

static int __init my_name(void)
{
	// declare data to pass to kernel thread function
	// int data = 20;

	// print starting message
	printk(KERN_INFO "\n-- defuse-mod STARTING --\n");

	// create and run kernel thread running "producer"
	// task_prod = kthread_run(&producer, (void*) &data, "producer");
	task_prod = kthread_run(&producer, NULL, "producer");
	printk(KERN_INFO "-- defuse-mod producer thread started --\n");

	// create and run kernel thread running "consumer"
	// task_cons = kthread_run(&consumer, (void*) &data, "consumer");
	task_cons = kthread_run(&consumer, NULL, "consumer");
	printk(KERN_INFO "-- defuse-mod consumer thread started --\n");
	
	/* 
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

static void __exit my_name_exit(void)
{
	// temp vbls
	int pstopflag = -1;
	int cstopflag = -1;

	// stop the kernel threads started earlier
	// stop the producer thread
	printk(KERN_INFO "-- stopping defuse-mod producer thread --\n");
	pstopflag = kthread_stop(task_prod);
	msleep(6000);		// wait for producer to stop
	// debug
	// printk(KERN_INFO "-- pstopflag = %d --\n", pstopflag);

	// stop the consumer thread
	printk(KERN_INFO "-- stopping defuse-mod consumer thread --\n");
	cstopflag = kthread_stop(task_cons);
	// msleep(6000);		// wait for consumer to stop
	// debug
	// printk(KERN_INFO "-- cstopflag = %d --\n", cstopflag);

	// free the kfifo
	printk(KERN_INFO "-- freeing the kfifo in defuse-mod --\n");
	kfifo_free(&fifo);

	// print end of defuse-mod
	printk(KERN_INFO "-- defuse-mod EXITING --\n\n");
}

module_init(my_name);
module_exit(my_name_exit);
	
