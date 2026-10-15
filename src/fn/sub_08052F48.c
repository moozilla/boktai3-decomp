#include "stacked_actor.h"
extern u8 *gUnk_02000580;
void sub_0815C99C(u8 *);
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
        sub_0815C99C(gUnk_02000580);
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
