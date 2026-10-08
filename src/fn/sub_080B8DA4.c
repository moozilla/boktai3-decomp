#include "global.h"

struct P { u8 f[0x2d4]; u16 fl; u8 g[0x6dc - 0x2d6]; u32 v; };
void sub_080B6210(struct P *);
void sub_080B5F18(struct P *);

s32 sub_080B8DA4(struct P *p)
{
    u32 m;
    u16 *q;
    sub_080B6210(p);
    sub_080B5F18(p);
    m = 0x200;
    q = &p->fl;
    if ((*q & m) == 0) {
        u32 *vp = &p->v;
        u32 k = 0xFFFFDFFF;
        *vp = *vp & k;
    }
    return 1;
}
