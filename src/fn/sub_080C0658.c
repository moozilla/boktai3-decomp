#include "global.h"
extern const u32 gUnk_08606098[];
void Skeleton_SetAttackControlTable(u8 *p)
{
    *(const u32 **)(p + 0x288) = gUnk_08606098;
}
