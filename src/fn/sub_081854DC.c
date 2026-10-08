#include "global.h"
struct S { u8 f0[0x48]; s16 a48; s16 a4a; s16 a4c; u8 g0[0xB39-0x4E]; u8 b39; u8 b3a; u8 f1; u8 b3c; u8 b3d; u8 b3e; u8 b3f; u8 b40; u8 b41; u8 b42; u8 b43; u8 f2; u8 b45; u8 g1[0x1124-0xB46]; s16 x; s16 y; s16 z;};
struct G { u8 f[0x30]; s16 x; s16 pad; s16 y; };
extern struct G *gUnk_02000710;
s32 sub_082215E4(s32, s32);
void sub_081850F4(struct S *, s32);
s32 sub_0818505C(struct S *, s32);
void sub_081854DC(struct S *p)
{
    u8 *q = &p->b3d;
    u8 r;
    switch (*q) {
    case 0: {
        struct G *g = gUnk_02000710;
        s32 a = sub_082215E4(g->x - p->a48, g->y - p->a4c);
        a = (a + 0x20) & 0xc0;
        if (p->b39 != a) sub_081850F4(p, a);
        else if (sub_0818505C(p, 6)) { r = 1; goto st; }
        break;
    }
    case 1:
        r = p->b40;
        if (r == 0xff) {
st:
            *q = r;
        }
        break;
    }
}
