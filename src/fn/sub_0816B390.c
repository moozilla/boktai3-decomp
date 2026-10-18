#include "global.h"
extern u8 *gUnk_02000710;
extern u16 gUnk_03004BD8;
u32 Hud_GetScaledRetryCount(void);
void Hud_InitRetryPanel(void *);
void Hud_BeginRetryPanel(u8 *s)
{
    s[0x48E6] = Hud_GetScaledRetryCount();
    {
        u16 *p = &gUnk_03004BD8;
        u32 mask = ~0x200;
        *p &= mask;
    }
    Hud_InitRetryPanel(s);
}
