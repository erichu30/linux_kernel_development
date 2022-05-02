#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

asmlinkage long sys_largest_process_id(void)
{
	long max = 0;
        struct task_struct *task;
	for_each_process(task) {
		/* get the max pid */
		if (task->pid > max){
			max = task->pid;
		}
	}
	return max;
}

SYSCALL_DEFINE0(largest_process_id) {
    return sys_largest_process_id();
}
