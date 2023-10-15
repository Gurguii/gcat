#include <gsocket/gsocket.hpp>
#include "gurguiTCPprotocol.h"
using namespace gsocket;

bool gstrcmp(char *buff1, char* buff2, int n){
  for(int i = 0; i < n; ++i)
  {
    if(*(buff1+i) != *(buff2+i)){
      return false;
    }
  }
  return true;
}

int main()
{
  uint64_t bytes = 0;
  ioctl(0,FIONREAD,&bytes);
  if(bytes <= 0){
    std::cerr << "[!] no data available in STDIN\n";
    return -1;
  }
  tcp4socket client;
  if(client.connect("127.0.0.1", 9001)){
    std::cerr << sockError << "\n";
    return -1;
  }

  char buff[4 * 1024 * 1024]; // 4MB
  uint64_t wbytes = 0, rbytes = 0, total = 0;

  wbytes = client.send(BEGINNING_OF_CONNECTION_MSG);
  if(wbytes <= 0){
    std::cerr << sockError << "\n"; return -1;
  }
  printf("[info] waiting OK from server\n");
  rbytes = client.recv(buff, sizeof(buff));
  if(!gstrcmp(buff,(char*)SERVER_OK,strlen(SERVER_OK))){
    std::cerr << "Didn't receive OK from server\n";
    return -1;
  }
  printf("[info] received OK from server, sending data...\n");
  // start sending data
  
  // if data fits in 1 buffer,
  // send it all at once, else
  // sent it in chunks
  if(bytes <= sizeof(buff)){
    read(0,buff,bytes);
    if(client.send(buff,bytes) <= 0){
      fprintf(stderr,"[error] sending data...\n"); return -1;
    };
    memset(buff,0,bytes);
  }else{
    int iters = bytes / sizeof(buff);
    int rest = bytes % sizeof(buff);
    char ans[1024];
    for(int i = 0; i < iters; ++i){
      rbytes = read(0,buff,sizeof(buff));
      if(rbytes <= 0){
        std::cerr << "smth happened reading stdin... - errcode: " << wbytes;
        break;
      }
      wbytes = client.send(buff);
      if(wbytes <= 0){
        std::cerr << "smth happened sending data... - errcode: " << wbytes;
        break;
      }
      memset(buff,0,wbytes);
      client.recv(ans,sizeof(ans));
      if(strcmp((char*)SERVER_OK,ans)){
        // not good
        std::cerr << "didn't receive OK from server, instead received " << ans << "\n";
        break;
      }
      memset(ans,0,sizeof(ans));
    }
    // send last chuk
    memset(buff,0,sizeof(buff));
    read(0,buff,rest);
    wbytes = client.send(buff,rest);
    if(wbytes < 0){
      std::cerr << "error sending rest ... " << sockError << "\n";
      return -1;
    } 
    memset(buff,0,wbytes);
  }
  printf("[info] sending END OF CONNECTION\n");
  if(client.send(END_OF_CONNECTION_MSG) <= 0){
    std::cerr << "error sending END OF CONNECTION\n";
    return -1;
  };
  memset(buff,0,sizeof(buff));
  printf("[info] waiting OK_END from server\n");
  int n = client.recv(buff,10);
  if(!gstrcmp(buff,(char*)SERVER_OK,3)){
    std::cerr << "not equal: " << buff << " : " << SERVER_END << "\n";
  }
  client.close();
  printf("[info] data succesfully sent\n");
  client.close();
  return 0;
}
