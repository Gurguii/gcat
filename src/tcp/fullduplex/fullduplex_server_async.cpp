#include <cstdio>
#include <gsocket/gsocket.hpp>
#include <sys/epoll.h>

namespace gcat
{

template <typename SocketClass> void reader(SocketClass sock, gsocket::Pipe *pipe)
{
  // child - reader
  //pipe->closeWriter();
  uint8_t *databuff = (uint8_t *)calloc(4 * 1024 * 1024,sizeof(uint8_t));
  int epollfd = epoll_create1(0);
  uint64_t rbytes = 0; 
  epoll_event ev1{
    .events = EPOLLIN
  };
  ev1.data.fd = sock._fd;
  epoll_event ev2{
    .events = EPOLLIN,
  };
  ev2.data.fd = pipe->GetReader();
  
  epoll_ctl(epollfd,EPOLL_CTL_ADD,sock._fd,&ev1);
  epoll_ctl(epollfd,EPOLL_CTL_ADD,pipe->GetReader(),&ev2);
  
  epoll_event available[2];
  for(;;){
    printf("[+] reader waiting for events\n");
    if(epoll_wait(epollfd, available, 2,-1)){
      for(epoll_event ev : available){
        if(ev.data.fd == sock._fd){
          printf("socket fd\n");
          while((rbytes = sock.recv((char*)databuff,4*1024*1024)) > 0){
            printf("[received %i bytes] %s\n",rbytes,databuff);
            memset(databuff,0,rbytes);
          }
        }else if(ev.data.fd == pipe->GetReader()){
          // read FILE
          printf("pipe fd\n");
          pipe->read((char*)databuff,4 * 1024 * 1024);
          printf("buffer: %s - size: %lu\n",databuff,strlen((char*)databuff));
          char delims[] = " ";
          char *filename = strtok((char*)databuff,delims);
          if(strcmp(filename,"FILE")){
            continue; 
          }
          filename = strtok(nullptr,delims);
          if(filename == nullptr){
            continue;
          }
          // remove \n from filename
          *(filename+strlen(filename)-1) = '\0';
          FILE *file = fopen(filename,"wb");
          if(file == nullptr){
            printf("couldn't open file '%s'\n",filename);
            continue;
          }
          while((rbytes = sock.recv((char*)databuff,4*1024*1024)) > 0){
            if(!strcmp((char*)databuff+rbytes-4,"END\n")){
              break;
            }
            fwrite(databuff,rbytes,1,file);
            memset(databuff,0,rbytes);
          }
          printf("File '%s' saved\n", (char*)filename);
          fclose(file);
        }
      }
      memset(available,0,sizeof(epoll_event)*2);
      break;
    }
  }
  printf("connection closed\n");
}

template <typename SocketClass> int start_fullduplex_tcp_server(SocketClass sock, const char *address, uint16_t port)
{
  if(sock.bind(address, port)){
    std::cerr << sockError << "\n"; return -1;
  }

  int rbytes = 0;
  gsocket::addr addr;

  printf("LISTENING ON %s:%d\n",address,port);
  printf("ACCEPTING CONNECTIONS\n");
  gsocket::tcp4socket client = sock.accept(addr);

  if(std::holds_alternative<gsocket::addr4>(addr)){
    printf("CONNECTION FROM %s:%d\n",std::get<gsocket::addr4>(addr).host(),std::get<gsocket::addr4>(addr).port);
  }else{
    printf("CONNECTION FROM %s:%d\n",std::get<gsocket::addr6>(addr).host(),std::get<gsocket::addr6>(addr).port);
  }

  gsocket::Pipe pipe;
  uint8_t *msgbuf = (uint8_t*)calloc(1024,sizeof(uint8_t));

  // launch reader task
  std::future<void> task = std::async(std::launch::async,reader<gsocket::tcp4socket>,client,&pipe);

  int sbytes = 0;
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
      //pipe.write("FILE");
      char delims[] = " ";
      char *filename = strtok((char*)msgbuf,delims);
      filename = strtok(NULL,delims);
      if(filename == nullptr){
        std::cerr << "filename not supplied\n"; 
        continue;
      }
      std::string fname = std::string("FILE ") += filename;
      memset(msgbuf,0,1024);
      int cx = snprintf((char*)msgbuf,20,"FILE %s",filename);
      if(cx <= 0){
        printf("snprintf() failed\n");
        free(filename); 
        filename = nullptr;
        continue;
      }
      printf("writing to pipe: %s\n", fname.c_str());
      pipe.write(fname);
    }
    else{
      // send data through socket
      sbytes = client.send((char*)msgbuf); 
      printf("sent %i bytes\n",sbytes);
      if(sbytes <= 0){
        break;
      }
    }
  }
  free(msgbuf);
  } 
}
