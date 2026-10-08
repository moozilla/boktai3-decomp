#include "global.h"

struct P0814A4B0 {
    u8 f0[0x34C]; s16 f34C;
    u8 f34E[0x3A4 - 0x34E]; u8 f3A4;
    u8 f3A5[0x4B6 - 0x3A5]; s16 f4B6;
};
u8 sub_08141BAC(void *);
void sub_0813ADD0(void *);

s32 sub_0814A4B0(struct P0814A4B0 *p)
{
    s32 r = 0;
    if (p->f34C != -1 && p->f4B6 == 0) {
        p->f3A4 = sub_08141BAC(p);
        r = 1;
    }
    sub_0813ADD0(p);
    return r;
}
