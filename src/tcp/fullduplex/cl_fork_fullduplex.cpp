#include <cstdio>
#include <gsocket/gsocket.hpp>
#include <string>
using namespace gsocket;

int main()
{
  tcp4socket sock;
  if(sock.connect("127.0.0.1", 9001)){
    std::cerr << sockError << "\n"; return -1;
  }
  if(fork() == 0){
    // child - reader
    uint64_t rbytes = 0;
    uint8_t *buff = (uint8_t*)calloc(4 * 1024 * 1024,sizeof(uint8_t)); // 4MB buffer
    while((rbytes = sock.recv((char*)buff,4 * 1024 * 1024)) > 0){
      printf("[server] - %s\n",buff);
      memset(buff,0,rbytes);
    }
  }else{
    // parent - writer
    char *msg = (char*)calloc(1024,sizeof(char));
    uint64_t sbytes = 0;
    for(;;){
      printf("[client] - ");
      fgets(msg,1024,stdin);
      sbytes = sock.send(msg);
      if(sbytes <= 0){
        break;
      }
      memset(msg,0,1024);
    }
  }
}
