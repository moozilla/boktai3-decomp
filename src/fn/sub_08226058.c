#include "global.h"
struct S { u8 pad[0x54]; u16 f54; u16 f56; u8 pad2[0x60-0x58]; u16 f60; u16 f62; u16 f64; };
extern struct S *gUnk_03005420;
void sub_08226058(s32 n, u16 *a)
{
    struct S *p = gUnk_03005420;
    if (p) {
        p->f60 = a[0];
        p->f62 = a[1];
        p->f64 = a[2];
        if (n <= 1) {
            p->f54 = 1;
        } else {
            p->f56 = n;
            p->f54 = 2;
        }
    }
}
