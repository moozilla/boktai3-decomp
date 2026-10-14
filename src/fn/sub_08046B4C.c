#include "damage_number_display.h"
#define DamageNumberDisplay_GetActiveBit sub_08046B4C
u32 DamageNumberDisplay_GetActiveBit(struct DamageNumberDisplay *display, u32 index)
{
    return display->activeMask & (1 << index);
}
