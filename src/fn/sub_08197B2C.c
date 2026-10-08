#include "global.h"
struct P { u8 f[0x3a]; u16 h; u8 g[0xc]; u32 w; };
extern u8 *gUnk_030042E4;
struct P *sub_0819792C(s32, void *, s32, s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_08197A50(void);
void sub_08197A0C(void);
void sub_08197B2C(struct P *p, s32 b, s32 c)
{
    struct P *r = sub_0819792C(0, p, b, 0, c);
    if (r != 0) {
        u8 *q;
        sub_0821A04C(r, sub_08197A50, sub_08197A0C);
        {
            u16 k = 0x192;
            q = gUnk_030042E4;
            q += 0x3240;
            q -= c << 5;
            p->h = k;
            p->w = (u32)q;
        }
    }
}
