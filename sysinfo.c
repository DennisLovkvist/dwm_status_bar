#include "sysinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

static char *load_file(char *path)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(fsize + sizeof(char));
    fread(buffer, fsize, 1, f);
    fclose(f);
    buffer[fsize] = '\0';
    return buffer;
}
void fetch_pwr_status(char *buffer, int buffer_size)
{
    char *filename = "/sys/class/power_supply/BAT0/status";

    if(access(filename, F_OK) == -1)
    {
        snprintf(&buffer[0],sizeof(char)*buffer_size, "pwr: 100%%");
        return;
    }

    char* battery_status = load_file(filename);

    if(strncmp(battery_status, "Discharging",sizeof(char)*11) == 0)//11 avoids including LF "Discharging\n\0"
    {
        char* battrey_capacity = load_file("/sys/class/power_supply/BAT0/capacity");
        int length = strlen(battrey_capacity);

        for (int i = 0; i < length; i++)
        {
            if(battrey_capacity[i] == '\n')
            {
                battrey_capacity[i] = '\0';
                break;
            }
        }

        snprintf(&buffer[0],sizeof(char)*buffer_size, "pwr:%s%%", battrey_capacity);
    }
    else
    {
        snprintf(&buffer[0],sizeof(char)*buffer_size, "pwr: 100%%");
    }
}
void fetch_cpu_status(char *buffer, int buffer_size)
{
    int cores = get_nprocs();

    double load_avg[3];
    if (getloadavg(load_avg, 3) == -1)
    {
        snprintf(&buffer[0],sizeof(char)*buffer_size,"cpu:n/a");
    }
    else
    {
        double cpu_usage = (load_avg[0] / cores) * 100.0;
        snprintf(&buffer[0],sizeof(char)*buffer_size,"cpu:%.0lf%%", cpu_usage);
    }
}
void fetch_mem_status(char *buffer, int buffer_size)
{
    struct sysinfo info;
    sysinfo(&info);
    double memory_usage = ((double)(info.totalram - info.freeram) / info.totalram) * 100.0;
    snprintf(&buffer[0],sizeof(char)*12,"mem:%.0lf%%", memory_usage);
}
void fetch_system_information(char *buffer, int buffer_size)
{
    char pwr_status[12];
    fetch_pwr_status(&pwr_status[0],12);

    char cpu_status[12];
    fetch_cpu_status(&cpu_status[0],12);

    char mem_status[12];
    fetch_mem_status(&mem_status[0],12);

    snprintf(&buffer[0],sizeof(char)*buffer_size,"%s %s %s",pwr_status,cpu_status,mem_status);
}