#include "global.h"
struct P { u32 a, b; };
struct S { u8 pad[0x1c]; u16 f1c; u8 pad2[0x68-0x1e]; u16 f68; u16 f6a; u8 pad3[0x74-0x6c]; struct P f74; u16 f7c; };
extern struct S *gUnk_03005420;
s32 sub_08225984(u32);
void sub_08226AAC(struct S *);
void sub_082263FC(u32 a, s32 n, struct P *b)
{
    if (gUnk_03005420) {
        gUnk_03005420->f7c = a;
        if (sub_08225984(gUnk_03005420->f7c)) {
            struct S *p = gUnk_03005420;
            p->f1c = 1;
            p->f74 = *b;
            if (n <= 1) {
                p->f68 = 3;
                sub_08226AAC(p);
            } else {
                p->f6a = n;
                p->f68 = 4;
            }
        }
    }
}
