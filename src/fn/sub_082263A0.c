#include "global.h"
struct P { u32 a, b; };
struct S { u8 pad[0x1c]; u16 f1c; u8 pad2[0x68-0x1e]; u16 f68; u16 f6a; u8 pad3[0x74-0x6c]; struct P f74; u16 f7c; };
extern struct S *gUnk_03005420;
s32 sub_08225984(u32);
void sub_082263A0(u32 a, struct P *b)
{
    if (gUnk_03005420) {
        gUnk_03005420->f7c = a;
        if (sub_08225984(gUnk_03005420->f7c)) {
            struct S *p = gUnk_03005420;
            p->f1c = 1;
            p->f74 = *b;
            p->f68 = 3;
        }
    }
}
