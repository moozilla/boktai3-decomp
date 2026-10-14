#include "damage_number_display.h"
#define DamageNumberDisplay_ClearActiveBit sub_08046B40
void DamageNumberDisplay_ClearActiveBit(struct DamageNumberDisplay *display, u32 index)
{
    display->activeMask &= ~(1 << index);
}
