#include "global.h"
struct S {
    u8 pad[8];
    u8 values[4];
    u8 count;
};

void sub_0811D47C(struct S *s, s32 count, u32 *src)
{
    s32 i;
    if (count > 4)
        count = 4;
    for (i = 0; i < count; i++)
        s->values[i] = src[i];
    s->count = count;
}
