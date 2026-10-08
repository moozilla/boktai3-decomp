#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0803F6C4(void *, void *);
void sub_0821A0C0(void *);
void sub_0803F690(void);
void sub_0803F6B0(void);

void *sub_0803F6F0(void *a)
{
    void *p = sub_08219FBC(9, 0x20);
    if (p != 0) {
        sub_0821A04C(p, sub_0803F690, sub_0803F6B0);
        if (sub_0803F6C4(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
