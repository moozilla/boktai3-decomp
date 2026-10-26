#include "damage_number_display.h"
void DamageNumberDisplay_RemoveGroup(struct DamageNumberDisplay *, s32);
u32 DamageNumberDisplay_Destroy(struct DamageNumberDisplay *display)
{
    s32 index = 0;
    do {
        DamageNumberDisplay_RemoveGroup(display, index);
        ++index;
    } while (index <= 3);
    return (u32)(gDamageNumberDisplay = NULL);
}
