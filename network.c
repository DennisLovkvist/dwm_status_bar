#include <stdio.h>
#include "network.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/wireless.h>


void fetch_ip_address(char *buffer, int buffer_size)
{
    struct ifaddrs *interfaces, *interface_current;

    if (getifaddrs(&interfaces) == -1)
    {
        snprintf(buffer, buffer_size, "172.0.0.1");
        return;
    }

    for (interface_current = interfaces; interface_current != NULL; interface_current = interface_current->ifa_next) 
    {
        if (interface_current->ifa_addr != NULL && interface_current->ifa_addr->sa_family == AF_INET) 
        {
            struct sockaddr_in *socket_address = (struct sockaddr_in *)interface_current->ifa_addr;
            char *ip_string = inet_ntoa(socket_address->sin_addr);

            int loopback = strncmp(ip_string,"127.0.0.1", sizeof(char)*10) == 0;

            if(!loopback)
            {
                int count = 0;
                int index = 0;
                while(ip_string[index] != '\0')
                {
                    count += (ip_string[index] == '.') ? 1 : 0;
                    index++;
                }
                if(count == 3)
                {
                    snprintf(buffer, buffer_size, "%s", ip_string);
                    freeifaddrs(interfaces);
                    return;
                }
            }
        }
    }

    snprintf(buffer, buffer_size, "127.0.0.1");
    freeifaddrs(interfaces);
    return;
}