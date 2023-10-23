#include <cstdio>
#include <gsocket/gsocket.hpp>
#include <sys/epoll.h>
using namespace gsocket;

int main()
{
  tcp4socket sock;
  if(sock.connect("127.0.0.1", 9001)){
    std::cerr << sockError << "\n"; return -1;
  }
  addr4 client_addr;
  int rbytes = 0;
  Pipe pipe;
  if(fork() == 0){
    // child - reader
    pipe.closeWriter();
    uint8_t *databuff = (uint8_t *)calloc(4 * 1024 * 1024,sizeof(uint8_t));
    int epollfd = epoll_create1(0);
    
    epoll_event ev1{
      .events = EPOLLIN
    };
    ev1.data.fd = pipe.GetReader();
    epoll_event ev2{
      .events = EPOLLIN,
    };
    ev2.data.fd = sock._fd;
    
    epoll_ctl(epollfd,EPOLL_CTL_ADD,sock._fd,&ev1);
    epoll_ctl(epollfd,EPOLL_CTL_ADD,pipe.GetReader(),&ev2);
    
    epoll_event available[2];
    printf("hahah\n");
    for(;;){
      printf("[+] reader waiting for events\n");
      if(epoll_wait(epollfd, available, 2,-1)){
        printf("got event");
        for(epoll_event ev : available){
          if(ev.data.fd == sock._fd){
            while((rbytes = sock.recv((char*)databuff,4*1024*1024)) > 0){
              printf("[received %i bytes] %s\n",rbytes,databuff);
              memset(databuff,0,rbytes);
            }
          }else if(ev.data.fd == pipe.GetReader()){
            // read FILE
            std::string bufer(1024,'\x00');
            pipe.read(bufer);
            if(bufer == "FILE"){
              printf("PIPE TOLD US TO READ A FILE\n");
            }
          }
        }
        memset(available,0,sizeof(epoll_event)*2);
      }
    }
    printf("connection closed\n");
  }else{
    uint8_t *msgbuf = (uint8_t*)calloc(1024,sizeof(uint8_t));
    int sbytes = 0;
    pipe.closeReader();
    // parent - writer
    for(;;){
      memset(msgbuf,0,1024);
      fgets((char*)msgbuf,1024,stdin);
      if(*msgbuf == '!'){
        // send command locally
        system((const char*)msgbuf+1);
      }else if(*msgbuf == '<'){
        char delims[] = " ";
        char *result = strtok((char*)msgbuf,delims);
        result = strtok(NULL,delims);
        if(result == nullptr){
          std::cerr << "< 'file'\n";
        }
        FILE *file = fopen(std::string(result,result+strlen(result)-1).c_str(),"rb");
        if(file == nullptr){
          std::cerr << "couldn't open file " << result << "\n";
          continue;
        }
        sbytes = sock.send(file);
        printf("sent '%s' Size: '%i' bytes\n",result, sbytes);
        fclose(file);
      }else if(*msgbuf == '>'){
        pipe.write("FILE");
        char delims[] = " ";
        char *result = strtok((char*)msgbuf,delims);
        uint8_t *databuff = (uint8_t*)calloc(4 * 1024 * 1024, sizeof(uint8_t));
        result = strtok(NULL,delims);
        if(result == nullptr){
          std::cerr << "filename not supplied\n"; 
          continue;
        }
        FILE *file = fopen(result,"wb");
        if(file == nullptr){
          fprintf(stderr,"couldn't open file '%s'\n", result);
          continue;
        }
        while((rbytes = sock.recv((char*)databuff,4*1024*1024)) > 0){
          fwrite(databuff,rbytes,1,file);
          memset(databuff,0,rbytes);
        }
        printf("data written to file '%s'\n",result);
        free(databuff);
        free(result);
        databuff = nullptr;
        result = nullptr;
        fclose(file);
      }
      else{
        // send data through socket
        sbytes = sock.send((char*)msgbuf); 
        printf("sent %i bytes\n",sbytes);
        if(sbytes <= 0){
          break;
        }
      }
    }
    free(msgbuf);
  } 
  sock.close();

  return 0;
}
