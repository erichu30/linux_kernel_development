#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

asmlinkage long sys_number_of_files_opened(int pid)
{
	int cnt;	// define the counting of number of files opened by specfic PID
	for_each_process(task) {
		/* this pointlessly prints the name, PID, and state of each task */
		/* task's state (-1 unrunnable, 0 runnable, > 0 stopped) */
		printk("%s[%d]_%ld\n", task->comm, task->pid, task->state);
		cnt += 1;
	}

	return cnt;
}

SYSCALL_DEFINE1(number_of_files_opened, int, pid) {
    return sys_number_of_files_opened(pid);
}

