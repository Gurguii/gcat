#include <gsocket/gsocket.hpp>
using namespace gsocket;

void reader(tcp4socket sock)
{
  uint8_t *buffer = (uint8_t*)calloc(1024 * 1024 * 4,sizeof(uint8_t));
  int rbytes = 0;
  for(;;){
    rbytes = sock.recv((char*)buffer,4*1024*1024);
    if(rbytes <= 0){
      break;
    }
    printf("%s\n",buffer);
  }
  free(buffer);
  buffer = nullptr;
}
int main()
{
  tcp4socket cl;
  if(cl.connect("127.0.0.1", 9001)){return -1;}
  std::future<void> task = std::async(std::launch::async,reader,cl);
  char *msg = (char*)malloc(1024);
  int sbytes = 0;
  memset(msg,0,1024);
  for(;;){
    printf("[client] - ");
    fgets(msg,1024,stdin);
    sbytes = cl.send(msg);
    if(sbytes <= 0){
      break;
    }
    memset(msg,0,sbytes);
  }
  free(msg);
  msg = nullptr;
  task.wait();
  return 0;
}
