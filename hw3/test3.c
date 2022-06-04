#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_number_of_files_opened 436

int main(){
    */Write a system call, number_of_files_opened(int pid),  which a C program can invoke to report the number of files currently opened by a particular process identified by the user from the command line.*/
	int pid;
	printf("Please enter the process id\n");
    	scanf("%d", &pid);
	printf("There are %d files currently opened by the process id %d\n", syscall(SYS_number_of_files_opened, pid), pid);
}
