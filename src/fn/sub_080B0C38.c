#include "global.h"

s32 Script_GetKeywordValue(s32, s32);

struct S080B0C38 {
    u8 pad0[0x67c];
    u16 a;
    s16 b;
    s16 c;
    u8 pad682[2];
    s32 value;
};

void sub_080B0C38(struct S080B0C38 *p)
{
    p->a = 0xffff;
    p->b = -1;
    p->c = -1;
    p->value = Script_GetKeywordValue(100, 0);
}
