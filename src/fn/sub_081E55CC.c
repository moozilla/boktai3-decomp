#include "global.h"
extern u8 *gUnk_020003C8;
u8 *Actor_EnsureController(u32, u32);
u8 Script_GetKeywordValue(u32, u32);
void Actor_SetPreviousAnimationStateFromScript(void)
{
    u8 *p = gUnk_020003C8;
    if (p == 0) {
        p = Actor_EnsureController(0xBB34, 0);
        if (p == 0)
            return;
    }
    p[0xF6C] = Script_GetKeywordValue(0x72, 0);
}
