#include "damage_number_display.h"
void DamageNumberDisplay_ClearActiveBit(struct DamageNumberDisplay *display, u32 index)
{
    display->activeMask &= ~(1 << index);
}
