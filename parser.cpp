#include <cstdint>
#include <cstring>
#include <gsocket/gsocket.hpp>
#include <iostream>
#include "utils/cfunctions/custom_functions.h"
#include "utils/memory.cpp"

/* OPTIONS */
bool opt_tcp = 1; // default protocol
bool opt_udp = 0;
bool opt_verbose = 0;
bool opt_listen = 0;
const char *filepath;
uint16_t opt_timeout = 0;
uint16_t opt_port = 9001;
const char *opt_interface = nullptr;
const char *opt_address = "0.0.0.0";
bool opt_ip6 = false;
const char *opt_whitelist = nullptr;
std::unordered_map<const char *,uint8_t>*opt_whitelist_cache;
const char *opt_blacklist = nullptr;
std::unordered_map<const char *,uint8_t>*opt_blacklist_cache;

#define OPTION_COLOR CYAN
#define ARGUMENT_COLOR GREEN
#define DESCRIPTION_COLOR RESET

void print_help() {
  msg("== GCAT ==\n",stdout,GREEN);
  msg(R"(Usage: ./gcat [options] <host>* <port>*
Note: default addr = 0.0.0.0 port = 9001)",stdout,CYAN);
  msg("\n",stdout);
}

// ./gcat -l <port> ~ listen on 0.0.0.0 <port>
// ./gcat <port> ~ connect to 0.0.0.0 <port>
// ./gcat -lv <port> ~ listen on 0.0.0.0 <port> with verbose
// ./gcat -v <port> ~ connect to 0.0.0.0 <port> with verbose
// ./gcat -l <addr> <port> ~ listen on <addr> <port> 
// ./gcat -l <addr> <port> ~ listen on <addr> <port> 
// ./gcat -lw <file> eth0 4444 ~ listen on interface eth0 port 4444 using whitelist <file> (addr-per-line)

void parse_host_port_opt(const char *address){
  /* 
    * Possible ways to indicate an address 
    * 1 - interface name -> eth0, enp3s0
    * 2 - localhost -> 127.0.0.1
    * 3 - address -> 192.168.1.45, 0.0.0.0, 127.0.0.1 etc.
  */
  for(int i = 0; i < strlen(address); ++i){
    if(*(address+i) == '.'){
      opt_address = address;
      return;
    }
  }
  uint16_t __port = strtol(address,nullptr,10);
  if(__port > 0){
    opt_port = __port;
    return;
  }
  if(!strcmp(address,"localhost")){
    opt_address = "127.0.0.1";
  }
  gsocket::NetworkInterface iface;
  if(gsocket::getIpByIface(&iface,address)){
    msg("getipbyiface()",stdout,RED);
    return;
  };
  address = opt_ip6 ? iface.ip6.c_str() : iface.ip4.c_str();
}

int parser(int argc, const char **args) {
  int pos = 1,err = 0;
  const char *arg = nullptr;
  for(int i = 0; i < argc; ++i){
    if(!strcmp(args[i],"-h") || !strcmp(args[i],"--help")){
      print_help();
      return 0;
    }
  }
  while(pos < argc){
    const char *opt = args[pos];
    if(*opt != '-'){
      parse_host_port_opt(opt);
    }else if(!gstrcmp(opt,"--",2)){
      // parse short option
      int optlen = strlen(opt);
      for(int i = 1; i < optlen; ++i){
        char subopt = *(opt+i);
        switch(subopt){
          case 'l':
            opt_listen = 1;
            break;
          case 'v':
            opt_verbose = 1;
            break;
          default:
            printf("unknown option -%c\n",subopt);
            break;
        }
      }
    }else{
      // parse long option
      if(!strcmp(opt,"--listen")){
        opt_listen = 1;
      }else if(!strcmp(opt,"--verbose")){
        opt_verbose = 1;
      }else if(!strcmp(opt,"--whitelist")){
        opt_whitelist_cache = (std::unordered_map<const char *,uint8_t>*)allocate_cache_list(args[++pos]);
      }else if(!strcmp(opt,"--blacklist")){
        opt_blacklist_cache = (std::unordered_map<const char *,uint8_t>*)allocate_cache_list(args[++pos]);
      }else{
        printf("option '%s' doesn't exist\n",opt);
      }
    }
    ++pos;
  } 
  if(err){
    print_help();
    return 1;
  }
  return 0;
}
