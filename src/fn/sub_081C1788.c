#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C1578(void *);
void sub_0821A0C0(void *);
u32 sub_081C1384(void *);
u32 sub_081C13C0(void *);
void *sub_081C1788(void)
{
    void *p = sub_08219FBC(0xb, 0x464);
    if (p != 0) {
        sub_0821A04C(p, (void (*)(void *))sub_081C1384, (void (*)(void *))sub_081C13C0);
        if (sub_081C1578(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
