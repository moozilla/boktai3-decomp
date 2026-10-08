#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081BD708(void *);
void sub_0821A0C0(void *);
void sub_081BD2C0(void);
void sub_081BD2D4(void);

void *sub_081BD814(void)
{
    void *r = sub_08219FBC(0xb, 2356);
    if (r != 0) {
        sub_0821A04C(r, sub_081BD2C0, sub_081BD2D4);
        if (sub_081BD708(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
