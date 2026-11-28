#include "global.h"
#include "engine/engine_globals.h"
#include "objects/floor_switch/floor_switch_layout.h"
void FloorSwitch_RunStateScript(struct FloorSwitch *);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void FloorSwitch_Release(struct FloorSwitch *p)
{
    if (p->flags88 & 2) {
        p->flags88 |= 1;
        p->flags88 &= ~2;
        FloorSwitch_RunStateScript(p);
        p->field10 = 0;
        AnimData_SelectPaletteBank(p->anim, 0x16C);
        if (!(gEngineFlags & 0x800)) {
            if (p->flags88 & 4)
                Sound_PlaySong(0x389);
            else
                Sound_PlaySong(0x155);
        }
        p->field5A = 0;
    }
}
