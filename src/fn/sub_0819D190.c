#include "global.h"
#include "input.h"

void sub_0819D190(u8 *p)
{
    if (gPadState[1] & 0x100) {
        s32 v = (p[0xab] + 1) % 39;
        if (v == 0)
            v = 1;
        p[0xaa] = v;
    }
    (*(u32 *)(p + 0xc4))++;
}
