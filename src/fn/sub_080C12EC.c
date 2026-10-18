#include "global.h"
extern const u32 gUnk_086060A0[];
void Skeleton_SetReactionControlTable(u8 *p)
{
    *(const u32 **)(p + 0x28c) = gUnk_086060A0;
}
