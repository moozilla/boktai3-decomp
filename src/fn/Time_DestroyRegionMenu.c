#include "global.h"
void Anim_Remove(u8 *);
void Time_DestroyRegionMenu(u8 *p)
{
    Anim_Remove(p + 0x1f8);
    Anim_Remove(p + 0x170);
    Anim_Remove(p + 0xe8);
    Anim_Remove(p + 0x60);
}
