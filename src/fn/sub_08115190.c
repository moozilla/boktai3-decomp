#include "global.h"
struct E { u8 f[0x13]; s8 idx; u8 p[4]; u32 v; };
extern u32 gUnk_020001DC;
void sub_08217EAC(void *);
s32 sub_08115190(u8 *g)
{
    struct E *e = (struct E *)(g + 0x720);
    s32 i = 0x1f;
    do {
        if (e->v != 0) sub_08217EAC(g + 0x20 + e->idx * 40);
        e = (struct E *)((u8 *)e + 0x1c);
        i--;
    } while (i >= 0);
    { u32 z = 0; gUnk_020001DC = z; }
    return 0;
}
