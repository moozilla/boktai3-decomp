#include "damage_number_display.h"
#define DamageNumberDisplay_SetActiveBit sub_08046B34
struct DamageNumberDisplay *DamageNumberDisplay_SetActiveBit(struct DamageNumberDisplay *display,
                                                             u32 index)
{
    display->activeMask |= 1 << index;
    return display;
}
