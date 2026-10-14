#include "global.h"
void sub_0822D740(u8 *src, u8 *dst)
{
    u32 *from;
    u32 *to;
    s32 n;
    dst[0] = src[0];
    {
        u16 life = 1000;
        *(u16 *)(dst + 2) = life;
    }
    from = (u32 *)src;
    to = (u32 *)dst;
    for (n = 2; n >= 0; n--) {
        to[1] = from[3];
        from++;
        to++;
    }
}
