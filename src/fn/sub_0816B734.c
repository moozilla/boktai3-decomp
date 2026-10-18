#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern u8 *gUnk_02000710;
void GameState_SetFloorFromScript(void)
{
    if (Script_SeekToKeyword(0x66))
        *(u16 *)(gUnk_02000710 + 0x5ae) = Script_GetValue();
}
