#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
int main(int argc,char *argv[]){
    if(argc != 3){
        fprintf(stderr,"用法:./cp 源文件 目标文件\n");
        return 1;
    }
    int src = open(argv[1],O_RDONLY);
    if(src < 0){
        perror("open src");
        return 1;
    }
    int dst = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(dst < 0){
        perror("open dst");
        close(src);
        return 1;
    }
    char buf[1024];
    while(1){
        ssize_t r_len = read(src,buf,sizeof(buf));
        if(r_len == 0)break;
        if(r_len < 0){
            if(errno == EINTR){
                continue;
            }
            perror("read");
            close(src);
            close(dst);
            return 1;
        }
        size_t total = 0;  
        while(total < (size_t)r_len){
            ssize_t w_len = write(dst,buf + total,(size_t)(r_len - total));
            if(w_len < 0){
                if(errno == EINTR){
                    continue;
                } 
                perror("write");
                close(src);
                close(dst);
                return 1;
            }
            if (w_len == 0) {
                fprintf(stderr, "write returned 0\n");
                close(src); close(dst);
                return 1;
            }
            total += (size_t)w_len;
        }
    }
    close(src);
    close(dst);
    return 0;
}