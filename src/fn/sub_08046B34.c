#include "damage_number_display.h"
struct DamageNumberDisplay *DamageNumberDisplay_SetActiveBit(struct DamageNumberDisplay *display,
                                                             u32 index)
{
    display->activeMask |= 1 << index;
    return display;
}
