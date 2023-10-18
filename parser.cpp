#include <iostream>
#include <cstdint>
#include <cstring>
/* Gurguicat arguments
*  -t : tcp mode
*  -u : udp mode
*  -p <port>: port number
*  -v : verbose
*  -l : listen mode
*  -f <file> : save data received to <file> 
*  -tout <int> : timeout to receive data after connection
*/

bool tcp = 0;
bool udp = 0;
bool verbose = 0;
bool listen_mode = 0;
std::string filepath;
uint16_t timeout = 0;
uint16_t port = 80; // Default port number

void print_help()
{
  std::cout << "Usage: gurguicat [options]\n"
               << "Options:\n"
               << "  -t : tcp mode\n"
               << "  -u : udp mode\n"
               << "  -p <port>: port number\n"
               << "  -v : verbose\n"
               << "  -l : listen mode\n"
               << "  -f <file> : save data received to <file>\n"
               << "  -tout <int> : timeout to receive data after connection\n";
}
// TODO - add long option (-p | --port)
int parser(int argc, const char **args)
{
  int pos = 0, err = 0;
  const char *arg = nullptr;
  for(int i = 1; i < argc; ++i)
  {
    arg = args[i];
    if(!std::strcmp(arg,"-t"))
    {
      tcp = true;
    }else if(!std::strcmp(arg,"-u"))
    {
      udp = true;
    }else if(!std::strcmp(arg,"-p"))
    {
      port = std::stoi(args[++i]);
    }else if(!std::strcmp(arg,"-v"))
    {
      verbose = true;
    }else if(!std::strcmp(arg,"-l"))
    {
      listen_mode = true;
    }else if(!std::strcmp(arg,"-f"))
    {
      filepath = args[++i]; 
    }else if(!std::strcmp(arg,"-tout"))
    {
      timeout = std::stoi(args[++i]);
    }else if(!std::strcmp(arg,"-h"))
    {
      return 1;
    }else 
    {
      fprintf(stderr,"[err] option '%s' doesn't exist\n",arg);
    }
  }
  return err;
}
