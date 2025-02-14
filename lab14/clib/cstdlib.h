#ifndef CSTDLIB_H
#define CSTDLIB_H

#include "../kernel/oslib.h"

// 加载用户程序
void _start_program(dword num);
void memset(byte *buffer, byte value, dword length);
void memcpy(byte *src, byte *dst, dword count);
void memcpy(void *src, void *dst, dword count);
void _start_program(dword num)
{
    /*byte buffer[512];

    switch (num)
    {
    case 4:
        sys_read_hd(1089);

        break;

    default:
        break;
    }
    */
}

void memset(byte *buffer, byte value, dword length)
{
    for (int i = 0; i < length; ++i)
    {
        buffer[i] = value;
    }
}

void memcpy(byte *src, byte *dst, dword count)
{
    for (int i = 0; i < count; ++i)
    {
        dst[i] = src[i];
    }
}

void memcpy(void *src, void *dst, dword count)
{
    byte *srcPtr = (byte *)src;
    byte *dstPtr = (byte *)dst;

    for (int i = 0; i < count; ++i)
    {
        dstPtr[i] = srcPtr[i];
    }
}

#endif