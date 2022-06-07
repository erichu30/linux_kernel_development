#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/fdtable.h>

asmlinkage long sys_number_of_files_opened(int _pid)
{
	struct task_struct *task;
	long opened_files_number = 0;
	for_each_process(task) {
		if (task->pid != _pid){
			continue;
		}

		// Ignore processes without files
    		if (task->files == NULL){
        		continue;
		}

		/* fd_array: array of file objects */
		printk("%s[%d]\n", task->comm, task->pid);

		struct file** f = task->files->fdt->fd;
		while(*f){
			opened_files_number += 1;
			f++;
		}
		printk("Number of files opened: %ld\n", opened_files_number);
	}

	return opened_files_number;
}

SYSCALL_DEFINE1(number_of_files_opened, int, _pid) {
    return sys_number_of_files_opened(_pid);
}

