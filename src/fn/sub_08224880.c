#include "global.h"
struct S { u8 pad[4]; u8 f4; u8 f5; u8 pad6[0xa]; u8 f10; u8 pad11[0x13]; u8 f24; u8 pad25[3]; u16 a28[4]; };
extern struct S gUnk_03005390;
extern u16 *gUnk_03006A70[];
void sub_08246AAC(u32, u32);
void sub_08224980(u32, u32);
void sub_08224880(void)
{
    struct S *s = &gUnk_03005390;
    if (s->f4 == 0xf) {
        u16 **t = gUnk_03006A70;
        u32 i = s->f10;
        if (*t[i] == 0x26) {
            u32 z = 0;
            s->f5 = z;
            s->f4 = z;
            sub_08246AAC(4, i);
            s->f24 &= ~(1 << s->f10);
            s->a28[s->f10] = z;
            sub_08224980(0x24, 0);
        }
    }
}
