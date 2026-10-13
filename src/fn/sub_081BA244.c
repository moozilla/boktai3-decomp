#include "global.h"

struct Q {
    u8 f[0x52c];
    s32 sel;
    u8 g[4];
    u32 arr[8];
    u8 g2[0xc];
    s32 cnt;
};
void Script_Call(u32, s32);

void sub_081BA244(struct Q *p)
{
    p->cnt++;
    if (p->cnt > 0x3c)
        Script_Call(p->arr[p->sel], 0);
}
