#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void ScriptActor_EnableCompletionScript(u8 *);
void PlayerCue_Windblown(u8 *, s32, s32);

void sub_0815D394(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor != NULL) {
        s32 value;
        if (Script_SeekToKeyword('r'))
            value = Script_GetValue();
        else
            value = 0;
        PlayerCue_Windblown(actor, value, ScriptActor_GetCueOptionFromScript());
        ScriptActor_EnableCompletionScript(actor);
    }
}
