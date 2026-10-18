#include "global.h"

u8 Registry1745_ConsumeStateEntry(u8 *);
void Hud_SetMode(s32);

u32 Registry1745_EnterCompletedState(u8 *p)
{
    u8 r = Registry1745_ConsumeStateEntry(p);
    if (r != 0) {
        Hud_SetMode(7);
        p[0x1e] = 1;
    }
}
