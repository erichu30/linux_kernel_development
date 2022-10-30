#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

asmlinkage long sys_hello(void)
{
        printk("Hello world\n");
        return 0;
}


SYSCALL_DEFINE0(hello) {
    printk("Hi, nice to meet you.\n");
    return sys_hello();
}
