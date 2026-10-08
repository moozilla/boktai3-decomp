#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081A522C(void *, s32, s32, s32);
void sub_0821A0C0(void *);
void sub_081A516C(void);
void sub_081A5198(void);

void *sub_081A5314(s32 a, s32 b, s32 c)
{
    void *r = sub_08219FBC(10, 0x13c);
    if (r != 0) {
        sub_0821A04C(r, sub_081A516C, sub_081A5198);
        if (sub_081A522C(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
