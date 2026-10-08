#include "global.h"
extern u8 *gUnk_02000710;
void sub_0811BA54(s32 x)
{
    s32 t;
    *(s32 *)(gUnk_02000710 + 0x88c) = x;
    t = *(s32 *)(gUnk_02000710 + 0x88c);
    if (t > 0xEA60) t = 0xEA60;
    else if (t < 0) t = 0;
    *(s32 *)(gUnk_02000710 + 0x88c) = t;
}
