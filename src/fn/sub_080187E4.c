#include "global.h"
extern u32 gUnk_03005308;
extern u8 gUnk_0203B400[];
struct B { u8 a; u8 r; u16 c; u16 d; u16 t; u32 fl; };
struct O { u8 f[0x18]; s32 lo; s32 hi; };
void sub_080187E4(struct O *o, struct B *b)
{
    if (b->t == 0) {
        if (o->hi < 0 || o->lo < o->hi) {
            b->a = 1;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            b->r = *(u16 *)(gUnk_0203B400 + gUnk_03005308 * 2);
            b->c = 0;
            b->fl &= ~1;
        }
    } else {
        b->t--;
    }
}
