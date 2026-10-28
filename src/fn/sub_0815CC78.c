#include "global.h"
#include "engine.h"
#include "player.h"
/* ScriptActor_RunToFromScript: Reads target coordinates from -p and invokes PlayerCue_RunTo */
#define ScriptActor_RunToFromScript sub_0815CC78
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Script_GetActorIndex(void);
s32 ScriptActor_GetDirectionFromScript(void);
s32 ScriptActor_GetCueOptionFromScript(void);
void PlayerCue_RunTo(u8 *actor, struct PackedVec *point, s32 direction, s32 e);
void ScriptActor_EnableCompletionScript(u8 *);
/* Sibling of ScriptActor_MoveToFromScript (not referenced directly by the
   registry): -p x y z, -d and -e for PlayerCue_RunTo. */
void sub_0815CC78(void)
{
    struct PackedVec point;
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor && Script_SeekToKeyword('p')) {
        s32 direction, e;
        point.x = Script_GetValue();
        point.y = Script_GetValue();
        point.z = Script_GetValue();
        direction = ScriptActor_GetDirectionFromScript();
        e = ScriptActor_GetCueOptionFromScript();
        PlayerCue_RunTo(actor, &point, direction, e);
        ScriptActor_EnableCompletionScript(actor);
    }
}
