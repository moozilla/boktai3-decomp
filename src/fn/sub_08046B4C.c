#include "damage_number_display.h"
u32 DamageNumberDisplay_GetActiveBit(struct DamageNumberDisplay *display, u32 index)
{
    return display->activeMask & (1 << index);
}
