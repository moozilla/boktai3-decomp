#include "global.h"
void sub_080515DC(u8 *);
void sub_0821AD08(u32, u32);
void sub_0821A0C0(u8 *);
u32 sub_080516FC(u8 *p)
{
    if (p[0xC8] != 0) {
        u32 *q;
        u32 v;
        *(u32 *)(p + 0x18) |= 1;
        sub_080515DC(p);
        q = (u32 *)(p + 0xCC);
        v = *q;
        if (v != 0) {
            *q = 0;
            sub_0821AD08(v, 0);
        }
        sub_0821A0C0(p);
    }
    return 0;
}
