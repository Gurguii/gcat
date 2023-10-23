#include <gsocket/gsocket.hpp>

using namespace gsocket;

int main()
{
  tcp4socket sock;
  if(sock.bind("127.0.0.1", 9001)){
    std::cerr << sockError << "\n"; return -1;
  }
  printf("accepting connections\n");
  addr4 _cl;
  tcp4socket client = sock.accept(_cl);
  printf("CONNECTION FROM %s:%d\n",_cl.host(),_cl.port);
  if(fork() == 0){
    // child - reader
    uint64_t rbytes = 0;
    uint8_t *buff = (uint8_t*)calloc(4 * 1024 * 1024,sizeof(uint8_t)); // 4MB buffer
    while((rbytes = client.recv((char*)buff,4 * 1024 * 1024)) > 0){
      printf("[client] - %s\n",buff);
      memset(buff,0,rbytes);
    }
  }else{
    // parent - writer
    char *msg = (char*)calloc(1024,sizeof(char));
    uint64_t sbytes = 0;
    for(;;){
      printf("[server] - ");
      sbytes = read(0,msg,1024);
      if(sbytes <= 0){
        break;
      }
      client.send(msg);
      memset(msg,0,1024);
    }
  }
}
