#include "global.h"
#include "engine.h"
#include "game_state.h"
/* Script: sets bit -i in the flag word of area -s (default: the current area,
   read from +0x5AC, which game_state.h names savedX (guess)). */
void GameState_SetAreaFlagFromScript(void)
{
    s32 area;
    if (Script_SeekToKeyword('s'))
        area = Script_GetValue();
    else
        area = (s16)gUnk_02000710->savedX;
    if (Script_SeekToKeyword('i')) {
        s32 bit = Script_GetValue();
        ((u32 *)gUnk_02000710->unk5C0)[area] |= 1 << bit;
    }
}
