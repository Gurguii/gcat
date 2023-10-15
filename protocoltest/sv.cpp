#include <gsocket/gsocket.hpp>
#include "gurguiTCPprotocol.h"
using namespace gsocket;

#define address "127.0.0.1"
#define port 9001

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
  tcp4socket sv;
  if(sv.bind(address, port)){
    std::cerr << sockError << "\n"; return -1;
  }
  printf("LISTENING ON %s:%d\n",address,port);
  // Accept clients
  char buffer[4 * 1024 * 1024];
  uint64_t rbytes = 0, wbytes = 0, total = 0;
  for(;;)
  {
    printf("[+] accepting connections\n");
    tcp4socket client = sv.accept();

    rbytes = client.recv(buffer, sizeof(buffer));
    if(rbytes <= 0){
      printf("there was some sort of error awaiting.. %lu\nerror: %s\n",rbytes,sockError);
      break;
    }
    
    // Check if it's using the gurguiprotocol
    // TODO - encapsulate all of this in a single function
    if(!gstrcmp(buffer,(char*)BEGINNING_OF_CONNECTION_MSG,strlen(BEGINNING_OF_CONNECTION_MSG))){
      printf("not %%BEG%%...\n");
      break;
    }
    
    memset(buffer,0,rbytes);

    wbytes = client.send((char*)SERVER_OK,sizeof(SERVER_OK));
    if(wbytes<=0){
      printf("sending first SERVER_OK\n");
      break;
    }
    
    // File to keep data
    FILE *file = fopen("testingo.7z","wb");
    if(file == NULL){
      fprintf(stderr,"couldn't open testingo\n");
      return -1;
    }
    // Start reading incoming data
    while((rbytes = client.recv(buffer,sizeof(buffer))) > 0)
    {
      total += rbytes;
      // Check for %%END%%
      if(gstrcmp(buffer+strlen(buffer)-strlen(END_OF_CONNECTION_MSG),(char*)END_OF_CONNECTION_MSG,strlen(END_OF_CONNECTION_MSG))){
        memset(buffer+rbytes-strlen(END_OF_CONNECTION_MSG),0,strlen(END_OF_CONNECTION_MSG));
        fwrite(buffer,rbytes,1,file);
        break;
      }
      fwrite(buffer,rbytes,1,file);
      client.send(SERVER_OK);
      memset(buffer,0,rbytes);
    }
    memset(buffer,0,rbytes);
    printf("\n\n\r[info] total received: %lu bytes\n",total);
    total = 0;
    // Send the END_OF_CONNECTION_MSG message.
    client.send(END_OF_CONNECTION_MSG);
    client.close();
  }
}

