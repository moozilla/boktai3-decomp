#include "global.h"
#include "ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Hud_SetPanelIndexFromScript(void)
{
    u8 *p = gFieldHud;
    if (p) {
        if (Script_SeekToKeyword(0x69))
            p[0x4864] = Script_GetValue();
    }
}
