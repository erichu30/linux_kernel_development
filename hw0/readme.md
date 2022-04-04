1. 在source code 中建立hello資料夾
2. cd hello
3. vim hello.c
```c=
#include <linux/kernel.h>
asmlinkage long sys_hello(void){
    printk("Hello World!\n");
    return 0;
}
```
4. vim Makefile
```makefile=
obj-y := hello.o
```
5. 在source code中的makefile，找到 core-y			+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/，依樣畫葫蘆新增 hello/ 在後面
6. 修改 ./linux-5.16.9/arch/x86/entry/syscalls/syscall_64.tbl，新增編號
7. 在 ./linux-5.16.9/include/linux/syscalls.h 的最後一個asmlinkage long...後下一行新增