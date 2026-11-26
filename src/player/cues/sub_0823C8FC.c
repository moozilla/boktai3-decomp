#include "global.h"
// confidence=low: tests the perpendicular cell coordinate against the terrain sidestep margin.
#define Player_IsWithinSidestepCellMargin sub_0823C8FC
s32 Player_IsWithinSidestepCellMargin(u8 *p)
{
    u32 direction = p[0x4A5];
    u32 coordinate;
    u32 test;
    if (direction == 1 || direction == 5)
        coordinate = *(u16 *)(p + 0x30);
    else
        coordinate = *(u16 *)(p + 0x34);
    test = 255;
    test &= coordinate;
    test -= 31;
    if (test > 194)
        return 0;
    return 1;
}
