#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(void) {
    char http[1024];
    write(1,"输入网址:\n",sizeof("输入网址:\n"));
    fgets(http,sizeof(http),stdin);
    http[strcspn(http,"\n")] = '\0';
    int server = socket(AF_INET,SOCK_STREAM,0);
    if (server < 0) {
      perror("socket");
      return 1;
    }
    struct hostent *host = gethostbyname(http);
    if(host == NULL){
      herror("gethostbyname");
      close(server);
      return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    memcpy(&addr.sin_addr.s_addr,host->h_addr,host->h_length);
    if(connect(server,(struct sockaddr *)&addr,sizeof(addr)) == 0){
      char s_send[1024];
      snprintf(s_send,sizeof(s_send),
               "GET / HTTP/1.1\r\n"
               "Host: %s\r\n"
               "Connection: close\r\n"
               "\r\n",http);
      size_t total = 0;
      size_t n_len = strlen(s_send);
      while(total < n_len){
        ssize_t n = send(server,s_send + total,(size_t)(n_len - total),0);
        if(n < 0){
          if(errno == EINTR)continue;
          perror("send");
          close(server);
          return 1;
        }
        total += (size_t)n;
      }
      char buf[1024];
      while(1){
        ssize_t n = recv(server,buf,sizeof(buf),0);
        if(n < 0){
          if(errno == EINTR)continue;
          perror("recv");
          close(server);
          return 1;
        }
        if(n == 0)break;
        write(1,buf,n);
      }
    }else{
      perror("connect");
      close(server);
      return 1;
    }
    close(server);   
    return 0;
}