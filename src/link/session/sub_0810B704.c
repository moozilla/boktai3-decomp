#include "global.h"

u8 LinkSession_ConsumeStateEntry(u8 *);
void Hud_SetMode(s32);

u32 LinkSession_EnterCompletedState(u8 *p)
{
    u8 r = LinkSession_ConsumeStateEntry(p);
    if (r != 0) {
        Hud_SetMode(7);
        p[0x1e] = 1;
    }
}
