#include "global.h"
void sub_08219DD8(void *data, s32 bytes)
{
    u8 *ptr = data;
    s32 aligned;
    u32 zero;
    while (bytes > 0 && ((u32)ptr & 3)) { *ptr++ = 0; bytes--; }
    zero = 0;
    aligned = bytes & ~3;
    CpuSet(&zero, ptr, ((aligned / 4) & 0x1FFFFF) | 0x05000000);
    ptr += aligned;
    bytes &= 3;
    while (bytes > 0) { *ptr = 0; bytes--; ptr++; }
}
