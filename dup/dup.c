#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
int main(void){
    int fd = open("a.out",O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(dup2(fd,1) < 0){
        perror("dup2");
        close(fd);
        return 1;
    }
    printf("你好\n");
    fflush(stdout);
    write(1,"你好\n",7);    
}