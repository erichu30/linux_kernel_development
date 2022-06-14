#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

int main(void) {
    int pid;
    printf("Please enter the process id\n");
    scanf("%d", &pid);
    
    long long sizes = syscall(437, pid);
    printf("The total number of bytes for files opened by the process %d is %lld bytes.\n", pid, sizes);
  
    return 0;
}
