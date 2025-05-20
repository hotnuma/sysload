/*
Copyright (c) 2012, Broadcom Europe Ltd
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
*/

#include "vcgencmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h> // ioctl

#define DEVICE_FILE_NAME "/dev/vcio"
#define MAJOR_NUM 100
#define IOCTL_MBOX_PROPERTY _IOWR(MAJOR_NUM, 0, char *)

#define MAX_STRING 1024
char _result[MAX_STRING] = {};

static int _mbox_open();
static void _mbox_close(int file_desc);
static unsigned _gencmd(int file_desc,
                        const char *command, char *result, int result_len);
static int _mbox_property(int file_desc, void *buf);

bool read_temp(bool output)
{
    int mb = _mbox_open();
    if (mb < 0)
        return false;

    int ret = _gencmd(mb, "measure_temp", _result, sizeof(_result));
    if (ret)
        return false;

    _mbox_close(mb);
    
    if (output)
    {
        //printf("%s\t", _result);
        
        float val;
        sscanf(_result, "temp=%f'C", &val);

        printf("%.2f", val);
    }

    return true;
}

static int _mbox_open()
{
    // open a char device file used for communicating with kernel mbox driver

    int file_desc = open(DEVICE_FILE_NAME, 0);

    if (file_desc < 0)
        return -1;

    return file_desc;
}

static void _mbox_close(int file_desc)
{
    close(file_desc);
}

#define GET_GENCMD_RESULT 0x00030080

static unsigned _gencmd(int file_desc,
                        const char *command, char *result, int result_len)
{
    int i = 0;
    unsigned p[(MAX_STRING >> 2) + 7];
    int len = strlen(command);
    // maximum length for command or response
    if (len + 1 >= MAX_STRING)
    {
        fprintf(stderr, "gencmd length too long : %d\n", len);
        return -1;
    }
    p[i++] = 0;          // size
    p[i++] = 0x00000000; // process request

    p[i++] = GET_GENCMD_RESULT; // (the tag id)
    p[i++] = MAX_STRING;        // buffer_len
    p[i++] = 0;                 // request_len (set to response length)
    p[i++] = 0;                 // error repsonse

    memcpy(p + i, command, len + 1);
    i += MAX_STRING >> 2;

    p[i++] = 0x00000000;  // end tag
    p[0] = i * sizeof *p; // actual size

    _mbox_property(file_desc, p);
    result[0] = 0;
    strncat(result, (const char*)(p + 6), result_len);

    return p[5];
}

static int _mbox_property(int file_desc, void *buf)
{
    // use ioctl to send mbox property message

    int ret_val = ioctl(file_desc, IOCTL_MBOX_PROPERTY, buf);

    if (ret_val < 0)
    {
        printf("ioctl_set_msg failed:%d\n", ret_val);
    }

    return ret_val;
}


