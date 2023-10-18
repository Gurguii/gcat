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

// TODO - add cleanup function
// 4Mb
char buffer[4 * 1024 * 1024];

int main()
{
  tcp4socket sv;
  if(sv.bind(address, port)){
    std::cerr << sockError << "\n"; return -1;
  }
  printf("LISTENING ON %s:%d\n",address,port);
  // Accept clients
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
    
    // 1. Look for %%BEG%% 
    if(!gstrcmp(buffer,(char*)BEGINNING_OF_CONNECTION_MSG,strlen(BEGINNING_OF_CONNECTION_MSG))){
      printf("not %%BEG%%...\n");
      break;
    }
    memset(buffer,0,rbytes);

    // 2. Send OK to client
    wbytes = client.send((char*)SERVER_OK,sizeof(SERVER_OK));
    if(wbytes<=0){
      printf("sending first SERVER_OK\n");
      break;
    }

    // 3. Receive message length from client 
    rbytes = client.recv(buffer,sizeof(buffer));
    if(rbytes <= 0){
      printf("[error] receiving message size - recv() returned %lu\n",rbytes);
      break;
    }
    char delims[] = " ";
    char *result = strtok(buffer,delims);
    result = strtok(NULL,delims);
    uint64_t msglen = strtol(result,NULL,10);
    char server_ok_size[100];
    memset(server_ok_size,0,sizeof(server_ok_size));

    if(msglen <= 0){
      printf("[error] message len <= 0 - %lu\n",msglen);
      break;
    }

    // 4. Send OK <size> to client
    int cx = snprintf(server_ok_size,sizeof(server_ok_size),"%s %lu\n",SERVER_OK,msglen);
    if(cx <= 0){
      printf("[error] - snprintf() returned %i\n",cx);
      return -1;
    }
    wbytes = client.send(server_ok_size,sizeof(server_ok_size));
    if(wbytes <= 0){
      printf("[error] - sending OK <size> returned %lu\n",wbytes);
      break;
    }

    // 5. Start reading data from client until recv() returns <= 0
    while(total < msglen)
    {
      rbytes = client.recv(buffer,sizeof(buffer));
      total += rbytes;
      // Not end, print data to terminal and send OK <size> to client
      write(0,buffer,rbytes);
      int cx = snprintf(server_ok_size,sizeof(server_ok_size),"%s %lu",SERVER_OK,rbytes);
      if( cx <= 0){
        printf("[error] snprintf() - %lu\n",rbytes);
        break;
      };
      // 5.2 send OK <size> to client 
      printf("sending to client - %s\n",server_ok_size);
      wbytes = client.send(server_ok_size,sizeof(server_ok_size)-1);
      if(wbytes <= 0){
        printf("[error] sending server_ok_size - %lu\n",wbytes);
        break;
      }
      memset(server_ok_size,0,cx);
      memset(buffer,0,rbytes);
    }

    // 6. Wait for %%END%%
    rbytes = client.recv(buffer,sizeof(buffer));
    if(rbytes <= 0){
      printf("[error] step 6 - recv() returned %lu\n",rbytes);
      break;
    }
    
    if(!gstrcmp(buffer+strlen(buffer)-strlen(END_OF_CONNECTION_MSG),(char*)END_OF_CONNECTION_MSG,strlen(END_OF_CONNECTION_MSG))){
      printf("[error] didn't receive %%END%%, instead received: %s\n",buffer);
      break;
    }

    // 7. Send DONE and close connection
    memset(buffer,0,rbytes);
    client.send(CONNECTION_DONE);
    client.close();
    total = 0;
  }
}
