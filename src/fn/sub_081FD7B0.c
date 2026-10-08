#include "global.h"
struct S { u8 p0[0x14]; u8 pad[0x3a4 - 0x14]; s32 w3a4; };
void sub_0822B2F8(u32);
void sub_081FE928(void *, void (*)(void));
void sub_081FD7EC(void);
u32 sub_081FD7B0(struct S *p)
{
    p->w3a4 += 0x4000;
    if (p->w3a4 > 0x40000) {
        sub_0822B2F8(0x564);
        sub_081FE928((u8 *)p + 0x14, sub_081FD7EC);
    }
    return 0;
}
