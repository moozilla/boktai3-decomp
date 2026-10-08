#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C47A0(void *, void *);
void sub_0821A0C0(void *);
void sub_081C46C0(void *);
void sub_081C46D0(void *);
void *sub_081C47EC(void *a)
{
    void *p = sub_08219FBC(0xb, 0x70);
    if (p != 0) {
        sub_0821A04C(p, sub_081C46C0, sub_081C46D0);
        if (sub_081C47A0(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
