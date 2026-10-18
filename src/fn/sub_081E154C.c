#include "global.h"

extern void sub_082195E0(void *);
extern void sub_08214514(void *);

void sub_081E154C(u8 *p)
{
    s32 i;
    u8 *q;
    u8 *a, *b;
    vu16 *r = (vu16 *)0x03004BD8;
    s32 m = 4;
    *r = *r & -m;
    q = p + 0x3b4;
    for (i = 1; i >= 0; i--) {
        sub_082195E0(q);
        q += 0x60;
    }
    a = p + 0xa4;
    b = p + 0x1c;
    q = p + 0x1d4;
    for (i = 4; i >= 0; i--) {
        sub_082195E0(q);
        q += 0x60;
    }
    sub_08214514(p + 0x474);
    sub_082195E0(p + 0x12c);
    sub_082195E0(a);
    sub_082195E0(b);
}
