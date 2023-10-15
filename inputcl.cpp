#include <cstdio>
#include <gsocket/gsocket.hpp>

using namespace gsocket;

int main()
{
  tcp4socket cl;
  if(cl.connect("127.0.0.1", 9001)){
    fprintf(stderr, "couldn't connect ...\n"); return -1;
  }
  char buff[4096];
  for(;;)
  {
    printf("msg: ");
    fgets(buff, sizeof(buff), stdin);
    if(!strcmp(buff,"exit")){
      break;
    }
    cl.send(buff,sizeof(buff));
  }
  cl.close();
  return 0;
}
