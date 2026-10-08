#include "global.h"

struct P { u8 f00[0x34]; u32 w34; u8 f38[0x60 - 0x38]; u32 w60; u8 f64[4]; u32 w68; };

void sub_0816044C(struct P *);

s32 sub_081604C0(struct P *p)
{
    u32 m = 1;
    if (p->w60 & m) {
        p->w34 |= m;
        return 0;
    }
    sub_0816044C(p);
    p->w68++;
}
