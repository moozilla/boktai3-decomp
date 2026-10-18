#include "global.h"
extern u8 *gUnk_0200025C;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetKeywordValue(u32, u32);
void Registry2388_StartTimedEffect(u8 *, u32, s32);
void Registry2388_StartTimedEffectFromScript(void)
{
    u8 *p = gUnk_0200025C;
    if (p) {
        if (Script_SeekToKeyword(0x66)) {
            s32 v = Script_GetValue();
            Registry2388_StartTimedEffect(p, Script_GetKeywordValue(0x74, 7), v);
        }
    }
}
