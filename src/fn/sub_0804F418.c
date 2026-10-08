#include "global.h"
extern u8 *gUnk_02000114;
void sub_08214514(u8 *);
void sub_082195E0(u8 *);
void sub_0821FE6C(u8 *);
u32 sub_0804F418(u8 *p)
{
    u8 *q;
    s32 i;
    sub_08214514(p + 0x18);
    q = p + 0x80;
    i = 3;
    do {
        sub_082195E0(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    sub_0821FE6C(p + 0x200);
    return 0;
}
