#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<dirent.h>
int main(){
    DIR *p = opendir(".");
    if(!p){
        perror("opendir");
        return 1;
    }
    struct dirent *ent;
    while((ent = readdir(p)) != NULL){
        if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
            continue;
        }
        printf("%s ",ent->d_name);
    }
    printf("\n");
    return 0;
}