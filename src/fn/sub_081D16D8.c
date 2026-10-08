#include "global.h"

struct S { u8 pad[0x6c]; s32 f6c; };

s32 sub_081D16D8(struct S *p)
{
    s32 r = 0;
    if (p->f6c == 4) r = 1;
    return r;
}
