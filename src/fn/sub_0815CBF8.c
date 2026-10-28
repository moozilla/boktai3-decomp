#include "global.h"
#include "engine.h"
#include "player.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_WalkTo(u8 *actor, struct PackedVec *destination, s32 e, s32 direction);
void ScriptActor_EnableCompletionScript(u8 *);
/* Script: -p x y z destination for the scripted actor chosen by -i, with -d
   (default -1) and -e (default 0); tentative: starts a move to that point. */
void ScriptActor_MoveToFromScript(void)
{
    struct PackedVec destination;
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor && Script_SeekToKeyword('p')) {
        s32 direction, e;
        destination.x = Script_GetValue();
        destination.y = Script_GetValue();
        destination.z = Script_GetValue();
        direction = ScriptActor_GetDirectionFromScript();
        e = ScriptActor_GetCueOptionFromScript();
        PlayerCue_WalkTo(actor, &destination, e, direction);
        ScriptActor_EnableCompletionScript(actor);
    }
}
