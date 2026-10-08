#include "global.h"
extern u32 gUnk_030053F4;
void sub_08049168(u8 *, s32);
void sub_08049234(u8 *, s32);
void sub_0804B43C(u8 *);
void sub_08048930(u8 *);
void sub_0804CF5C(u8 *);
void sub_0804D094(u8 *p)
{
    u16 *q;
    if (p[0x255] != 0x10) sub_08049168(p, 0x10);
    *(u16 *)(p + 0x136) &= ~4;
    q = (u16 *)(p + 0x38a);
    *q = 1;
    sub_08049234(p, 0);
    if ((gUnk_030053F4 & 0x200) == 0) {
        u32 v = *(u16 *)(p + 0x3fc);
        if (v != 0) {
            sub_0804B43C(p);
        } else if (*(u16 *)(p + 0x384) != 0) {
            *q = v;
            sub_08048930(p);
        } else {
            sub_0804CF5C(p);
        }
    }
}
