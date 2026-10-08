#include "global.h"
struct S { u8 f0; u8 f1; u8 pad[2]; u8 f4; u8 f5; u8 f6; u8 f7; u8 f8; u8 pad2[4]; u8 fd; };
extern struct S gUnk_03005390;
u32 sub_08245890(void);
void sub_08224C84(void);
u32 sub_08223534(void)
{
    u32 r = sub_08245890();
    struct S *s;
    u32 c;
    if (r == 0x8001)
        gUnk_03005390.f8 = 1;
    c = gUnk_03005390.f4;
    s = &gUnk_03005390;
    if (c != 0x17 && c != 1) {
        s->f5 = 0;
        s->f4 = 0;
    }
    s->f7 = 0;
    s->fd = 0;
    s->f1 = 0;
    s->f0 = 0;
    s->f6 = 0xff;
    sub_08224C84();
    return r;
}
