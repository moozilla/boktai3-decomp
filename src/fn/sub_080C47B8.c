#include "global.h"

void sub_080C47B8(void *ptr)
{
    u16 *p = ptr;
    long long mask = 0xFFFFFF00;

    *p = (*p & mask) + 0x80;
    p[1] = (mask & p[1]) + 0x80;
    p[2] = (mask & p[2]) + 0x80;
}
