#include "global.h"

extern const u8 gUnk_0824F878[];
struct Q {
    u8 f[0x22c];
    u32 w;
};
void memcpy(void *, const void *, s32);
u32 sub_081BB1A0(s32, s32, s32, void *);

void sub_081BAB7C(struct Q *p, s32 a)
{
    u8 buf[0xb0];
    memcpy(buf, gUnk_0824F878, 0xb0);
    p->w = sub_081BB1A0(1, a, 0xb, buf);
}
