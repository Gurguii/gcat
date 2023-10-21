#include "server.hpp"

namespace gcat
{
char server_buffer[4 * 1024 * 1024];

int start_tcp_server(const char *address, uint16_t port)
{
  gsocket::tcp4socket sv;
  if(sv.bind(address, port)){
    std::cerr << sockError << "\n"; return -1;
  }

  char *listenmsg = (char*)malloc(200);
  memset(listenmsg,0,strlen(listenmsg));
  snprintf(listenmsg,200,"LISTENING ON %s:%d\n",address,port);
  write(0,listenmsg,strlen(listenmsg));
  free(listenmsg);
  listenmsg = nullptr;

  // Accept clients
  uint64_t rbytes = 0, wbytes = 0, total = 0;
  for(;;)
  {
    write(0,"[+] accepting connections\n",26);
    gsocket::tcp4socket client = sv.accept();

    rbytes = client.recv(server_buffer, sizeof(server_buffer));
    if(rbytes <= 0){
      printf("there was some sort of error awaiting.. %lu\nerror: %s\n",rbytes,sockError);
      break;
    }
    
    // 1. Look for %%BEG%% 
    if(!gstrcmp(server_buffer,(char*)BEG_MSG,strlen(BEG_MSG))){
      printf("not %%BEG%%...\n");
      break;
    }
    memset(server_buffer,0,rbytes);

    // 2. Send OK to client
    wbytes = client.send((char*)OK_MSG,sizeof(OK_MSG));
    if(wbytes<=0){
      printf("sending first OK_MSG\n");
      break;
    }

    // 3. Receive message length from client 
    rbytes = client.recv(server_buffer,sizeof(server_buffer));
    if(rbytes <= 0){
      printf("[error] receiving message size - recv() returned %lu\n",rbytes);
      break;
    }
    char delims[] = " ";
    char *result = strtok(server_buffer,delims);
    result = strtok(NULL,delims);
    uint64_t msglen = strtol(result,NULL,10);
    char server_ok_size[100];
    memset(server_ok_size,0,sizeof(server_ok_size));

    if(msglen <= 0){
      printf("[error] message len <= 0 - %lu\n",msglen);
      break;
    }

    // 4. Send OK <size> to client
    int cx = snprintf(server_ok_size,sizeof(server_ok_size),"%s %lu\n",OK_MSG,msglen);
    if(cx <= 0){
      printf("[error] - snprintf() returned %i\n",cx);
      return -1;
    }
    wbytes = client.send(server_ok_size,sizeof(server_ok_size));
    if(wbytes <= 0){
      printf("[error] - sending OK <size> returned %lu\n",wbytes);
      break;
    }
    
    char sendbuff[100];
    memset(sendbuff,0,sizeof(sendbuff));
    // 5. Start reading data from client until recv() returns <= 0
    while(total < msglen)
    {
      rbytes = client.recv(server_buffer,sizeof(server_buffer));
      total += rbytes;
      // Not end, print data to terminal and send OK <size> to client
      printf("%s",server_buffer);
      // fwrite(server_buffer, rbytes, 1, stdout);
      // fflush(stdout);
      int cx = snprintf(server_ok_size,sizeof(server_ok_size),"%s %lu",OK_MSG,rbytes);
      if( cx <= 0){
        printf("[error] snprintf() - %lu\n",rbytes);
        break;
      };
      // 5.2 send OK <size> to client 
      int cx2 = snprintf(sendbuff,sizeof(sendbuff),"sending to client - %s\n",server_ok_size);
      write(0,sendbuff,cx2);

      wbytes = client.send(server_ok_size,sizeof(server_ok_size)-1);
      if(wbytes <= 0){
        printf("[error] sending server_ok_size - %lu\n",wbytes);
        break;
      }
      memset(server_ok_size,0,cx);
      memset(server_buffer,0,rbytes);
      memset(sendbuff,0,cx2);
    }
    // 6. Wait for %%END%%
    rbytes = client.recv(server_buffer,sizeof(server_buffer));
    if(rbytes <= 0){
      printf("[error] step 6 - recv() returned %lu\n",rbytes);
      break;
    }
    
    if(!gstrcmp(server_buffer+strlen(server_buffer)-strlen(END_MSG),(char*)END_MSG,strlen(END_MSG))){
      printf("[error] didn't receive %%END%%, instead received: %s\n",server_buffer);
      break;
    }

    // 7. Send DONE and close connection
    memset(server_buffer,0,rbytes);
    client.send(END_MSG);
    client.close();
    total = 0;
  }
  return 0;
}
} // namespace gcat
