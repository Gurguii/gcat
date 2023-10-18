#include <gsocket/gsocket.hpp>
#include <string.h>
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

// TODO - add cleanup
unsigned char buffer[4 * 1024 * 1024]; // 4MB

int main()
{
  memset(buffer,0,sizeof(buffer));
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

  uint64_t wbytes = 0, rbytes = 0, total = 0;
  // 1. Send %%BEG%% to server
  wbytes = client.send((char*)BEGINNING_OF_CONNECTION_MSG,strlen(BEGINNING_OF_CONNECTION_MSG));
  if(wbytes <= 0){
    printf("[error] step 1. - send() returned %lu\n",wbytes);
    return -1;
  }
  // 2. Wait OK from server
  rbytes = client.recv((char*)buffer,sizeof(buffer));
  if(rbytes <= 0){
    printf("[error] step 2. - recv() returned %lu\n",wbytes);
    return -1;
  }
  if(!gstrcmp((char*)buffer,(char*)SERVER_OK,strlen(SERVER_OK))){
    printf("[error] step 2. - expected %s, instead got %s\n",SERVER_OK,buffer);
    return -1;
  }
  memset(buffer,0,sizeof(buffer));
  // 3. Send SIZE <size> to server
  char message_size[100];
  memset(message_size,0,sizeof(message_size));
  int cx = snprintf(message_size,sizeof(message_size),"SIZE %lu",bytes);
  if(cx <= 0){
    printf("[error] snprintf() returned %i\n",cx);
    return -1;
  }
  wbytes = client.send(message_size);
  if(wbytes <= 0){
    printf("[error] - step 3. send() SIZE <size> returned %lu\n",wbytes);
    return -1;
  }
  // 4. Receive server's `OK <size>`
  char server_message_size[80];
  char delim[] = " ";
  rbytes = client.recv((char*)buffer,sizeof(buffer));
  if(rbytes <= 0){
    printf("rbytes returned %lu\n",rbytes);
    return -1;
  }
  char *result = strtok((char*)buffer,delim); 
  if(result == nullptr || (result = strtok(NULL,delim)) == nullptr){
    printf("[error] step 3. strok() returned NULL\n");
    return -1;
  }
  uint64_t server_msg_size = strtol(result,NULL,10);
  if(server_msg_size <= 0){
    printf("[error] strtol() returned %lu\n", server_msg_size);
    return -1;
  }
  if(server_msg_size != bytes){
    printf("[error] server_msg != bytes ~ %lu != %lu\n",server_msg_size,bytes);
    return -1;
  }
  memset(server_message_size,0,sizeof(server_message_size));
  // 5. Start sending data
  if(bytes < MAX_PACKET_LENGTH){
    // Data can be sent in 1 chunk, go for it
    if(read(0,buffer,bytes) <= 0){
      printf("[error] - step 5. - read(stdin) returned <= 0");
      return -1;
    };
    // Buffer succesfully populated with STDIN data
    wbytes = client.send((char*)buffer,bytes);
    if(wbytes <= 0){
      printf("[error] sending data in 1 go returned %lu\n",wbytes);
      return -1;
    }
    // Wait for server OK <size>
    rbytes = client.recv((char*)buffer,sizeof(buffer));
    if(rbytes <= 0){
      printf("[error] receiving server OK returned %lu\n",rbytes);
      return -1;
    }
    result = strtok((char*)buffer,delim);
    if(result == nullptr || (result = strtok(nullptr,delim)) == nullptr){
      printf("[error] strtok() returned nullptr Aa\n");
      return -1;
    }
    server_msg_size = strtol(result,NULL,10);
    if(server_msg_size != wbytes){
      printf("[error] server reported %lu bytes when %lu were sent\n",server_msg_size,wbytes);
      return -1;
    }
  }else{
    printf("woski\n");
    // Data has to be sent in +1 send() since its more data than what max tcp packet length
    // uint16_t;
    unsigned char *buffptr = buffer;while(read(0,buffptr,sizeof(buffer))){
    while(read(0,buffer,sizeof(buffer)) >= 0){
      while((wbytes = client.send((char*)buffptr,MAX_PACKET_LENGTH))){
        buffptr+=MAX_PACKET_LENGTH;
        memset(buffer,0,sizeof(buffer));
        // 5.2 Wait for server OK <size> 
        rbytes = client.recv((char*)buffer,sizeof(buffer));
        if(rbytes <= 0){
          printf("[error] receiving server OK returned %lu\n",rbytes);
          return -1;
        }
        result = strtok((char*)buffer,delim);
        if(result == nullptr){
          printf("first strtok returned NULL\n");
          return -1;
        } 
        result = strtok(nullptr,delim);
        if(result == nullptr){
          printf("second result returned NULL\n");
          return -1;
        }
        server_msg_size = strtol(result,NULL,10);
        if(server_msg_size != wbytes){
          printf("[error] server reported %lu bytes when %lu were sent\n",server_msg_size,wbytes);
          return -1;
        } 
        result = nullptr;
        memset(buffer,0,sizeof(buffer));
      }
    }
  }
}
  memset(buffer,0,sizeof(buffer));
  // 6. Send %%END%%
  wbytes = client.send((char*)END_OF_CONNECTION_MSG,strlen(END_OF_CONNECTION_MSG));
  if(wbytes <= 0){
    printf("[error] - sending %%END%% returned %lu\n", wbytes);
    return -1;
  }
  // 7. Wait for server DONE and close connection
  rbytes = client.recv((char*)buffer,sizeof(buffer));
  if(rbytes <= 0){
    printf("[error] - receiving DONE from server returned %lu\n",rbytes);
    return -1;
  }
  if(!gstrcmp((char*)buffer,(char*)CONNECTION_DONE,strlen(CONNECTION_DONE))){
    printf("[error] - expected %s and got %s\n",CONNECTION_DONE, buffer);
  }
  printf("DONE\n");
  client.close();
  return 0;
}
