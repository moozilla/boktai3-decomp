#include "global.h"

s32 Script_GetKeywordValue(s32, s32);
u8 *Pool1BC_FindByName(void);

void AnimatedTile_StartAlternateAnimationFromScript(void)
{
    if (Script_GetKeywordValue(0x6e, 0) != 0) {
        u8 *p = Pool1BC_FindByName();
        if (p != 0) {
            *(u16 *)(p + 0x60) = 2;
            p[0x65] = 1;
        }
    }
}
