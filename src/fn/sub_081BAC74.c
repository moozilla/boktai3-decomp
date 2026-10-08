#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081BABB0(void *, void *);
void sub_0821A0C0(void *);
void sub_081BAB20(void);
void sub_081BAB48(void);
void *sub_081BAC74(void *a)
{
    void *p = sub_08219FBC(8, 0x268);
    if (p) {
        sub_0821A04C(p, sub_081BAB20, sub_081BAB48);
        if (sub_081BABB0(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
