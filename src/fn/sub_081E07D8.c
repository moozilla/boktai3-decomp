#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081E056C(void *);
void sub_0821A0C0(void *);
void sub_081E04B4(void);
void sub_081E0560(void);

void *sub_081E07D8(void)
{
    void *r = sub_08219FBC(0xb, 1228);
    if (r != 0) {
        sub_0821A04C(r, sub_081E04B4, sub_081E0560);
        if (sub_081E056C(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
