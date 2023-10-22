#include <iostream>
#include <cstdint>
#include <cstring>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

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
  std::cout << MAGENTA << "Usage:" << RESET << " gurguicat [options]\n"
               << YELLOW << "Options:" << RESET << "\n"
               << OPTION_COLOR << "-h" << RESET << " :print this message and exit\n"
               << OPTION_COLOR << "-t" << RESET << " :tcp mode\n"
               << OPTION_COLOR << "-u" << RESET << " :udp mode\n"
               << OPTION_COLOR << "-p "<< ARGUMENT_COLOR << "<port>" << DESCRIPTION_COLOR " :port number\n"
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
  
  while(pos < argc)
  {
    arg = args[pos];
    if(!std::strcmp(arg,"-t"))
    {
      tcp = true;
    }else if(!std::strcmp(arg,"-u"))
    {
      udp = true;
    }else if(!std::strcmp(arg,"-p"))
    {
      if(++pos == argc){fprintf(stderr,"option '%s' requires an argument\n",arg);break;}
      port = std::stoi(args[pos]);
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
  return err;
}
