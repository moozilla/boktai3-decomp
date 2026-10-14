#include "global.h"

s32 sub_0803F82C(void *, s32);

static inline void Fill(s32 *p, s32 v, s32 n)
{
    s32 *q = p + n - 1;
    do {
        *q = v;
        q--;
    } while ((s32)q >= (s32)p);
}

s32 sub_0803F8F4(u8 *p, s32 *out)
{
    s32 *w;
    s32 n, i;
    Fill(out, -1, 16);
    n = 0;
    for (i = 0, w = out; i <= 23; i++) {
        if (sub_0803F82C(p, i) && n <= *(s16 *)(p + 0xa)) {
            *w++ = i;
            n++;
        }
    }
    return n;
}
