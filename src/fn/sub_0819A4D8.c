#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0819A470(void *);
void sub_0821A0C0(void *);
void sub_0819A23C(void);
void sub_0819A414(void);
void *sub_0819A4D8(u16 a)
{
    u32 *p = sub_08219FBC(8, 0x1328);
    if (p) {
        p[6] = a;
        sub_0821A04C(p, sub_0819A23C, sub_0819A414);
        if (sub_0819A470(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
