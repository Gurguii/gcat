#include <gsocket/gsocket.hpp>

using namespace gsocket;

int main()
{
  time_t start = time(NULL);
  tcp4socket sock;
  if(sock.connect("127.0.0.1", 9001)){
    std::cerr << sockError << "\n"; return -1;
  }
  char buff[1024];
  char msg[] = "Hello its gurguisocket\n";
  int sbytes, rbytes;
  for(int i = 0; i < 100000; ++i){
    sbytes = sock.send(msg, sizeof(msg));
    if(sbytes > 0){
      printf("sent %i bytes\n", sbytes);
    }
    rbytes = sock.recv(buff, sizeof(buff));
    if(rbytes > 0){
      printf("read %i bytes: %s\n", rbytes, buff);
      memset(buff,0,rbytes);
    }
  }
  sock.close();
  time_t end = time(NULL);
  double exec_time = (end - start) / (double)CLOCKS_PER_SEC * 1000;
  printf("exec time: %fms\n",exec_time);
  return 0;
}
