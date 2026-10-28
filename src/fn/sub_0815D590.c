#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScriptActor_EnableCompletionScript(s32);
void PlayerCue_ReleaseControl(s32);
void ScriptActor_ResetActionStateFromScript(void)
{
    s32 i = Script_GetActorIndex();
    s32 p = gPlayers[i];
    if (p != 0)
        PlayerCue_ReleaseControl(p);
}
