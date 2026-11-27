#include "global.h"
#include "objects/floor_switch/floor_switch_layout.h"
void FloorSwitch_RunStateScript(struct FloorSwitch *);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void FloorSwitch_SetOccupant(struct FloorSwitch *p, u32 occupant)
{
    p->field5A = occupant;
    if (p->flags88 & 1) {
        p->flags88 |= 2;
        p->flags88 &= ~1;
        p->field8C = 0;
        FloorSwitch_RunStateScript(p);
        p->field10 = 1;
        AnimData_SelectPaletteBank(p->anim, 0x16D);
        if (p->flags88 & 4)
            Sound_PlaySong(0x389);
        else
            Sound_PlaySong(0x155);
    }
}
