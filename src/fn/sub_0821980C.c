#include "global.h"

struct Obj { u8 pad[2]; u16 f2; u8 pad4[0x17]; u8 f1B; };
extern u16 gUnk_03005238;
s32 sub_08219728(struct Obj *, u32, u32);

s32 sub_0821980C(struct Obj *o, u32 b, u16 c, u8 d)
{
    if (sub_08219728(o, b, c) < 0)
        return -1;
    o->f2 = gUnk_03005238;
    o->f1B = d;
    return 0;
}
