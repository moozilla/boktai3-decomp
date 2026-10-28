#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void PlayerCue_Hide(u8 *);
void ScriptActor_EnableCompletionScript(u8 *);
void ScriptActor_HideFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p) {
        PlayerCue_Hide(p);
        ScriptActor_EnableCompletionScript(p);
    }
}
