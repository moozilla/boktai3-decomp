#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080039F0(void *);
void sub_0821A0C0(void *);
void sub_080039BC(void);
void sub_080039E4(void);

void *sub_08003A68(void)
{
    void *p = sub_08219FBC(0xb, 0x15c);
    if (p != 0) {
        sub_0821A04C(p, sub_080039BC, sub_080039E4);
        if (sub_080039F0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
