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
