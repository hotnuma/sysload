/*============================================================================
Copyright (c) 2018-2025 Raspberry Pi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
============================================================================*/

#include "gpuload.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static float _gpu_get_usage_new(GpuData *gpudata);
static float _gpu_get_usage(GpuData *gpudata);

float gpuload(GpuData *gpudata, bool output)
{
    float gpuval;

   //if (g_source_is_destroyed (g_main_current_source ())) return FALSE;

    gpuval = _gpu_get_usage_new(gpudata);

    if (gpuval < 0.0)
        gpuval = _gpu_get_usage(gpudata);

    gpuval *= 100;

    if (output)
        printf("%.2f", gpuval);

    return gpuval;
}

static float _gpu_get_usage_new(GpuData *gpudata)
{
    FILE *fp;

    fp = fopen ("/sys/devices/platform/axi/1002000000.v3d/gpu_stats", "rb");

    if (fp == NULL)
        fp = fopen ("/sys/devices/platform/v3dbus/fec00000.v3d/gpu_stats", "rb");

    if (fp == NULL)
        return -1.0;

    char *buf = NULL;
    size_t res = 0;

    char type[16];
    unsigned long jobs;
    unsigned long long timestamp, elapsed = 0, runtime;
    float max, load[5];
    int i;

    // read the stats file a line at a time
    while (getline(&buf, &res, fp) > 0)
    {
        if (sscanf(buf, "%s %lld %ld %lld",
                   type, &timestamp, &jobs, &runtime) == 4)
        {
            // use the timestamp line to calculate time since last measurement
            if (gpudata->last_timestamp < timestamp)
            {
                elapsed = timestamp - gpudata->last_timestamp;
                gpudata->last_timestamp = timestamp;
            }

            // depending on which queue is in the line, calculate the
            // percentage of time used since last measurement
            // store the current time value for the next calculation

            i = -1;

            if (!strncmp(type, "bin", 7))
                i = 0;

            if (!strncmp(type, "render", 7))
                i = 1;

            if (!strncmp(type, "tfu", 7))
                i = 2;

            if (!strncmp(type, "csd", 7))
                i = 3;

            if (!strncmp(type, "cache_clean", 7))
                i = 4;

            if (i == -1)
                continue;

            if (gpudata->last_val[i] == 0)
            {
                load[i] = 0.0;
            }
            else
            {
                if (elapsed)
                {
                    load[i] = runtime;
                    load[i] -= gpudata->last_val[i];
                    load[i] /= elapsed;
                }
            }

            gpudata->last_val[i] = runtime;
        }
    }

    // list is now filled with calculated loadings for each queue for each PID
    free (buf);
    fclose (fp);

    // calculate the max of the five queue values and store in the task array
    max = 0.0;

    for (i = 0; i < 5; i++)
    {
        if (load[i] > max)
            max = load[i];
    }

    return max;
}

static float _gpu_get_usage(GpuData *gpudata)
{
    char *buf = NULL;
    size_t res = 0;
    unsigned long jobs, active;
    unsigned long long timestamp, elapsed = 0, runtime;
    float max, load[5];
    int i;

    // open the stats file
    FILE *fp = fopen("/sys/kernel/debug/dri/0/gpu_usage", "rb");

    if (fp == NULL)
        fp = fopen("/sys/kernel/debug/dri/1/gpu_usage", "rb");

    if (fp == NULL)
        return 0.0;

    // read the stats file a line at a time
    while (getline(&buf, &res, fp) > 0)
    {
        if (sscanf(buf, "timestamp;%lld;", &timestamp) == 1)
        {
            // use the timestamp line to calculate time since last measurement
            elapsed = timestamp - gpudata->last_timestamp;
            gpudata->last_timestamp = timestamp;
        }
        else if (sscanf(strchr(buf, ';'), ";%ld;%lld;%ld;",
                        &jobs, &runtime, &active) == 3)
        {
            // depending on which queue is in the line, calculate the percentage of time used since last measurement
            // store the current time value for the next calculation
            i = -1;

            if (!strncmp(buf, "v3d_bin", 7))
                i = 0;
            if (!strncmp(buf, "v3d_ren", 7))
                i = 1;
            if (!strncmp(buf, "v3d_tfu", 7))
                i = 2;
            if (!strncmp(buf, "v3d_csd", 7))
                i = 3;
            if (!strncmp(buf, "v3d_cac", 7))
                i = 4;

            if (i != -1)
            {
                if (gpudata->last_val[i] == 0) load[i] = 0.0;
                else
                {
                    if (elapsed)
                    {
                        load[i] = runtime;
                        load[i] -= gpudata->last_val[i];
                        load[i] /= elapsed;
                    }
                }

                gpudata->last_val[i] = runtime;
            }
        }
    }

    // list is now filled with calculated loadings for each queue for each PID
    free (buf);
    fclose (fp);

    // calculate the max of the five queue values and store in the task array
    max = 0.0;
    for (i = 0; i < 5; i++)
        if (load[i] > max)
            max = load[i];

    return max;
}


