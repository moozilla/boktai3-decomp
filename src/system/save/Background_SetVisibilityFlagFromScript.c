#include "global.h"
#include "engine/video/render.h"
#include "gba/io_reg.h"
u32 Script_GetValue(void);
void Background_SetVisibilityFlagFromScript(void)
{
    u16 layer = Script_GetValue();
    u16 mask = DISPCNT_BG0_ON << layer;

    gDispCntShadow |= mask;
}
