#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/fs.h>

asmlinkage long long sys_open_file_sizes(int _pid)
{
	struct task_struct *task;
	long opened_files_number = 0;
	long long opened_files_sizes = 0;
	for_each_process(task) {
		if (task->pid != _pid){
			continue;
		}

		// Ignore processes without files
    		if (task->files == NULL){
        		continue;
		}
		printk("%s[%d]\n", task->comm, task->pid);
	
		struct file ** f = task->files->fdt->fd;
		//fd is a pointer to array of pointer to file object represented by struct file
                
		int i = 0;
		while(f[i]!=NULL){
                        opened_files_number += 1;
                	//printk("file size: %lld bytes\n", f[i]->f_inode->i_size);
			printk("file size: %lld bytes\n", i_size_read(f[i]->f_inode));
			opened_files_sizes += f[i]->f_inode->i_size;
			i++;
                }
		
                printk("Number of files opened: %ld\n", opened_files_number);
		printk("Size of files opened: %lld bytes\n", opened_files_sizes);
	}

	return opened_files_sizes;
}

SYSCALL_DEFINE1(open_file_sizes, int, _pid) {
    return sys_open_file_sizes(_pid);
}

