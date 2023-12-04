#pragma once

#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void fetch_ip_address(char *buffer, int buffer_size);