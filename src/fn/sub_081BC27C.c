#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081BC22C(void *);
void sub_0821A0C0(void *);
void sub_081BBD94(void);
void sub_081BBDA8(void);
void *sub_081BC27C(void)
{
    void *p = sub_08219FBC(0xb, 0x734);
    if (p) {
        sub_0821A04C(p, sub_081BBD94, sub_081BBDA8);
        if (sub_081BC22C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
