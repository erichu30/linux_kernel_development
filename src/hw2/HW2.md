# The Research of Operating System and Cloud System Kernel
## HW_2
###### tags: `Linux` `kernel`
###### 2022.04.16
## Environment
*我是用 Proxmox 架的 VM 來編譯跟測試*

kernel version: 5.4.188
cpu: xeon e5-2690 (x86_64 bits)
distribution: ubuntu 20.04

## Problem and hint
### finding the largest process id

Write a system call, largest_process_id( ),  which a C program can invoke to print out the largest process id currently in the system.

ex.
```c=
main()

{

int max_process_id;

.....

max_process_id = largest_process_id();

printf("The largest process id currently in the system is %d", max_process_id);

...

}
```

### 自訂 system call

```linux=
# 清除不需要的 object file, config, and dependencies
sudo make mrproper
# 在source code 中建立largest_process_id資料夾
mkdir largest_process_id
cd largest_process_id
vim largest_process_id.c
```
- largest_process_id.c
```c=
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
```
- 同目錄新增 Makefile
```makefile=
vim Makefile

obj-y := largest_process_id.o
```
- 修改 source code 根目錄下的 Makefile，找到
```linux=
core-y	+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/
```
新增 ```largest_process_id/``` 在後方，變成
```linux=
core-y	+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ largest_process_id/ 
```

