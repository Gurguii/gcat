#include "utils/cfunctions/custom_functions.c"
#include "parser.cpp"
#include "src/tcp/server.cpp"
#include "src/tcp/client.cpp"

/*
bool tcp = 0;
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
  if (argc == 1)
  {
    print_help();
    return 0;
  }

  if(parser(argc-1, args+1)){
    print_help();
    return 0;
  };
  
  if(tcp && listen_mode){
    if(interface != nullptr){
      gsocket::NetworkInterface ifa;
      gsocket::getIpByIface(&ifa, interface);
      gcat::start_tcp_server(ip6 ? ifa.ip6.c_str() : ifa.ip4.c_str(),port);
    }
    gcat::start_tcp_server(address,port);
  }else if(tcp && !listen_mode){
    gcat::start_tcp_client(address,port);
  }
  printf("== Arguments parsed ==\ntcp: %b\nudp: %b\nverbose: %b\nlisten: %b\nfilepath: %s\ntimeout: %d\nport: %d\n", tcp, udp, verbose, listen_mode, filepath, timeout, port);
  return 0;
}
