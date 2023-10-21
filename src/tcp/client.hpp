#include <gsocket/gsocket.hpp>
#include <string.h>
#include "protocoltest/gurguiTCPprotocol.h"
using namespace gsocket;

bool gstrcmp(char *buff1, char* buff2, int n);
int simple_client(const char *address, uint16_t port);

