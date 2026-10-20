#include "global.h"
#include "engine.h"
void *BestiaryDisplay_GetEntryForActor(s32 k, s32 c);
void GameState_SetPairFlag(void *, s32 i);
void RegistryA76D_SetFromScript(void)
{
    s32 k, c, i;
    if (Script_SeekToKeyword('k'))
        k = Script_GetValue();
    else
        k = 0;
    if (Script_SeekToKeyword('c'))
        c = Script_GetValue();
    else
        c = 0;
    if (Script_SeekToKeyword('i'))
        i = Script_GetValue();
    else
        i = 0;
    GameState_SetPairFlag(BestiaryDisplay_GetEntryForActor(k, c), i);
}
