#include "global.h"
extern u8 *gGameState;
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Hud_UpdateCountdownDisplay(u32, u32, u32);
u32 Countdown_InitializeFromScript(u8 *p)
{
    s32 *q = (s32 *)(gGameState + 0x618);
    s32 r;
    if (*q <= 0)
        *q = 1;
    if (Script_SeekToKeyword(0x73))
        r = Script_GetValue();
    else
        r = 0x1E;
    *(u16 *)(p + 0x1A) = r;
    *(u16 *)(p + 0x18) = 0;
    Hud_UpdateCountdownDisplay(*(u32 *)(gGameState + 0x618), *(u32 *)(gGameState + 0x61C), 0);
    return 0;
}
