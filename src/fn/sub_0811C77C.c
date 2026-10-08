#include "global.h"

struct S { u8 filler[0xd2]; u16 h; u8 filler2[0x252 - 0xd4]; s8 b; };
s32 sub_0811C67C(s32);

s32 sub_0811C77C(struct S *s)
{
    s32 r;
    switch (s->h) {
    case 0:
        r = sub_0811C67C(s->b);
        break;
    case 1:
        r = 0x106;
        break;
    case 2:
        r = 0x107;
        break;
    case 3:
        r = 0x108;
        break;
    case 4:
    case 5:
        r = 0x109;
        break;
    default:
        r = 0x378;
        break;
    }
    return r;
}
