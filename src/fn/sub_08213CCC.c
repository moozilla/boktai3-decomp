#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_0821A0C0(void *);
s32 sub_08213B64(void *, u32);
void sub_08213888(void);
void sub_08213B50(void);
void *sub_08213CCC(u32 a)
{
    void *p = sub_08219FBC(8, 0x724);
    if (p) {
        sub_0821A04C(p, sub_08213888, sub_08213B50);
        if (sub_08213B64(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
