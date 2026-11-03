#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
int copy(char *argv1,char *argv2){
    int src = open(argv1,O_RDONLY);
            if(src <0){
                perror("open src");
                return 1;
            }
            int dst = open(argv2,O_WRONLY | O_CREAT | O_TRUNC,0644);
            if(dst < 0){
                perror("open dst");
                close(src);
                return 1;
            }
            char buf[1024];
            while(1){
                size_t total = 0;
                ssize_t r_len = read(src,buf,sizeof(buf));
                if(r_len < 0){
                    if(errno == EINTR){
                        continue;
                    }
                    perror("read");
                    close(src);
                    close(dst);
                    return 1;
                }
                if (r_len == 0) break;
                while(total < (size_t)r_len){
                    ssize_t w_len = write(dst,buf + total,(size_t)r_len - total);
                    if(w_len < 0){
                        if(errno == EINTR){
                            continue;
                        }
                        perror("write");
                        close(src);
                        close(dst);
                        return 1;
                    }               
                    total += (size_t)w_len;                 
                }
            }
            close(src);
            close(dst);
            return 0;
    }
int is_same(char *argv1, char *argv2) {
    int fd1 = open(argv1, O_RDONLY);
    if (fd1 < 0) { perror("open f1"); return -1; }

    int fd2 = open(argv2, O_RDONLY);
    if (fd2 < 0) { perror("open f2"); close(fd1); return -1; }

    char buf1[1024], buf2[1024];
    while (1) {
        ssize_t r1_len = read(fd1, buf1, sizeof(buf1));
        if (r1_len < 0) {
            if (errno == EINTR) continue;
            perror("read f1"); close(fd1); close(fd2); return -1;
        }
        ssize_t r2_len = read(fd2, buf2, sizeof(buf2));
        if (r2_len < 0) {
            if (errno == EINTR) continue;
            perror("read f2"); close(fd1); close(fd2); return -1;
        }
        if (r1_len != r2_len) { close(fd1); close(fd2); return 1; }
        if (r1_len == 0) break;
        if (memcmp(buf1, buf2, (size_t)r1_len) != 0) { close(fd1); close(fd2); return 1; }
    }
    close(fd1); close(fd2);
    return 0;
}
int main(int argc,char *argv[]){
    if(argc != 3){
        fprintf(stderr,"使用方法:./mv 源文件路径 新路径\n");
        return 1;
    }
    if(rename(argv[1],argv[2]) == 0){
        return 0;
    }else if(errno == EXDEV){
        int n = copy(argv[1],argv[2]);
        if(n != 0)return 1;
        int m = is_same(argv[1],argv[2]);
        if(m > 0){
            fprintf(stderr,"复制检查失败!\n");
            return 1;
        }else if(m < 0){
            fprintf(stderr,"文件操作失败!\n");
            return 1;
        }else if(m == 0){
            if(unlink(argv[1]) < 0){perror("unlink"); return 1;}
            return 0;
        }
    }else{
        perror("rename");
        return 1;
    }
}