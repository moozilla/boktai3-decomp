#include "global.h"
struct O { u8 pad0[0x24]; u8 f24[0xA0]; u8 fc4[0xC0]; u8 f184[0x570]; u16 f6f4; };
void sub_0821980C(void *, void *, u32, u32);
s32 sub_08213170(struct O *o)
{
    sub_0821980C(o->f184, o->f24, (u16)(o->f6f4 + 0x59), 0);
    return 0;
}