![](https://i.imgur.com/iFX5HXW.png)


- 修改 ```./linux-5.4.188/arch/x86/entry/syscalls/syscall_64.tbl```，新增編號
```linux=
437 common  largest_process_id           __x64_sys_largest_process_id
```
:::success
- From linux kernel v4.17 onwards, x86_64 system calls begin with "__x64_sys"
    - [reference](https://github.com/torvalds/linux/commit/d5a00528b58cdb2c71206e18bd021e34c4eab878)
- "hello": name of the system call
- "__x64_sys_hello": name of the function implementing it
:::

![](https://i.imgur.com/rydfkya.png)


- 在 ```./linux-5.4.188/include/linux/syscalls.h``` 找到
```#ifndef CONFIG_ARCH_HAS_SYSCALL_WRAPPER``` 並在下方新增 自訂systemcal call 的 prototype
```linux=
asmlinkage long sys_largest_process_id(void);
```

![](https://i.imgur.com/jwSV4wl.png)


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
##### If running into error
```
make[1]: *** 沒有規則可製作目標「debian/canonical-certs.pem」，由「certs/x509_certificate_list」 需求。 停止。
make[1]: *** 正在等待未完成的作業....
# make[1]: *** No rule to make target 'debian/canonical-certs.pem', needed by 'certs/x509_certificate_list'.  Stop.
# make: *** [Makefile:1809: certs] Error 2
```
##### Solution:

```linux=
sudo scripts/config --disable SYSTEM_TRUSTED_KEYS
sudo scripts/config --disable SYSTEM_REVOCATION_KEYS
sudo make clean
sudo make -j8
# or sudo make -j $(nproc)
sudo make modules
sudo make modules_install
sudo make install
```
[reference](https://askubuntu.com/questions/1329538/compiling-the-kernel-5-11-11)


### Sign Linux Kernel for Secure Boot
只要 Secure Boot 有打開，GRUB2(2.02+dfsg1-5ubuntu1) 就不會載入未 sign 過的 kernel

Ubuntu 18.04 之後的版本也會禁止升級未簽署的 kernel 

理論上把主機板的 Secure Boot 關閉也行 （未實驗過）

```linux=
# 在 source code 的 root 中新增 mokconfig.cnf
vim mokconfig.cnf
```
- mokconfig.cnf
```linux=
 HOME                    = .
 [ req ]
 distinguished_name      = req_distinguished_name
 x509_extensions         = v3
 string_mask             = utf8only
 prompt                  = no

 [ req_distinguished_name ]
 countryName             = <YOURcountrycode>
 stateOrProvinceName     = <YOURstate>
 localityName            = <YOURcity>
 0.organizationName      = <YOURorganization>
 commonName              = Secure Boot Signing Key
 emailAddress            = <YOURemail>

 [ v3 ]
 subjectKeyIdentifier    = hash
 authorityKeyIdentifier  = keyid:always,issuer
 basicConstraints        = critical,CA:FALSE
 extendedKeyUsage        = codeSigning,1.3.6.1.4.1.311.10.3.6
 nsComment               = "OpenSSL Generated Certificate"
 ```
- 替換掉 <YOUR...> 等欄位內容，內容隨意
for example

```linux=
 HOME                    = .
 [ req ]
 distinguished_name      = req_distinguished_name
 x509_extensions         = v3
 string_mask             = utf8only
 prompt                  = no

 [ req_distinguished_name ]
 countryName             = TW
 stateOrProvinceName     = Taiwan
 localityName            = Taipei
 0.organizationName      = FJCU
 commonName              = Secure Boot Signing Key
 emailAddress            = example@example.com

 [ v3 ]
 subjectKeyIdentifier    = hash
 authorityKeyIdentifier  = keyid:always,issuer
 basicConstraints        = critical,CA:FALSE
 extendedKeyUsage        = codeSigning,1.3.6.1.4.1.311.10.3.6
 nsComment               = "OpenSSL Generated Certificate"
 ```
- 回到bash
```linux=
# 產生private key 跟public key
openssl req -config ./mokconfig.cnf \
         -new -x509 -newkey rsa:2048 \
         -nodes -days 36500 -outform DER \
         -keyout "MOK.priv" \
         -out "MOK.der"

# mokutil 要 DER 格式, sbsign 要 PEM 格式
openssl x509 -in MOK.der -inform DER -outform PEM -out MOK.pem

# enroll the key
sudo mokutil --import MOK.der
# 輸入自訂密碼等等要用
# 重啟系統，會進入藍底的 MOKManager
# 選擇 Enroll MOK -> View Key
# 確定這是剛剛建立的key，並enroll -> 輸入密碼
# 繼續 boot
```
```linux=
# 確定有enroll成功
sudo mokutil --list-enrolled

# 簽署已安裝的kernel
# 替換 kernel 版本, e.g. 5.4.188 -> any version
sudo sbsign --key MOK.priv --cert MOK.pem /boot/vmlinuz-5.4.188 --output /boot/vmlinuz-5.4.188.signed

# 將未簽署的 kernel 的 initram 複製到已簽署的 kernel上
sudo cp /boot/initrd.img-5.4.188{,.signed}

# 更新grub
sudo update-grub
# reboot，在grub中選擇已簽署的kernel開機
# 如果開機成功，就可以移除未簽署的 kernel
sudo mv /boot/vmlinuz-5.4.188{.signed,}
sudo mv /boot/initrd.img-5.4.188{.signed,}
sudo update-grub

# check kernel version
uname -mrs
```

### test
- 新增一個 test.c
```c=
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/sched.h>

#define SYS_largest_process_id 437
#define largest_process_id() syscall(SYS_largest_process_id)

int main(){
    
    int max_process_id;
    max_process_id = largest_process_id();

    printf("The largest process id currently in the system is %d\n", max_process_id);
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



## Reference 
- [鳥哥: Linux 核心編譯與管理](https://linux.vbird.org/linux_basic/centos7/0540kernel.php#intro)
- [Signing a Linux Kernel for Secure Boot](https://gloveboxes.github.io/Ubuntu-for-Azure-Developers/docs/signing-kernel-for-secure-boot.html)
- [Kernel Hacking - System Calls(3/3)](https://marcocetica.com/posts/kernel_hacking_part3/)
- [Linux 讀書會 - System call](https://hackmd.io/@combo-tw/Linux-%E8%AE%80%E6%9B%B8%E6%9C%83/%2F%40a29654068%2FHyD4Lu_Dr)
- [Build Linux Kernel, Add a New System Call to Linux v2.4](http://osnet.cs.nchu.edu.tw/powpoint/Distributed_OS/Os/system_call.pdf)
- [增加一個 System Call 到 Linux Kernel (v4.x)](https://wenyuangg.github.io/posts/linux/linux-add-system-call.html)
- [How to compile and install Linux Kernel 5.16.9 from source code](https://www.cyberciti.biz/tips/compiling-linux-kernel-26.html)
- [Attempting to compile kernel yields a certification error](https://unix.stackexchange.com/questions/293642/attempting-to-compile-kernel-yields-a-certification-error)
- [Preparing the installer to update Linux kernel in a VirtualBox causes errors connected with CONFIG_X86_X32 and modules.order](https://unix.stackexchange.com/questions/680261/preparing-the-installer-to-update-linux-kernel-in-a-virtualbox-causes-errors-con)
    - CONFIG_X86_X32 enabled but no binutils support
    - 64 位處理器執行原生 32 位程式，需要開啟核心 CONFIG_X86_X32 選項
    - CONFIG_X86_X32 選項需要toolchain支援編譯 elf32_x86_64 目標
- [Sign kernel modules](https://www.twblogs.net/a/5b8a366d2b71775d1ce60496)
    - https://wiki.gentoo.org/wiki/Signed_kernel_module_support
- [SYSCALL: 添加零个参数的系统调用](https://biscuitos.github.io/blog/SYSCALL_PARAMENTER_ZERO/)
    - [X86_64 架构增加一个系统调用](https://biscuitos.github.io/blog/SYSCALL_ADD_NEW_X86_64/)
- [Linux kernel 5.0 增加 System Call](http://www.cjwind.idv.tw/Add-system-call-to-linux/)
- [關於新增system calls可能遭遇的問題](https://joj4211.pixnet.net/blog/post/133723316)