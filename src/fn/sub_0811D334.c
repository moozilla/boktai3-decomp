#include "global.h"
struct S { u8 f[8]; s8 a[4]; };
s32 sub_0811D334(struct S *s)
{
    s32 n = 0;
    s32 i = 0;
    do {
        if (s->a[i] < 0) break;
        n++;
        i++;
    } while (i <= 3);
    return n;
}
