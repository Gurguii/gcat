#include <gsocket/gsocket.hpp>
using namespace gsocket;
#define host "127.0.0.1"
#define port 9001
int main()
{
  tcp4socket sv;
  sv.bind(host, port);
  char buffer[4 * 1024 * 1024];
  int sbytes = 0;
  uint64_t total = 0;
  fprintf(stderr,"Listening on %s:%d\n",host,port);
  for(;;)
  {
    tcp4socket client = sv.accept();
    while((sbytes = client.recv(buffer,sizeof(buffer))) > 0){
      total += sbytes;
      // Got data 
      write(0,buffer,sbytes);
      memset(buffer,0,sbytes);
    }
    // Client closed connection
    // empty buffer
    memset(buffer,0,sizeof(buffer));
    client.close();
  }
  sv.close();
  return 0;
}
