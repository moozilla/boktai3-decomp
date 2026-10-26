#include "global.h"
extern u8 *gGameState;
extern u32 gUnk_030054B0;
s32 Script_GetValue(void);
void Region_SetCurrentFromScript(void)
{
    *(s32 *)(gGameState + 0x5a0) = Script_GetValue();
    gUnk_030054B0 = 1;
}
