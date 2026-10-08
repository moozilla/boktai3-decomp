#include "global.h"
extern u8 *gUnk_02000710;
void sub_0811BAD8(s32 x)
{
    s32 t;
    *(s16 *)(gUnk_02000710 + 0x894) = x;
    t = *(s16 *)(gUnk_02000710 + 0x894);
    if (t > 0x3E7) t = 0x3E7;
    else if (t < 0) t = 0;
    *(s16 *)(gUnk_02000710 + 0x894) = t;
}
