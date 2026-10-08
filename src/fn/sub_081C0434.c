#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0390(void *);
void sub_0821A0C0(void *);
void sub_081C0344(void *);
void sub_081C0378(void *);
void *sub_081C0434(void)
{
    void *p = sub_08219FBC(9, 0xac);
    if (p != 0) {
        sub_0821A04C(p, sub_081C0344, sub_081C0378);
        if (sub_081C0390(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
