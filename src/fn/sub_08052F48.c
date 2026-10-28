#include "stacked_actor.h"
#include "player.h"
void PlayerCue_ReleaseControl(u8 *);
void ScriptUtility_ClearControlFlag(void);
s32 Script_Call(u32, void *);
void StackedActor_FinishPlayerInteraction(struct StackedActor *actor)
{
    actor->sprite.flags &= ~1;
    if (actor->sprite.position.y < actor->world.y + 22)
        actor->sprite.position.y += 2;
    if (actor->height) {
        actor->height--;
        StackedActor_UpdateSegments(actor);
    } else {
        PlayerCue_ReleaseControl((*(u8 **)gPlayers));
        ScriptUtility_ClearControlFlag();
        actor->shadow.flags &= ~1;
        if (actor->removeAfterUse)
            StackedActor_SetUpdate(actor, StackedActor_Disappear);
        else
            StackedActor_SetUpdate(actor, StackedActor_TickIdle);
        if (actor->completedScriptId)
            Script_Call(actor->completedScriptId, 0);
    }
}
