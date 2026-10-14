#include "global.h"
#define Modifier_EnumerateSelection sub_0803F8F4

s32 Modifier_TestSelection(void *, s32);

static inline void Fill(s32 *p, s32 v, s32 n)
{
    s32 *q = p + n - 1;
    do {
        *q = v;
        q--;
    } while ((s32)q >= (s32)p);
}

s32 Modifier_EnumerateSelection(u8 *p, s32 *out)
{
    s32 *w;
    s32 n, i;
    Fill(out, -1, 16);
    n = 0;
    for (i = 0, w = out; i <= 23; i++) {
        if (Modifier_TestSelection(p, i) && n <= *(s16 *)(p + 0xa)) {
            *w++ = i;
            n++;
        }
    }
    return n;
}
