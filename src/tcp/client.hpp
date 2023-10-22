#pragma once
#include <gsocket/gsocket.hpp>
#include "gurguiTCPprotocol.h"
#include "../../utils/cfunctions/custom_functions.h"

namespace gcat
{
int start_tcp_client(const char *address, uint16_t port);
int start_tcp_client_no_protocol(const char *address, uint16_t port);
}
