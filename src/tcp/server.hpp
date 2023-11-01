#pragma once
#include <gsocket/gsocket.hpp>
#include "../../utils/cfunctions/custom_functions.h"
#include "protocol.hpp"
namespace gcat
{
int start_tcp_server(const char *address = "0.0.0.0", uint16_t port = 9001);
int start_tcp_server_no_protocol(const char *address, uint16_t port);
}
