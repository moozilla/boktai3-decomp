#include "global.h"
u8 *sub_08037DC0(void);
static inline u8 g(u8 *q) { return q[5]; }
u32 sub_08039744(void)
{
    u8 *p = sub_08037DC0();
    u32 r;
    if (p)
        r = g(p + 0x48);
    else
        r = 0;
    return r;
}
