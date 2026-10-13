#include "global.h"
extern u8 *gUnk_020000B0;
void sub_08214374(u8 *, int);
u32 SpritePool_Activate(u8 *p, u32 callback, u32 context)
{
    u8 *manager = gUnk_020000B0;
    u32 id;
    if (!manager || p[4])
        return 0;
    if (*(u32 *)(p + 12) & 0x80)
        sub_08214374(p + 12, 1);
    else
        sub_08214374(p + 12, 0);
    *(u32 *)(p + 0x68) = callback;
    *(u32 *)(p + 0xac) = context;
    *(u32 *)(p + 8) = 0;
    p[4] = 1;
    id = ++*(u32 *)(manager + 0xb18);
    id = (id << 28) | (u32)p;
    *(u32 *)p = id;
    return id;
}
