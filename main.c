//@reboot export DISPLAY=:0 && /home/delo/jobs/dwm_status_bar

#include <stdio.h>
#include "crypto.h"
#include "sysinfo.h"
#include "network.h"
#include <unistd.h>

#define INTERVAL_CRYPTOS 60
#define INTERVAL_SYSINFO 1
#define INTERVAL_NETWORK 5
#define INTERVAL_CLOCK   1

int main(void) 
{
    char buffer_cryptos[64];
    char buffer_sysinfo[64];
    char buffer_network[64];
    char buffer_clock  [20];

    int interval_cryptos  = INTERVAL_CRYPTOS;
    int interval_sysinfo = INTERVAL_SYSINFO;
    int interval_network = INTERVAL_NETWORK;
    int interval_clock   = INTERVAL_CLOCK;

    char command[256];
    sleep(1);

    while(1)
    {
        interval_cryptos += 1;
        interval_sysinfo += 1;
        interval_network += 1;
        interval_clock   += 1;

        if(interval_cryptos >= INTERVAL_CRYPTOS)
        {
            fetch_crypro_price_list(&buffer_cryptos[0], 64);
            interval_cryptos = 0;
        }
        if(interval_sysinfo >= INTERVAL_SYSINFO)
        {
            fetch_system_information(&buffer_sysinfo[0], 64);
            interval_sysinfo = 0;
        }
        if(interval_network >= INTERVAL_NETWORK)
        {
            fetch_ip_address(&buffer_network[0],64);
            interval_network = 0;
        }
        if(interval_clock >= INTERVAL_CLOCK)
        {
            interval_clock = 0;
            time_t t;
            struct tm *tm_info;

            time(&t);
            tm_info = localtime(&t);

            strftime(buffer_clock, sizeof(buffer_clock), "%Y-%m-%d %H:%M", tm_info);

        }

        snprintf(command, sizeof(char)*256, "xsetroot -name ' | %s| %s | %s | %s | '", buffer_cryptos, buffer_network , buffer_sysinfo, buffer_clock);

        system(command);

        system("setxkbmap se");

        sleep(60);
    }

    return 0;
}
