#include "cpuload.h"
#include "gpuload.h"
#include "vcgencmd.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

int msleep(long msec)
{
    // https://stackoverflow.com/questions/1157209/

    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    }
    while (res && errno == EINTR);

    return res;
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    setbuf(stdout, NULL);

    long cpucount = 1;
    cpucount = sysconf(_SC_NPROCESSORS_ONLN);

    bool opt_withgpu = true;
    bool opt_withtemp = read_temp();

    GpuData gpudata = {0};

    cpuload(cpucount, false);
    gpuload(&gpudata, false);

    while (1)
    {
        msleep(1000);

        cpuload(cpucount, true);

        if (opt_withgpu)
        {
            printf("%s", COLSEP);
            gpuload(&gpudata, true);
        }

        if (opt_withtemp)
        {
            printf("%s", COLSEP);
            read_temp();
        }

        printf("\n");
    }

    return EXIT_SUCCESS;
}


