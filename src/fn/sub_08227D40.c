#include "global.h"
#include "render.h"
u32 Script_GetValue(void);
void Background_SetVisibilityFlagFromScript(void)
{
    u16 n = Script_GetValue();
    u32 m = (0x1000000u << n) >> 16;
    gDispCntShadow = m | gDispCntShadow;
}
