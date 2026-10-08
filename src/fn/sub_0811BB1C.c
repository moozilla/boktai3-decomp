#include "global.h"
extern u8 *gUnk_02000710;
s16 sub_0811BB1C(void)
{
    s32 t = *(s16 *)(gUnk_02000710 + 0x890);
    if (t > 0xb) t = 0xb;
    else if (t < 0) t = 0;
    *(s16 *)(gUnk_02000710 + 0x890) = t;
    return *(s16 *)(gUnk_02000710 + 0x890);
}
