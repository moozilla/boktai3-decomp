#include "objects/warp_circle/warp_circle.h"
#include "player/player.h"
void PlayerCue_ReleaseControl(u8 *);
void ScriptUtility_ClearControlFlag(void);
s32 Script_Call(u32, void *);
void WarpCircle_FinishPlayerInteraction(struct WarpCircle *actor)
{
    actor->sprite.flags &= ~1;
    if (actor->sprite.position.y < actor->world.y + 22)
        actor->sprite.position.y += 2;
    if (actor->height) {
        actor->height--;
        WarpCircle_UpdateSegments(actor);
    } else {
        PlayerCue_ReleaseControl((*(u8 **)gPlayers));
        ScriptUtility_ClearControlFlag();
        actor->shadow.flags &= ~1;
        if (actor->removeAfterUse)
            WarpCircle_SetUpdate(actor, WarpCircle_Disappear);
        else
            WarpCircle_SetUpdate(actor, WarpCircle_TickIdle);
        if (actor->completedScriptId)
            Script_Call(actor->completedScriptId, 0);
    }
}
