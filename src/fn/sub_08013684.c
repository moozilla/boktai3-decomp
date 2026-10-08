#include "global.h"
extern u32 gUnk_02000054;
void sub_08217EAC(u8 *);
void sub_080133F4(u32, u8 *);
u32 sub_08013684(u8 *p)
{
    sub_08217EAC(p + 0xc);
    if (gUnk_02000054 != 0)
        sub_080133F4(gUnk_02000054, p);
    return 0;
}
