#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_BeginMode28(u8 *actor, s32 direction, s32 h, s32 e);
void ScriptActor_EnableCompletionScript(u8 *);
/* Script: PlayerCue_BeginMode28 on the scripted actor chosen by -i with -d, -h
   (default 1500) and -e (tentative). */
void ScriptActor_StartActionFromScript(void)
{
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor) {
        s32 direction = ScriptActor_GetDirectionFromScript();
        s32 e = ScriptActor_GetCueOptionFromScript();
        s32 h;
        if (Script_SeekToKeyword('h'))
            h = Script_GetValue();
        else
            h = 1500;
        PlayerCue_BeginMode28(actor, direction, h, e);
        ScriptActor_EnableCompletionScript(actor);
    }
}
