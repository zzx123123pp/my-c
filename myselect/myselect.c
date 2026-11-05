#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(void) {
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd < 0){
      perror("socket");
      return 1;
    }
    int opt = 1;
    if (setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) < 0) {
      perror("setsocketopt");
      close(server_fd);
      return 1;
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(server_fd,(struct sockaddr *)&addr,sizeof(addr)) < 0){
      perror("bind");
      close(server_fd);
      return 1;
    }
    if(listen(server_fd,10) < 0){
      perror("listen");
      close(server_fd);
      return 1;
    }
    fd_set master_set,read_set;
    FD_ZERO(&master_set);
    FD_SET(server_fd,&master_set);
    int max_fd = server_fd;
    while(1){
      read_set = master_set;
      if(select(max_fd + 1,&read_set,NULL,NULL,NULL) < 0){
        if(errno == EINTR)continue;
        perror("select");
        close(server_fd);
      }
      for(int fd = 0;fd <= max_fd;fd++){
        if(!FD_ISSET(fd,&read_set))continue;
        if(fd == server_fd){
          int client = accept(server_fd,NULL,NULL);
          if(client < 0){
            perror("accept");
            continue;
          }
          if(client > max_fd){
            max_fd = client;
          }
          FD_SET(client,&master_set);
          printf("%d加入了服务器\n",client);
        }else{
          char buf[1024];
          ssize_t n = recv(fd,buf,sizeof(buf)-1,0);
          if(n < 0){
            if(errno == EINTR)continue;
            perror("recv");
            FD_CLR(fd,&master_set);
            printf("用户%d掉线\n",fd);
          }else if(n == 0){
            FD_CLR(fd,&master_set);
            printf("用户%d下线\n",fd);
          }else{
            buf[n] = '\0';
            printf("用户%d:%s",fd,buf);
          }
        }
      }
      
    }
    return 0;
}