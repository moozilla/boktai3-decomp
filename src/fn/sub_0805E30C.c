#include "global.h"

s32 sub_0805E30C(s32 a)
{
    if (a != 0)
        a += 0x80;
    else
        a = 0x7f;
    return a;
}
