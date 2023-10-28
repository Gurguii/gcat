#include "src/tcp/server.hpp"
#include "utils/cfunctions/custom_functions.c"
#include "parser.cpp"
#include "src/tcp/server.cpp"
#include "src/tcp/client.cpp"
#include "src/tcp/client_noprotocol.cpp"
#include "src/tcp/server_noprotocol.cpp"
#include "src/tcp/fullduplex/fullduplex_client.cpp"
#include "src/tcp/fullduplex/fullduplex_server_fork.cpp"
#include "src/tcp/fullduplex/fullduplex_server_async.cpp"
/*
bool tcp = 1;
bool udp = 0;
bool verbose = 0;
bool listen_mode = 0;
std::string filepath;
uint16_t timeout = 0;
uint16_t port = 4444; // Default port number
std::string interface = nullptr 
bool ip6 = false;
*/

enum class MODE : uint8_t
{
  CLIENT = 0,
  SERVER = 1
};

uint8_t operator==(uint8_t n,MODE n2)
{
  return static_cast<uint8_t>(n2) == n;
}

int main(int argc, const char **args)
{
  if(parser(argc-1,args+1)){print_help(); return 0;}
  printf("address: %s - port: %d\n", address, port);
  if(tcp && listen_mode){
    gcat::start_fullduplex_tcp_server<gsocket::tcp4socket>(gsocket::tcp4socket(),address,port);
  }else if(tcp && !listen_mode){
    gcat::start_fullduplex_tcp_client<gsocket::tcp4socket>(gsocket::tcp4socket(),address,port);
    //gcat::start_tcp_client_no_protocol(address,port);
  }
  
  if(udp && listen_mode){
  }else if(udp && !listen_mode){
  }
  //printf("== Arguments parsed ==\ntcp: %b\nudp: %b\nverbose: %b\nlisten: %b\nfilepath: %s\ntimeout: %d\nport: %d\n", tcp, udp, verbose, listen_mode, filepath, timeout, port);
  return 0;
}
