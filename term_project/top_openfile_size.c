#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/delay.h>

int front, rear;

struct process_struct{
        long pid;
	long long opened_file_size;
};

void create(void);
void insert(struct process_struct *ptr, struct process_struct p_in, int top_n);
void prioritized(struct process_struct *ptr, struct process_struct p_in);
void display(struct process_struct *ptr);

asmlinkage long sys_top_openfile_size(struct process_struct *user_addr_space, int top_n)
{
	struct process_struct *ptr;
	ptr = kmalloc(sizeof(struct process_struct)*top_n, GFP_KERNEL);
	if(!ptr){
		return -ENOMEM;
	}else{
		printk("allocated a pointer point to block of memory\n");
	}

	create();
	struct process_struct p;

	struct task_struct *task;
	long task_number = 0;
	for_each_process(task) {
		// Ignore processes without files
		if (task->files == NULL){
			continue;
		}
		printk("%s[%d]\n", task->comm, task->pid);
	
		struct file ** f = task->files->fdt->fd;
		//fd is a pointer to array of pointer to file object represented by struct file
		
		long opened_files_number = 0;
		long long opened_files_sizes = 0.0;
		
		int i = 0;
		while(f[i]!=NULL){
			opened_files_number += 1;
			//printk("file size: %lld bytes\n", i_size_read(f[i]->f_inode));
			opened_files_sizes += f[i]->f_inode->i_size;
			i++;
		}
		
		printk("Number of files opened: %ld\n", opened_files_number);
		printk("Size of files opened: %lld bytes\n", opened_files_sizes);
		
		p.pid = task->pid;
		p.opened_file_size = opened_files_sizes/1000000;

		insert(ptr, p, top_n);

		task_number += 1;
	}

	// display(ptr);

	copy_to_user(user_addr_space, ptr, (sizeof(struct process_struct)*top_n));
	kfree(ptr);
	
	return task_number;
}

void create(void)
{
        front = -1;
        rear = -1;
}

void insert(struct process_struct *ptr, struct process_struct p_in, int top_n)
{
        if(rear >= top_n - 1){
                long long min_opened_file_size = LLONG_MAX;
		int i, minimum_file_size_pid = 0;	
		for(i = 0; i < top_n; ++i){
			if(ptr[i].opened_file_size <= min_opened_file_size){
				min_opened_file_size = ptr[i].opened_file_size;
				minimum_file_size_pid = i;
			}
		}
		if(ptr[minimum_file_size_pid].opened_file_size <= p_in.opened_file_size){
			ptr[minimum_file_size_pid] = p_in;
		}
		
                prioritized(ptr, p_in);
		return;
        }

        if((front == -1)&&(rear == -1)){
                front += 1;
                rear += 1;
                ptr[rear] = p_in;
                return;
        }else{
                prioritized(ptr, p_in);
        }
        rear++;
}

void prioritized(struct process_struct *ptr, struct process_struct p_in)
{
        int i, j;
        for (i = 0; i <= rear; i++){
                if(p_in.opened_file_size >= ptr[i].opened_file_size){
                        for(j = rear+1; j > i; j--){
                                ptr[j] = ptr[j-1];
                        }
                        ptr[i] = p_in;
                        return;
                }
        }
        ptr[i] = p_in;
}

void display(struct process_struct *ptr)
{
	if((front == -1) && (rear == -1)){
		printk("\nQueue is empty!\n");
	}
	
	for (;front<=rear; front++){
		printk("->%ld", ptr[front].pid);
	}
}


SYSCALL_DEFINE2(top_openfile_size, struct process_struct *, user_addr_space, int, top_n) {
	return sys_top_openfile_size(user_addr_space, top_n);
}
