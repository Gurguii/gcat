#include "server.hpp"
namespace gcat
{
int start_tcp_server_no_protocol(const char *address, uint16_t port)
{
  uint8_t *buffer = (uint8_t*)calloc(4 * 1024 * 1024, sizeof(uint8_t));
  if(buffer == nullptr){
    fprintf(stderr,"[error] couldn't allocate 4mb for buffer\n");
    return -1;
  }

  gsocket::tcp4socket sock;
  sock.bind(address, port);
  gsocket::addr _add;
  sock.get_address(_add);
  fprintf(stderr,"[+] Listening on %s:%d\n", address, port);

  uint64_t rbytes = 0, total_rbytes = 0;
  gsocket::addr4 client_addr;
  for(;;)
  {
    fprintf(stderr,"[+] accepting connection\n");
    gsocket::tcp4socket client = sock.accept(client_addr);
    printf("CONNECTION FROM %s:%d\n",client_addr.host(),client_addr.port);
    for(;;)
    {
      rbytes = client.recv((char*)buffer,(4*1024*1024));
      if(rbytes <= 0){
        break;
      } 
      write(1,buffer,rbytes);
      memset(buffer,0,rbytes);
    }
    client.close();
    memset(buffer,0,4*1024*1024);
  }
}
} // namespace gcat
