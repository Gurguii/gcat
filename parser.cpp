#include <gsocket/gsocket.hpp>
#include <iostream>
#include <cstdint>
#include <cstring>
#include "messages.cpp"

bool tcp = 1; // default protocol
bool udp = 0;
bool verbose = 0;
bool listen_mode = 0;
const char *filepath;
uint16_t timeout = 0;
uint16_t port = 4444;
const char *interface = nullptr; 
const char *address = nullptr;
bool ip6 = false;

#define OPTION_COLOR CYAN
#define ARGUMENT_COLOR GREEN
#define DESCRIPTION_COLOR RESET

void print_help()
{
  std::cout << MAGENTA << "Usage:" << RESET << " gurguicat [options] <host> <port>\n"
               << YELLOW << "Options:" << RESET << "\n"
               << OPTION_COLOR << "-h" << RESET << " :print this message and exit\n"
               << OPTION_COLOR << "-t" << RESET << " :tcp mode\n"
               << OPTION_COLOR << "-u" << RESET << " :udp mode\n"
               << OPTION_COLOR << "-v" << RESET << " :verbose" << DESCRIPTION_COLOR << "\n"
               << OPTION_COLOR << "-l" << RESET << " :listen mode" << DESCRIPTION_COLOR << "\n"
               << OPTION_COLOR << "-f" << ARGUMENT_COLOR << " <file> " << RESET << ":save data received to a file\n"
               << OPTION_COLOR << "-tout" << ARGUMENT_COLOR << " <time(s)> " << RESET << ":timeout to receive data after connection" << DESCRIPTION_COLOR << "\n"
               << OPTION_COLOR << "-i" << ARGUMENT_COLOR << " <iface> " << RESET << ":interface to bind to (server)" << DESCRIPTION_COLOR << "\n"
               << OPTION_COLOR << "-ip6" << RESET << " :use IPv6\n";
}

// TODO - add long option (-p | --port)
int parser(int argc, const char **args)
{
  int pos = 0, err = 0;
  const char *arg = nullptr;
  for(int i = 0; i < argc; ++i){
    if(!strcmp(args[i],"-h") || !strcmp(args[i],"--help")){
      return 1;
    }
  }
  if(argc == 0){
    return 1;
  }
  // LISTEN ON ALL ADDRS - GIVEN PORT
  if(argc == 1){
    address = "0.0.0.0";
    port = strtol(args[0],nullptr,10);
    listen_mode = true;
    return port <= 0 ? 1 : 0;
  }
  // CONNECT TO GIVEN ADDR - GIVEN PORT
  if(argc == 2){
    address = args[0];
    port = strtol(args[1],nullptr,10);
    return port <= 0 ? 1 : 0;
  }

  // gcat [options] <host> <port>  
  address = args[argc-2];
  port = strtol(args[argc-1],nullptr,10);
  argc -= 2; 
  while(pos < argc)
  {
    arg = args[pos];
    msg(GREEN, arg, stdout);
    if(!std::strcmp(arg,"-u"))
    {
      udp = true;
      tcp = false;
    }else if(!std::strcmp(arg,"-v"))
    {
      verbose = true;
    }else if(!std::strcmp(arg,"-l"))
    {
      listen_mode = true;
    }else if(!std::strcmp(arg,"-f"))
    {
      if(++pos == argc){fprintf(stderr,"option '%s' requires an argument\n",arg);break;}
      filepath = args[pos]; 
    }else if(!std::strcmp(arg,"-tout"))
    {
      timeout = std::stoi(args[++pos]);
    }else if(!std::strcmp(arg,"-h"))
    {
      err = 1;
    }else if(!std::strcmp(arg,"-i")){
      if(++pos == argc){fprintf(stderr,"option '%s' requires an argument\n",arg);break;}
      interface = args[pos];
    }else if(!std::strcmp(arg,"-ip6")){
      ip6 = true;
    }
    else 
    {
      fprintf(stderr,"[err] option '%s' doesn't exist\n",arg);
    }
    ++pos;
  }

  // check if given address is an interface or an actual address
  int size = strlen(address);
  for(int i = 0; i < size; ++i){
    if(*(address+i) < 48 || *(address+i) > 57){
      // not a number, interpret as interface
      gsocket::NetworkInterface ifa;
      if(gsocket::getIpByIface(&ifa,address)){
        // not a valid interface
        fprintf(stderr,"[error] invalid interface '%s'\n", address);
        return -1;
      };
      printf("ipv4: %s\nipv6: %s\n", ifa.ip4.c_str(), ifa.ip6.c_str());
      address = ip6 ? std::move(ifa.ip6.c_str()) : std::move(ifa.ip4.c_str());
      break;
    }
  }
  return err;
}
