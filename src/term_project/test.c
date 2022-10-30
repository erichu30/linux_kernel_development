#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>

struct process_struct
{
    long pid;
    long long opened_file_size;
};

int main(void) {
    int top_n;
    printf("Please enter a number that how many processes you want to rank\n");
    scanf("%d", &top_n);

    struct process_struct *user_addr = malloc(sizeof(struct process_struct)*top_n);
    long task_number = syscall(438, user_addr , top_n);
    // printf("check the dmesg\n");

    int i;
    printf("PID\tSIZES(MB)\n");
    for (i = 0; i < top_n; ++i){
	printf("%ld\t%lld\n", user_addr[i].pid, user_addr[i].opened_file_size);
    }
    
    free(user_addr);
    return 0;
}
