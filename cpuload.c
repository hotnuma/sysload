/*
 * Copyright (c) 2003 Riccardo Persichetti <riccardo.persichetti@tin.it>
 * Copyright (c) 2010 Florian Rivoal <frivoal@xfce.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpuload.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PROC_STAT "/proc/stat"

static unsigned long oldtotal[MAXCPU];
static unsigned long oldused[MAXCPU];

static void _cpu_readline(char *line, int numcpu, bool output);

bool cpuload(int numcpu, bool output)
{
    bool ret = false;

    FILE *fd = fopen(PROC_STAT, "r");
    if (!fd)
        return ret;

    char *line = NULL;
    size_t len;

    // get first line for global cpu usage
    if (getline(&line, &len, fd) <= 0)
        goto out;

    ret = true;

    if (numcpu == 1)
    {
        _cpu_readline(line, 0, output);
    }
    else
    {
        int i = 0;

        // get cpu lines for detailed core usage
        while (getline(&line, &len, fd) != -1)
        {
            if (strncmp(line, "cpu", 3) != 0)
                break;

            _cpu_readline(line, i, output);

            ++i;
        }
    }

out:

    fclose(fd);

    if (line)
        free(line);

    return ret;
}

static void _cpu_readline(char *line, int numcpu, bool output)
{
    assert(line != NULL);
    assert(numcpu < MAXCPU);

    unsigned long long int user = 0;
    unsigned long long int unice = 0;
    unsigned long long int usystem = 0;
    unsigned long long int idle = 0;
    unsigned long long int iowait = 0;
    unsigned long long int irq = 0;
    unsigned long long int softirq = 0;
    unsigned long long int guest = 0;

    // don't count steal time, it is neither busy nor free time

    int nb_read = sscanf(line,
                         "%*s %llu %llu %llu %llu %llu %llu %llu %*u %llu",
                         &user, &unice, &usystem, &idle,
                         &iowait, &irq, &softirq, &guest);

    if (nb_read <= 4)
        iowait = 0;

    if (nb_read <= 5)
        irq = 0;

    if (nb_read <= 6)
        softirq = 0;

    if (nb_read <= 7)
        guest = 0;

    unsigned long used = user + unice + usystem + irq + softirq + guest;
    unsigned long total = used + idle + iowait;
    double div = (total - oldtotal[numcpu]);

    double cpu_used = 0;

    if (div != 0)
    {
        cpu_used = (double) (100.0 * (double) (used - oldused[numcpu]))
                   / div;
    }

    oldused[numcpu] = used;
    oldtotal[numcpu] = total;

    if (output)
    {
        if (numcpu > 0)
            printf("%s", COLSEP);

        printf("%.2f", cpu_used);
    }
}


