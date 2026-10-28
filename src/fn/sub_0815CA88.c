#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void PlayerCue_OverlayOn(u8 *actor, s32 animation);
void ScriptActor_EnableCompletionScript(u8 *);
/* Script: the scripted actor chosen by -i plays animation -p (passed to
   PlayerCue_OverlayOn as -p + 366; tentative). */
void ScriptActor_SetAnimationFromScript(void)
{
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor && Script_SeekToKeyword('p')) {
        s32 animation = Script_GetValue();
        PlayerCue_OverlayOn(actor, animation + 366);
        ScriptActor_EnableCompletionScript(actor);
    }
}
