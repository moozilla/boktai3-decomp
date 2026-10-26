#include "global.h"
#include "render.h"
#include "game_state.h"
u32 Hud_GetScaledRetryCount(void);
void Hud_InitRetryPanel(void *);
void Hud_BeginRetryPanel(u8 *s)
{
    s[0x48E6] = Hud_GetScaledRetryCount();
    {
        u16 *p = &gDispCntShadow;
        u32 mask = ~0x200;
        *p &= mask;
    }
    Hud_InitRetryPanel(s);
}
