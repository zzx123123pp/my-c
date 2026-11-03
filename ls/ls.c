#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<dirent.h>
#include<string.h>
int main(){
    DIR *p = opendir(".");
    if(!p){
        perror("opendir");
        return 1;
    }
    struct dirent *ent;
    errno = 0;
    while((ent = readdir(p)) != NULL){
     
        if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
            continue;
        }
        printf("%s\n",ent->d_name);
    }
    if(errno != 0)perror("readdir");
    closedir(p);
    printf("\n");
    return 0;
}