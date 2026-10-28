#include "global.h"
#include "engine.h"
#include "player.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_MoveTo(u8 *actor, struct PackedVec *point, s32 direction, s32 f, s32 e);
void ScriptActor_EnableCompletionScript(u8 *);
/* Script: PlayerCue_MoveTo on the scripted actor chosen by -i, with the optional
   point -p x y z, -d (default -1), -f (default 0) and -e (default 0);
   tentative: turn to a direction or point. */
void ScriptActor_TurnFromScript(void)
{
    struct PackedVec point;
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor) {
        s32 f, direction, e;
        if (Script_SeekToKeyword('f'))
            f = Script_GetValue();
        else
            f = 0;
        direction = ScriptActor_GetDirectionFromScript();
        e = ScriptActor_GetCueOptionFromScript();
        if (Script_SeekToKeyword('p')) {
            point.x = Script_GetValue();
            point.y = Script_GetValue();
            point.z = Script_GetValue();
            PlayerCue_MoveTo(actor, &point, direction, f, e);
        } else
            PlayerCue_MoveTo(actor, 0, direction, f, e);
        ScriptActor_EnableCompletionScript(actor);
    }
}
