#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>
void qwq(){
    write(1,"\n不给退略略略\n",20);
}
int main(void){
    printf("我的终端,exit退出\n");
    char cmd[1024];
    signal(SIGINT,qwq);
    while(1){
        printf("mysh>>");
        fflush(stdout);
        if(fgets(cmd,sizeof(cmd),stdin) == NULL){
            if (errno == EINTR) {
                clearerr(stdin); 
                write(1, "\n", 1);
                continue;
                } 
        }   
        
        cmd[strcspn(cmd,"\n")]  = '\0';
        if(strlen(cmd) == 0 )continue;
        if(strcmp(cmd,"exit") == 0)break;
        char *argv[128];
        int argc = 0;
        char *saveptr;
        char *token = strtok_r(cmd," ",&saveptr);
        while(token != NULL){
            argv[argc++] = token;
            token = strtok_r(NULL," ",&saveptr);
        }
        argv[argc] = NULL;
        pid_t pid=fork();
        if(pid < 0){
            perror("fork");
            return 1;
        }
        if(pid == 0){
            signal(SIGINT,SIG_DFL);
            execvp(argv[0],argv);
            perror("execvp");
            _exit(1);
        }
        if(pid > 0){
            waitpid(pid,NULL,0);
        }
    }
}