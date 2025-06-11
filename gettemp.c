#include "gettemp.h"
#include <stdlib.h>
#include <stdio.h>

#define CPU_TEMP "/sys/devices/virtual/thermal/thermal_zone0/temp"
char _tempbuff[6];

bool gettemp(bool output)
{
    FILE *fd = fopen(CPU_TEMP, "r");
    if (!fd)
        return false;

    float val = 0;

    if (fread(_tempbuff, sizeof(char), 5, fd) == 5)
    {
        _tempbuff[5] = '\0';
        val = (float) atoi(_tempbuff) / (float) 1000.0;
    }

    if (output)
        printf("%.2f", val);

    fclose(fd);

    return true;
}


