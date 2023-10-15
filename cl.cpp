#include <cstdio>
#include <gsocket/gsocket.hpp>
#include <ulimit.h>

using namespace gsocket;

bool is_stdin_empty(uint64_t *bytes) {
  ioctl(0,FIONREAD, bytes);
  return (*bytes > 0 ? false : true);
}

int main()
{
  //uint64_t b = 0;
  //if(is_stdin_empty(&b)){
  //  printf("stdin is empty...\n"); return -1;
  //};
  char buff[4194304];
  tcp4socket sock;
  if(sock.connect("127.0.0.1", 9001)){
    std::cerr << sockError << "\n"; return -1;
  }
  // connected
  int wbytes = 0;
  for(;;)
  {
    printf("msg: ");
    fgets(buff,sizeof(buff),stdin);
    wbytes = sock.send(buff,strlen(buff));
    printf("sent %i bytes\n", wbytes);
    if(wbytes <= 0){ break; }
  }
  return 0;
}
