#include "global.h"
extern u8 *gUnk_02000114;
void sub_08214514(u8 *);
void sub_08217EAC(u8 *);
u32 sub_08051DE4(u8 *p)
{
    u8 *q;
    s32 i;
    sub_08214514(p + 0x18);
    sub_08214514(p + 0x60);
    q = p + 0xC0;
    i = 7;
    do {
        sub_08217EAC(q);
        q += 0x34;
        i--;
    } while (i >= 0);
    return 0;
}
