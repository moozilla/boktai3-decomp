#include "global.h"
struct S { u8 pad[0x1c]; u16 f1c; u16 f1e; u8 pad2[0x68-0x20]; u16 f68; u16 f6a; };
extern struct S *gUnk_03005420;
void sub_08226AAC(struct S *);
void sub_082264D0(s32 n)
{
    struct S *p = gUnk_03005420;
    if (p) {
        if (n <= 1) {
            p->f1c = 0;
            sub_08226AAC(p);
        } else {
            p->f1c = 1;
            p->f68 = 6;
            p->f6a = n;
        }
    }
}
