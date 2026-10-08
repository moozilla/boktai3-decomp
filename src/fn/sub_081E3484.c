#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081E3400(void *);
void sub_0821A0C0(void *);
void sub_081E2B50(void);
void sub_081E2D40(void);
void *sub_081E3484(void)
{
    void *p = sub_08219FBC(8, 0x238);
    if (p) {
        sub_0821A04C(p, sub_081E2B50, sub_081E2D40);
        if (sub_081E3400(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
