#include "global.h"
struct E { u8 f[0x13]; u8 idx; u8 p[8]; u32 v; };
struct E *sub_08114E34(u8 *g)
{
    s32 i = 0;
    u32 one = 1;
    u32 *p = (u32 *)(g + 0x738);
    struct E *e = (struct E *)(g + 0x720);
    struct E *r;
    do {
        if (*p != 1) {
            e->idx = i;
            *p = one;
            r = e;
            goto end;
        }
        p = (u32 *)((u8 *)p + 0x1c);
        e = (struct E *)((u8 *)e + 0x1c);
        i++;
    } while (i <= 0x1f);
    r = 0;
end:
    return r;
}
