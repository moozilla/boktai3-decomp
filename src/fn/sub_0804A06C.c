#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
u32 sub_0804A06C(void)
{
    u8 *p = gUnk_02000488;
    if (!p) return 0;
    if (p[0x1A] != 4) return 0;
    if (p[0x255] > 4) return 0;
    sub_08049168(p, 9);
    return 1;
}
