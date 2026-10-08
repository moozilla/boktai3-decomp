#include "global.h"
extern u8 *gUnk_02000610;
s32 Script_ParseStringRef(u8 *p)
{
    s32 v = (s16)((p[2] << 8) | p[1]);
    gUnk_02000610 = p + 3;
    return v;
}
