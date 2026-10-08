#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C5308(void *, void *);
void sub_0821A0C0(void *);
void sub_081C52DC(void *);
void sub_081C52FC(void *);
void *sub_081C53EC(void *a)
{
    void *p = sub_08219FBC(8, 0xdc);
    if (p != 0) {
        sub_0821A04C(p, sub_081C52DC, sub_081C52FC);
        if (sub_081C5308(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
