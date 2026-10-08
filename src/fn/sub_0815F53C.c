#include "global.h"

struct O { u8 f00[0x30]; u8 a[0xDC]; u8 b[1]; u8 f10d[0x457 - 0x10D]; u8 c57; };
extern struct O *gUnk_02000580[];

u8 *sub_0815F53C(u32 i)
{
    struct O *o = gUnk_02000580[i];
    if (o == 0)
        return 0;
    if (o->c57 == 0x26 || o->c57 == 0xc)
        return (u8 *)o + 0x10C;
    return (u8 *)o + 0x30;
}
