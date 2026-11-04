#include<stdio.h>
#include<sys/socket.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
int main(int argc,char *argv[]){
    if(argc != 2){
        fprintf(stderr,"使用方法:./client 网站名");
        return 1;
    }
    int client = socket(AF_INET,SOCK_STREAM,0);
    if(client < 0){
        perror("socket");
        return 1;
    }
    struct hostent *src = gethostbyname(argv[1]);
    if(src == NULL){
        fprintf(stderr,"该网站无法访问!\n");
        close(client);
        return 1;
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    memcpy(&addr.sin_addr.s_addr,src->h_addr,src->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);    
    if(connect(client,(struct sockaddr *)&addr,sizeof(addr)) == 0){
        char buf[1024];
        snprintf(buf,sizeof(buf),
        "GET / HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        argv[1]);
        size_t buf_len = strlen(buf);
        size_t total_len = 0;
        while(total_len < buf_len){
            ssize_t n = send(client,buf + total_len,buf_len - total_len,0);
            if(n < 0){
                if(errno == EINTR)continue;
                perror("send");
                close(client);
                return 1;
            }
            total_len += (size_t)n;
        }
        errno = 0;
        char str_re[4096];
        while(1){
            ssize_t n = recv(client,str_re,sizeof(str_re),0);
            if(n < 0){
                if(errno == EINTR)continue;
                perror("recv");
                close(client);
                return 1;
            }
            if(n == 0)break;
            write(1,str_re,n);
        }
    }else {
        perror("connect");
        close(client);
        return 1;
    }
    close(client);
    return 0;
}