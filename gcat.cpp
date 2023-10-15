#include <gsocket/gsocket.hpp>
using namespace gsocket;
int main()
{
  tcp4socket sv;
  if(sv.bind("127.0.0.1", 9001)){
    fprintf(stderr, "%s\n", sockError); return -1;
  };
  addr myadd;
  sv.get_address(myadd);
  printf("Listening on %s:%d\n",std::get<addr4>(myadd).host().c_str(),std::get<addr4>(myadd).port);
  char buff[4096];
  int rbytes;
  printf("[+] waiting client...\n");
  tcp4socket client = sv.accept();
  printf("[+] got new client\n");
  for(rbytes = client.recv(buff, sizeof(buff)); rbytes > 0; rbytes = client.recv(buff,sizeof(buff)))
  {
    printf("%s",buff);
  }
}

