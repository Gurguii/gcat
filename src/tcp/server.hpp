#include <gsocket/gsocket.hpp>
#include "protocoltest/gurguiTCPprotocol.h"
using namespace gsocket;

bool gstrcmp(char *buff1, char* buff2, int n);
int start_server(const char *address = "0.0.0.0", uint16_t port = 9001);
