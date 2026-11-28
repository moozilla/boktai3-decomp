#include "global.h"
#include "engine/runtime.h"
#include "system/game_state/game_state.h"
u32 Script_GetValue(void);
s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
void Script_SetEntryProcedure(s32);
s32 SceneLifecycle_LoadFromScript(void)
{
    u16 s = Script_GetValue();
    gSceneTransitionFlags = 1;
    if (Script_SeekToKeyword(0x6e) == 0 || (Script_GetPc() != 0 && Script_GetValue() == 0))
        gSceneTransitionFlags |= 0x10;
    Script_SetEntryProcedure((s16)s);
    gGameState->unk5A4 = s;
    return 0;
}
