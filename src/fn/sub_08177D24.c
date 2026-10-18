#include "global.h"
struct HudManager {
    u8 pad0[0x13F1];
    u8 scriptQOption;
};
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Hud_ReadScriptQOption(struct HudManager *p)
{
    if (Script_SeekToKeyword('q'))
        p->scriptQOption = Script_GetValue();
    else
        p->scriptQOption = 1;
}
