#include "global.h"

extern u8 *gUnk_02000580;

void sub_08159010(s32 v)
{
    u8 *g = gUnk_02000580;

    if (g != 0) {
        g[0xAC9] = v;
    }
}
