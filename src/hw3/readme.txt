My readme file is refer to the following link, thanks.

https://hackmd.io/@cwhu/r1w3nn2_c

# hw3_The Research of Operating System and Cloud System Kernel
## HW_3
###### tags:`OSS` `Linux` `kernel`
###### 2022.06.07
## Environment
*我是用 Proxmox 架的 VM 來編譯跟測試*

kernel version: 5.4.188
cpu: xeon e5-2690 (x86_64 bits)
distribution: ubuntu 20.04

## Problem and hint
### number of files opened

Write a system call, number_of_files_opened(int pid),  which a C program can invoke to report the number of files currently opened by a particular process identified by the user from the command line.

ex.

shell> Please enter the process id

shell> 128 (input by the user)

shell> There are xxxx files currently opened by the process id 128.


## Solution
### 自訂 system call

```linux=
# 清除不需要的 object file, config, and dependencies
sudo make mrproper
# 在source code 中建立largest_process_id資料夾
mkdir largest_process_id
cd largest_process_id
vim largest_process_id.c
```
- number_of_files_opened.c
```c=
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/sched.h>    /* Needed for for_each_process */
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
```
#### Idea
![](https://i.imgur.com/7PUmOH2.png)

![](https://i.imgur.com/E3gFR58.png)

![](https://i.imgur.com/Ftcj8PG.png)

- 同目錄新增 Makefile
```makefile=
vim Makefile

obj-y := number_of_files_opened.o
```
- 修改 source code 根目錄下的 Makefile，找到
```linux=
core-y	+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/
```
新增 ```number_of_files_opened/``` 在後方，變成
```linux=
core-y	+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ number_of_files_opened/ 
```


- 修改 ```./linux-5.4.188/arch/x86/entry/syscalls/syscall_64.tbl```，新增編號
```linux=
437 common  number_of_files_opened           __x64_sys_number_of_files_opened
```
:::success
- From linux kernel v4.17 onwards, x86_64 system calls begin with "__x64_sys"
    - [reference](https://github.com/torvalds/linux/commit/d5a00528b58cdb2c71206e18bd021e34c4eab878)
:::


- 在 ```./linux-5.4.188/include/linux/syscalls.h``` 找到
```#ifndef CONFIG_ARCH_HAS_SYSCALL_WRAPPER``` 並在下方新增 自訂systemcal call 的 prototype
```linux=
asmlinkage long sys_number_of_files_opened(int _pid);
```



:::info
:bulb: kernel v4.10 後，kernel 會用script自動計算 __NR_syscalls  因此不再需修改```/arch/x86/entry/entry_64.S```及```arch/x86/include/generated/uapi/asm/unistd_64.h```

只需要更改 ```arch/x86/entry/syscalls/syscall_64.tbl``` 和 ```include/linux/syscalls.h``` 就可以了

詳見 [Patch - ARM: convert to generated system call tables](https://patchwork.kernel.org/project/linux-arm-kernel/patch/E1bwa6s-0004ZY-4k@rmk-PC.armlinux.org.uk/)
:::
#### Compile the Kernel

```linux=
make oldconfig/ make menuconfig
sudo make -jN (N: by "nproc")
sudo make modules
sudo make modules_install
sudo make install
```


### test
- 新增一個 test.c
```c=
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

#define SYS_hello 436

int main(void) {
    int pid;
    printf("Please enter the process id\n");
    scanf("%d", &pid);
    
    long output = syscall(436, pid);
    printf("There are %ld files currently opened by the process id %d.\n", output, pid);
  
    return 0;
}
```
- compile and execute
```c=
gcc test.c -o test.o
./test.o
```
- check dmesg 
```linux=
sudo dmesg
```
## Result

![](https://i.imgur.com/bbP0hHY.png)

另也可用以下指令驗證結果
```linux=
ls /proc/$pid/fd/ | wc -l
```


## Reference 
- [Data Structures Associated with a Process](http://books.gigatux.nl/mirror/kerneldevelopment/0672327201/ch12lev1sec10.html)
- [Linux 系統程式設計 - fd 及 open()、close() 系統呼叫](https://blog.jaycetyle.com/2018/12/linux-fd-open-close/)
- [系統程式設計 - open(), read(), write()](https://hackmd.io/@0xff07/B1MoQAorF#%E9%82%A3%E5%80%8B-Open-File-Table)
- [https://quant67.com/post/linux/filesio.html](https://quant67.com/post/linux/filesio.html)
- [printing open files of current process](https://stackoverflow.com/questions/8115299/printing-open-files-of-current-process)

