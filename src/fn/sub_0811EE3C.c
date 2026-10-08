#include "global.h"
struct O { u8 pad[0x54]; u8 a; u8 pad2[2]; u8 c; u16 h; };
s32 sub_0811EE3C(struct O *o)
{
    if (o->a != 0 && o->h == 0 && o->c != 1)
        return 1;
    return 0;
}
