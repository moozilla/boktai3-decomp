#include "global.h"

struct Q { u8 f[0x52c]; s32 sel; u8 g[4]; u32 arr[8]; u8 g2[0xc]; s32 cnt; };
void sub_0821AD08(u32, s32);

void sub_081BA244(struct Q *p)
{
    p->cnt++;
    if (p->cnt > 0x3c)
        sub_0821AD08(p->arr[p->sel], 0);
}
