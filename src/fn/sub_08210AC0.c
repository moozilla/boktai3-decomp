#include "global.h"
s32 sub_08210A8C(void);
u32 sub_08210AC0(void)
{
    s32 v = sub_08210A8C();
    u32 r = 0;
    if (v > 26)
        r = 1;
    return r;
}
