#include "global.h"

s32 sub_0822CDA8(s32);

s32 sub_08060958(void)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (sub_0822CDA8(i) < 0) return i;
    }
    return -1;
}
