#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
u8 sub_0804A138(void)
{
    u8 *p = gUnk_02000488;
    u8 r;
    if (p) r = p[0x3E9];
    else r = 0;
    return r;
}
