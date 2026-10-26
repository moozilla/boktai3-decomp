#include "global.h"
#include "ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Hud_SetGaugeDisplayFlagFromScript(void)
{
    u8 *p = gFieldHud;
    if (p) {
        if (Script_SeekToKeyword(0x66))
            p[0xa6c] = Script_GetValue();
    }
}
