#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081F2C64(void *, void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081F2BB0(void);
void sub_081F2BFC(void);

void *sub_081F2C8C(void *a, void *b, void *c)
{
    void *r = sub_08219FBC(10, 0x68);
    if (r != 0) {
        sub_0821A04C(r, sub_081F2BB0, sub_081F2BFC);
        if (sub_081F2C64(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
