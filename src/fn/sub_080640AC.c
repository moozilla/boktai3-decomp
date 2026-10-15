#include "roaming_object.h"
#define RoamingObjectB_FlashPalette sub_080640AC
void RoamingObjectB_FlashPalette(struct RoamingObject *owner)
{
    if (owner->flashCount <= 5)
        AnimData_SelectPaletteBank(owner->graphics, 0x192);
    else
        AnimData_SelectPaletteBank(owner->graphics, 0x193);
    owner->animation = 2;
    owner->flashCount++;
}
