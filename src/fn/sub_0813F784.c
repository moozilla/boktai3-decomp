#include "global.h"
extern u32 gUnk_030053F4;
extern const u16 gUnk_0861091C[], gUnk_08610940[];
u32 ActorParameter_GetModeValue(s32 index)
{
    if (gUnk_030053F4 & 0x1000)
        return gUnk_08610940[index];
    else
        return gUnk_0861091C[index];
}
