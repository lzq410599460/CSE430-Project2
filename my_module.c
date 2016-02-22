/*   -- Start of my_module.c --   */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kthread.h>

struct task_struct *my_task;

int my_kthread_function(void *data) {

	// repeat my_kthread_function until stop signal is detected
	while(!kthread_should_stop()) {
		
		//printk(KERN_INFO "Work before bed\n");
		
		// sleep for five seconds on each iteration
		msleep(5000);
		
		//printk(KERN_INFO "Wake up and do some work\n");

	}
	return 0;
}

static int __init my_module_init(void) {
	
	int data = 20;

	printk(KERN_INFO "----Module loaded----\n");

	// create new thread
	my_task = kthread_run(&my_kthread_function,(void *) &data,"my_thread");
	return 0;
}

static void __exit my_module_exit(void) {

	printk(KERN_INFO "----Module removed----\n");
	kthread_stop(my_task);
}

module_init(my_module_init);
module_exit(my_module_exit);

/*   -- End of my_module.c --   */
