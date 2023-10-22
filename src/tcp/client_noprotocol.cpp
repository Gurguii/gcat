#include "client.hpp"
namespace gcat
{

int start_tcp_client_no_protocol(const char *address, uint16_t port){
  uint64_t available = 0;
  ioctl(0,FIONREAD,&available);
  if(available <= 0){
    std::cerr << "available: " << available << "\n"; return -1;
  }
  uint8_t *buffer = (uint8_t*)calloc(available,sizeof(uint8_t));
  if(buffer == nullptr){
    std::cerr << "couldn't allocate '" << available << "' bytes\n"; return -1;
  }
  if(read(0,buffer,available) <= 0){
    std::cerr << "error reading stdin to buffer\n"; return -1;
  };
  
  gsocket::tcp4socket sock;
  if(sock.connect(address, port)){
    std::cerr << "connection refused\n"; return -1;
  };
  
  uint64_t sbytes = 0;
  uint64_t total_sbytes = 0;
  sbytes = sock.send((char*)buffer,available);
  uint64_t left = available - sbytes;
  
  if(left){
    printf("%lu bytes left to send\n",left);
    sbytes = sock.send((char*)buffer+sbytes,left);
    if(sbytes != left){
      fprintf(stderr,"couldn't send last %lu bytes\n", left);
    }
  }
  
  // Cleanup
  memset(buffer,0,available);
  free(buffer);
  buffer = nullptr;
  sock.close();
  return 0;
  }
} // namespace gcat
