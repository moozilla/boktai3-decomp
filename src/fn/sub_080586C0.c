#include "global.h"
struct P { u8 f[0x1328]; u16 f1328; u16 f132a; u8 f132c[0x1378 - 0x132c]; u16 f1378; };
extern struct P *gUnk_020004A4;
void sub_0822B2F8(u32);
void sub_080586C0(void)
{
    struct P *p = gUnk_020004A4;
    if (p != 0) {
        u16 *a = &p->f1328;
        u32 z = 0;
        *a = 0x40;
        p->f132a = z;
        p->f1378 = 1;
        sub_0822B2F8(0x129);
    }
}
