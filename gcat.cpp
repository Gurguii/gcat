#include <gsocket/gsocket.hpp>
#include "./parser.cpp"
using namespace gsocket;

int main(int argc, const char **args)
{
  if (argc == 1)
  {
    print_help();
    return 0;
  }

  if(parser(argc, args)){
    print_help();
    return 0;
  };

  printf("== Arguments parsed ==\ntcp: %b\nudp: %b\nverbose: %b\nlisten: %b\nfilepath: %s\ntimeout: %d\nport: %d\n", tcp, udp, verbose, listen_mode, filepath.c_str(), timeout, port);
  return 0;
}

