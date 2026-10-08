#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081AF640(void *, s32, s32);
void sub_0821A0C0(void *);
void sub_081AF5EC(void);
void sub_081AF628(void);

void *sub_081AF6F4(s32 a, s32 b)
{
    void *r = sub_08219FBC(9, 0x64);
    if (r != 0) {
        sub_0821A04C(r, sub_081AF5EC, sub_081AF628);
        if (sub_081AF640(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
