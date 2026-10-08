#include "global.h"
struct O { u8 f[0x18]; u32 *q; u8 g[0xa]; u8 n; u8 h; u8 s; };
static inline u32 tst(u32 *a, u32 m) { return *a & m; }
void sub_0804E618(struct O *, u32);
void sub_0804E87C(struct O *o)
{
    if (o->s == 1) {
        s32 i;
        o->s = 0;
        i = 0;
        if (i < o->n) {
            u32 m = 1;
            u8 *fp = (u8 *)o + 0xc9;
            u8 *e = (u8 *)o;
            u8 *np = &o->n;
            do {
                if (*fp != 0) *(u32 *)(e + 0x34) |= m;
                fp += 0xa8;
                e += 0xa8;
                i++;
            } while (i < *np);
        }
    }
    sub_0804E618(o, tst(&o->q[8], 0x20) ? 2 : 0);
}
