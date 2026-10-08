#include "global.h"

s32 sub_08108B70(u8 *p)
{
    s32 lim = *(s32 *)(p + 0x18);
    s32 cnt;

    if (lim < 0) {
        return 0;
    }
    cnt = *(s32 *)(p + 0x34) + 1;
    *(s32 *)(p + 0x34) = cnt;
    if (cnt >= lim) {
        return 1;
    }
    return 0;
}
