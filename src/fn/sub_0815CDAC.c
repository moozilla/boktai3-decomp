#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_Motion7Front(u8 *actor, s32 f, s32 e);
void PlayerCue_Stand(u8 *actor, s32 state);
void ScriptActor_EnableCompletionScript(u8 *);
void ScriptActor_CallCompletionScript(u8 *);
/* Script: puts the scripted actor chosen by -i into state 4 with -e (default 0).
   From states 0, 1 and 6 it goes through PlayerCue_Motion7Front, which also takes -f. */
void ScriptActor_EnterStateFourFromScript(void)
{
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor) {
        s32 e = ScriptActor_GetCueOptionFromScript();
        u8 state = actor[0x418];
        if (state <= 1 || state == 6) {
            s32 f; /* meaning unknown */
            if (Script_SeekToKeyword('f'))
                f = Script_GetValue();
            else
                f = 0;
            PlayerCue_Motion7Front(actor, f, e);
            ScriptActor_EnableCompletionScript(actor);
        } else {
            *(s32 *)(actor + 0x5A4) = e;
            PlayerCue_Stand(actor, 4);
            ScriptActor_EnableCompletionScript(actor);
            ScriptActor_CallCompletionScript(actor);
        }
    }
}
