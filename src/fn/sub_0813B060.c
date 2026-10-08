#include "global.h"
void sub_0824923C(void *, u32);
void sub_0813B060(u8 *p)
{
    u32 *q = (u32 *)(p + 0xca8);
    if (*q) {
        sub_0824923C(p, *q);
        *q = 0;
    }
}
