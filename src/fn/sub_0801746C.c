#include "global.h"
extern u32 gUnk_03005308;
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
struct S { u8 f[0x1e]; u16 a; u8 g[0xc]; u16 b; u16 c; };
void sub_0801746C(struct S *s)
{
    s->a = s->b;
    if (s->c != 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
        s->a += Mod(*(u16 *)(gUnk_0203B400 + gUnk_03005308 * 2), s->c);
    }
    if (s->a <= 9)
        s->a = 10;
}
