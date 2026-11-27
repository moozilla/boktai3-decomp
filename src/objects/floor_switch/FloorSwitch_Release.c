// MATCHING_CONCESSION: register-pinning (register pin and empty asm operand hint)
#include "global.h"
#include "engine/engine_globals.h"
#include "objects/floor_switch/floor_switch_layout.h"
void FloorSwitch_RunStateScript(struct FloorSwitch *);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void FloorSwitch_Release(struct FloorSwitch *p)
{
    u16 *flags = &p->flags88;
    u32 f = *flags;
    if (2 & f) {
        u32 one;
        u32 zero;
        u32 v;
        u32 mask;
        one = 1;
        zero = 0;
        v = one;
        v = v | f;
        mask = 0xFFFD;
        do {
            *flags = v & mask;
            FloorSwitch_RunStateScript(p);
            p->field10 = zero;
            AnimData_SelectPaletteBank(p->anim, 0x16C);
        } while (0);
        if (!(gEngineFlags & 0x800)) {
            if (*flags & 4)
                Sound_PlaySong(0x389);
            else
                Sound_PlaySong(0x155);
        }
        p->field5A = 0;
    }
}
