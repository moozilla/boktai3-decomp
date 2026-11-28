#include "npc/manager/scene_actor_lifecycle.h"

void Kuro_DispatchCutsceneCommands(struct SceneActor *);
void Actor_UpdateMovementAnimationAndEffects(void *, struct SceneActor *);

s32 Kuro_OnFrameUpdate(void *ctx, struct SceneActor *actor)
{
    Kuro_DispatchCutsceneCommands(actor);
    Actor_UpdateMovementAnimationAndEffects(ctx, actor);
    actor->action = 0;
    return 0;
}
