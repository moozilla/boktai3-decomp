#include "global.h"

struct S { u8 f[0x18]; u8 a[0x18]; u8 b[1]; };
extern struct S *gUnk_02000020;
extern u32 gUnk_03004DA0;
extern u16 gUnk_030051C4;
extern u16 gUnk_03005214;
extern u32 gUnk_030051D4;
extern u16 gUnk_030051DC;
extern u32 gUnk_030051C8;
void sub_0800377C(void *);

void sub_080037B0(u32 a, u32 b, u32 c)
{
    struct S *s = gUnk_02000020;
    if (s != 0) {
        gUnk_03004DA0 = a;
        gUnk_030051C4 = b;
        gUnk_03005214 = c;
        gUnk_030051D4 = a;
        gUnk_030051DC = c;
        gUnk_030051C8 = 0;
        sub_0800377C(s->a);
        sub_0800377C(s->b);
    }
}
