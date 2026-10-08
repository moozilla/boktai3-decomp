#include "global.h"
extern u8 *gUnk_02000710;
void sub_0811BA1C(s32 x)
{
    s32 t;
    *(s16 *)(gUnk_02000710 + 0x890) = x;
    t = *(s16 *)(gUnk_02000710 + 0x890);
    if (t > 0xb) t = 0xb;
    else if (t < 0) t = 0;
    *(s16 *)(gUnk_02000710 + 0x890) = t;
}
