#include "global.h"

#define GameState_ClearAreaFlagFromScript sub_0816B8D8
extern u8 *gUnk_02000710;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
/* Script: clears bit -i of area -s (default: game state +0x5AC) in the words
   at +0x5C0; counterpart of GameState_SetAreaFlagFromScript. */
void GameState_ClearAreaFlagFromScript(void)
{
    s32 area;
    if (Script_SeekToKeyword('s'))
        area = Script_GetValue();
    else
        area = *(s16 *)(gUnk_02000710 + 0x5AC);
    if (Script_SeekToKeyword('i')) {
        s32 bit = Script_GetValue();
        *(u32 *)(area * 4 + gUnk_02000710 + 0x5C0) &= ~(1 << bit);
    }
}
