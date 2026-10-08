#include "global.h"
struct E { u32 a; u16 *b; };
struct P { u8 f[0x4c]; u32 w; u8 g[0x7249 - 0x50]; u8 s; };
s32 sub_0821A3E8(u32, struct E **);
void sub_08195A4C(void *, s32);
void sub_08197324(struct P *p)
{
    struct E *e;
    s32 n = sub_0821A3E8(p->w, &e);
    if (n > 0) {
        do {
            if (*e->b == 0 && p->s == 0) sub_08195A4C(p, 1);
            n--;
            e++;
        } while (n > 0);
    }
}
