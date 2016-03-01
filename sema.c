//This is just a basic function module for semaphore, will improve.

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

MODULE_LICENSE("GPL");

int count=4;     //this count control the loop between producer and consumer
int p2k;	//store the PID for fork bomb
struct semaphore write;		//semaphore to sync
struct semaphore read;




int producer(void *data) 
{
	int j=0;			//j variable is just for test we will use for_each_task() in the while loop with -
	down(&read);			//while (1) to keep it going
	while(j<10)
	{
		//read_lock(&tasklist_lock)  not tested yet (in linux/sched.h ln317 )
		printk(KERN_INFO "PID : %d  children: %d\n",j,-(j-5)*(j-5)+30);
		j++;
		if(-(j-5)*(j-5)+30>29)		//assume 29 is threshold of fork bomb
		{
			p2k=j;
			up(&write);
			
		}
		//read_unlock(&tasklist_lock)
		//put sleep time here 
	}
return 0;
}

int consumer (void *data)
{
	while(1)
	{
		down(&write);
		printk(KERN_INFO "\n====Process killed ID :%d====\n",p2k);	//kill the fork bomb here
		up(&read);
	}
	return 0;
}

static int __init start(void)
{
	sema_init(&write,0);
	sema_init(&read,1);

	printk(KERN_INFO "\n------STARTING------\n");

	while(count>0)			//using while(1) to keep it going forever
	{
		task_prod = kthread_run(&consumer,NULL,"consumer");
		printk(KERN_INFO "\n------Consuming------\n");
		task_prod = kthread_run(&producer,NULL,"producer");
		printk(KERN_INFO "\n------Producing------\n");

		count--;
	}

return 0;
}

static void __exit exit(void)
{
//int pstopflag = -1;
//int cstopflag = -1;

	printk(KERN_INFO "-- stopping defuse-mod producer thread --\n");
//pstopflag = kthread_stop(task_prod);

	printk(KERN_INFO "-- stopping defuse-mod consumer thread --\n");
//cstopflag = kthread_stop(task_cons);

	printk(KERN_INFO "\n------Exiting------\n");


}

module_init(start);
module_exit(exit);
