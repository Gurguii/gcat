#include "client.hpp"
namespace gcat
{
int start_udp_client(const char *address, uint16_t port){
  gsocket::udp4socket client;
  if(client.connect(address,port)){
    return -1;
  }
  // Check for data in STDIN 
  uint64_t available = 0;
  ioctl(0,FIONREAD,&available);
  if(available > 0){
    // data available
     
  }
  for(;;){

  }
  return 0;  
};
}
