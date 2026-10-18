#include "global.h"
void ResultGame_SetTrackingVisibility(u8 *p)
{
    u32 *field;
    field = (u32 *)(p + 0x324);
    field[2] &= ~1;
    field = (u32 *)(p + 0x5C4);
    field[2] |= 1;
}
