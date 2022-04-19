* make mrproper
  * 清除不需要的 object file, config, and dependencies
* 在source code 中建立hello資料夾
* cd hello
* vim hello.c

```c=
#include <linux/kernel.h>
asmlinkage long sys_hello(void){
    printk("Hello World!\n");
    return 0;
}
```

* vim Makefile

```makefile=
obj-y := hello.o
```

* 在source code中的makefile，找到 core-y			+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/，依樣畫葫蘆新增 hello/ 在後面
* 修改 ./linux-5.16.9/arch/x86/entry/syscalls/syscall_64.tbl，新增編號

```
335	64	hello	__x64_sys_hello
```

* 在 ./linux-5.16.9/include/linux/syscalls.h 的最後一個asmlinkage long...後下一行新增
* 在 ./linux-5.15.32/arch/x86/include/generated/uapi/asm/unistd_64.h 新增 #define __NR_hello 335
* make oldconfig/ make menuconfig
* make -jN (N: by ``nproc``)
* if run into error

  * make[1]: *** 沒有規則可製作目標「debian/canonical-certs.pem」，由「certs/x509_certificate_list」 需求。 停止。
    make[1]: *** 正在等待未完成的作業....
  * then:

  ```
  sudo scripts/config --disable SYSTEM_TRUSTED_KEYS
  sudo scripts/config --disable SYSTEM_REVOCATION_KEYS
  ```
  一路enter
* sudo make modules
* sudo make modules_install
* sudo make install
* sign (https://gloveboxes.github.io/Ubuntu-for-Azure-Developers/docs/signing-kernel-for-secure-boot.html)
