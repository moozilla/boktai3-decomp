#include "global.h"
struct S { u8 f[0x18]; u16 fl; u8 g[0x8c-0x1a]; u32 a; u8 h[0xc]; u16 b; };
void sub_081DAD0C(void *, void (*)(void));
void sub_081D9D14(void);
s32 sub_081D9CDC(u8 *p)
{
    u8 *q;
    sub_081DAD0C(p + 0x18, sub_081D9D14);
    if (!(*(u16 *)(p + 0x18) & 0x2000)) {
        q = p + 0x8c;
        *(u16 *)(q + 0x10) = 0x19;
        *(u32 *)q &= ~0xc;
    }
    return 0;
}
