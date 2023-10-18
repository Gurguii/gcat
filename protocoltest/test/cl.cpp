// TODO - make a copy of this file and after filling the 4mb buffer, start sending
// blocks of 64 * 1024 (64kilobytes) so that it fits in the tcp packet.
#include <gsocket/gsocket.hpp>
#define max_data_len 64000

using namespace gsocket;

char buffer[4*1024*1024]{0};
char *buffptr = buffer;
uint64_t available = 0;
uint64_t sbytes = 0;

int main()
{
  tcp4socket client;
  client.connect("127.0.0.1", 9001);

  ioctl(0,FIONREAD,&available);
  if(available)
  {
    if(available > sizeof(buffer)){
      // Send by chunks
      int iters = available/sizeof(buffer);
      int rest = available%sizeof(buffer);
      int _iters = sizeof(buffer)/max_data_len;
      int _rest = sizeof(buffer)%max_data_len;

      for(int i = 0; i < iters; ++i){
        read(0,buffer,sizeof(buffer));
        for(int n = 0; n < _iters; ++n){
          sbytes = client.send(buffptr,max_data_len);
          //buffptr+=max_data_len;
        }
        sbytes = client.send(buffptr,rest);
      }

      read(0,buffer,rest);
      client.send(buffer);
      memset(buffer,0,rest);
    }else{
      read(0,buffer,available);
      client.send(buffer,sizeof(buffer));
      memset(buffer,0,available);
    }
  }else{
    // Load /etc/passwd file
    FILE *file = fopen("/etc/passwd", "r");
    fseek(file,0,SEEK_END);
    uint64_t size = ftell(file);
    fseek(file,0,SEEK_SET);
    if(file == nullptr){
      fprintf(stderr,"couldn't open file\n");
      return -1;
    }
    fread(buffer,sizeof(buffer),1,file);
    client.send(buffer,size);
  }
  // Wait for socket to be empty in order to close
  uint64_t check = 0;
  for(;;)
  {
    ioctl(client._fd,FIONREAD,&check);
    if(check == 0){
      printf("0 to read\n");
      break;
    } 
    printf("still data available: %lu\n", check);
    sleep(1);
  }
  printf("Total sent: %lu\n",available);
  client.close();
}
