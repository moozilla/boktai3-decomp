#include "global.h"
struct S { u8 f[0x48]; u32 n; u16 ids[8]; u32 d[8][8]; };
extern struct S *gUnk_02000020;
s32 sub_08003AA8(u32 id, s32 cnt, u32 *src)
{
    struct S *s = gUnk_02000020;
    s32 i;
    if (s == 0 || s->n > 7) return -1;
    s->ids[s->n] = id;
    for (i = 0; i < cnt; i++)
        s->d[s->n][i] = *src++;
    s->n++;
    return 0;
}
