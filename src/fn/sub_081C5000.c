#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C4EF0(void *);
void sub_0821A0C0(void *);
void sub_081C4EDC(void *);
void sub_081C4EEC(void *);
void *sub_081C5000(void)
{
    void *p = sub_08219FBC(0xb, 0x40);
    if (p != 0) {
        sub_0821A04C(p, sub_081C4EDC, sub_081C4EEC);
        if (sub_081C4EF0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
