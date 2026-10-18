#include "global.h"
void Anim_Remove(u8 *);
void PauseMenu_RemoveEquipmentSlotAnimations(u8 *p)
{
    u8 *q = p + 0xB08;
    s32 i = 2;
    do {
        Anim_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
}
