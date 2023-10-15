#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#define host "127.0.0.1"
#define port 9001

int main()
{
  time_t start = time(NULL);
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0){
    fprintf(stderr,"error creating socket...\n");return -1;
  }
  struct sockaddr_in addr;
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  if(!inet_pton(AF_INET, host, &addr.sin_addr)){
    fprintf(stderr, "%s\n", strerror(errno)); return -1;
  }
  if(connect(sock,(struct sockaddr*)&addr, sizeof(addr))){
    fprintf(stderr, "%s\n", strerror(errno)); return -1;
  }
  char buff[1024];
  char msg[] = "Hello its gurguisocket\n";
  int sbytes, rbytes;
  for(int i = 0; i < 100000; ++i){
    sbytes = send(sock, msg, sizeof(msg), 0);
    if(sbytes > 0){
      printf("sent %i bytes\n", sbytes);
    }
    rbytes = recv(sock, msg, sizeof(msg),0);
    if(rbytes > 0){
      printf("read %i bytes: %s\n", rbytes, buff);
      memset(buff,0,rbytes);
    }
  }
  close(sock);
  time_t end = time(NULL);
  double exec_time = (end - start) / (double)CLOCKS_PER_SEC * 1000;
  printf("%fms",exec_time);
}
