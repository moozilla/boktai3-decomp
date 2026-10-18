#include "global.h"
extern const u32 gUnk_086060B4[];
void Skeleton_SetMovementControlPreset(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_086060B4;
}
