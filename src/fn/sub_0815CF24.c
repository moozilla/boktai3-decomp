#include "global.h"

#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_BeginMode27(u8 *, s32, s32, s32);
/* Script: on the scripted actor picked by -i, PlayerCue_BeginMode27 with -d
   (direction, -1 to keep), -f and -e (tentative). */
void ScriptActor_FaceFromScript(void)
{
    s32 i = Script_GetActorIndex();
    u8 *actor = gPlayers[i];
    if (actor) {
        s32 a = ScriptActor_GetDirectionFromScript();
        s32 b = ScriptActor_GetCueOptionFromScript();
        s32 f;
        if (Script_SeekToKeyword('f'))
            f = Script_GetValue();
        else
            f = 0;
        PlayerCue_BeginMode27(actor, a, f, b);
    }
}
