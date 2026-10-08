#include "global.h"

struct S { u8 pad[0x18]; u8 pad18[8]; u32 f20; u8 pad2[0xcc-0x24]; u32 fcc; u8 pad3[0xd4-0xd0]; void (*fd4)(void); u32 fd8; };
void sub_08220D78(u8 *, u8 *, u32, u32, u32);
void sub_081C5190(void);

void sub_081C50E0(struct S *p)
{
    void (*f)(void);
    u32 z;
    p->f20 &= ~1;
    p->fcc = (z = 0);
    sub_08220D78((u8 *)p + 0x18, (u8 *)p + 0x78, 0x3a, 1, z);
    f = sub_081C5190;
    p->fd4 = f;
    p->fd8 = z;
}
