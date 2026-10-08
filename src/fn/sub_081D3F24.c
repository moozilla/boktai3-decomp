#include "global.h"

struct S { u8 pad[0x1c]; s32 f1c; };

s32 sub_081D3F24(struct S *p)
{
    s32 r;
    switch (p->f1c) {
    case 0x3e:
    case 0x64:
        r = 0x65;
        break;
    case 0x65:
        r = 0x64;
        break;
    case 0x3f:
    case 0x66:
        r = 0x67;
        break;
    case 0x67:
        r = 0x66;
        break;
    default:
        r = 0;
        break;
    }
    return r;
}
