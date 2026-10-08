#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0B24(void *, void *);
void sub_0821A0C0(void *);
void sub_081C0A80(void *);
void sub_081C0A9C(void *);
void *sub_081C0B94(void *a)
{
    void *p = sub_08219FBC(8, 0x60);
    if (p != 0) {
        sub_0821A04C(p, sub_081C0A80, sub_081C0A9C);
        if (sub_081C0B24(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
