#include "global.h"
extern u8 *gUnk_02000214;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Hud_SetGaugeDisplayFlagFromScript(void)
{
    u8 *p = gUnk_02000214;
    if (p) {
        if (Script_SeekToKeyword(0x66))
            p[0xa6c] = Script_GetValue();
    }
}
