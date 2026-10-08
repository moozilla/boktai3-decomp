#include "global.h"
struct O { u8 pad0[0x24]; u8 f24[0xA0]; u8 fc4[0x20]; u16 fe4; u16 fe6; u8 pad[0x6F8-0xE8]; s16 f6f8; s16 f6fa; };
void sub_0821980C(void *, void *, u32, u32);
s32 sub_0821319C(struct O *o)
{
    o->fe4 = o->f6f8 * 40;
    o->fe6 = o->f6fa * 40;
    sub_0821980C(o->fc4, o->f24, 0x58, 0);
    return 0;
}
