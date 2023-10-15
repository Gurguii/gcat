#include <cstdio>
#include <gsocket/gsocket.hpp>

using namespace gsocket;

#define BEGINNING "%%BEG%%"
#define ENDOFMSG "%%END%%"
/* Gurguiprotocol
* client - connect() & send("%%BEG%%");
* server - send("OK");
* client - send("%%<SIZE>%%");
* server - send("READY");
* client - send() ...
* server - read() ...
* client - send("%%END%%");
* server - send("OK.END");
* client - send("OK.END");
* */
bool is_first_message(char *buffer, uint64_t bytes)
{
  char check[sizeof(BEGINNING)];
  if(bytes > sizeof(BEGINNING)){
    memcpy(check,buffer,sizeof(check)); 
    if(strcmp(check,BEGINNING) == 0){
      return 1; 
    }
  }
  return 0;
}
// 4096 / 8192 / 16384
int main(int argc, const char **args)
{
  tcp4socket server;
  if(server.bind("127.0.0.1", 9001)){
    fprintf(stderr,"bind() - %s\n", sockError); return -1;
  }
  char buff[4194304]; // 4MB
  int rbytes;
  for(;;)
  {
    printf("[+] accepting connection ...\n");
    tcp4socket client = server.accept(); 

    // 1 - check for first message 'mark' 
    rbytes = client.recv(buff,strlen(BEGINNING));
    if(strcmp(buff,BEGINNING))
    {
      printf("not a valid message\n%s %lu\n%s %lu\n",buff,strlen(buff),BEGINNING,strlen(BEGINNING));
      return -1;
    }

    // 2 - check for size 'mark'

    for(;;)
    {
      rbytes = client.recv(buff, sizeof(buff));
      if(rbytes > 0){
        printf("[+] Read %i bytes: %s\n", rbytes, buff);
        memset(&buff[0],0,rbytes);
      }else{
        printf("[-] Client closed connection...\n");
        break;
      }
    }
    client.close();
  }
  server.close();
}
