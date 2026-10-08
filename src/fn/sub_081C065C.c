#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C0608(void *, void *);
void sub_0821A0C0(void *);
void sub_081C05F8(void *);
void sub_081C0604(void *);
void *sub_081C065C(void *a)
{
    void *p = sub_08219FBC(0xb, 0x38);
    if (p != 0) {
        sub_0821A04C(p, sub_081C05F8, sub_081C0604);
        if (sub_081C0608(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
