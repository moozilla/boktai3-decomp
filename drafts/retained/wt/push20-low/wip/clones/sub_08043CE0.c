#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08043C4C(void *, s32, s32, s32);
void sub_0821A0C0(void *);
void sub_08043ACC(void);
void sub_08043C24(void);

void *sub_08043CE0(s32 a, s32 b, s32 c)
{
    void *r = sub_08219FBC(8, 0x13c);
    if (r != 0) {
        sub_0821A04C(r, sub_08043ACC, sub_08043C24);
        if (sub_08043C4C(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
