#include "global.h"

struct S { u8 pad[0x18]; u32 f18; u8 pad2[0x10]; u32 f2c; };
void sub_0821AD08(u32, u32);
void sub_0821A0C0(u32);

void sub_081C4C18(struct S *p)
{
    if (p->f2c) sub_0821AD08(p->f2c, 0);
    sub_0821A0C0(p->f18);
    sub_0821A0C0((u32)p);
}
