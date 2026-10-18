#include "global.h"
#define Hud_ReadScriptQOption sub_08177D24
struct HudManager {
    u8 pad0[0x13F1];
    u8 scriptQOption;
};
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_08177D24(struct HudManager *p)
{
    if (Script_SeekToKeyword('q'))
        p->scriptQOption = Script_GetValue();
    else
        p->scriptQOption = 1;
}
