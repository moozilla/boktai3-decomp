#include "global.h"
struct O { u8 pad0[0x44]; u8 f44[0x80]; u8 fc4[0x20]; u16 fe4; u16 fe6; u8 pad[0xB64-0xE8]; s16 b64; s16 b66; };
void sub_0821980C(void *, void *, u32, u32);
s32 sub_08211D94(struct O *o)
{
    o->fe4 = o->b64 * 24 + 0x58;
    o->fe6 = o->b66 * 24 + 0x18;
    sub_0821980C(o->fc4, o->f44, 0x4d, 0);
    return 0;
}
