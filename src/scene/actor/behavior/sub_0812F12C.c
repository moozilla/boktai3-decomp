#include "global.h"
#include "scene/actor/scene_actor_registry.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "scene/actor/scene_actor_availability.h"
#include "scene/actor/scene_actor_behavior.h"
extern u32 gUnk_02000560;
void SceneActorPlacement_HideActor(struct ChaseActor *, s32);
void SceneActorPlacement_ShowActor(struct ChaseActor *, s32);
s32 SceneActorMovement_UpdateStatus(struct ChaseActor *);
s32 SceneActorMovement_UpdateAnimationAndTerrain(struct ChaseActor *);
void SceneActorPlacement_ConsumeCallbackFlag(struct ChaseActor *);
void SceneActorPlacement_ConsumeShakeFlags(struct ChaseActor *);
s32 SceneActorMovement_RequestAvailableAction(struct ChaseActor *);
void SceneActorTarget_OnBindingChanged(struct ChaseActor *);
static inline u8 sameRegion(u16 *region)
{
    if (gGameState->region == *region)
        return 1;
    return 0;
}
static inline void dispatch(void **callback, void *context)
{
    Callback_InvokeActor(context, *callback);
}
s32 SceneActor_UpdateActiveCallbacks(struct ChaseActor *p)
{
    if (SceneActorBehavior_GlobalPauseBits(0x400))
        return 0;
    if (SceneActorBehavior_OverrideBits(4, &gUnk_02000560))
        return 0;
    if (!SceneActorBehavior_OverrideBits(2, &p->base.behaviorFlags)) {
        if (!sameRegion(&p->base.regionId)) {
            SceneActorPlacement_HideActor(p, 0);
            return 0;
        }
        if (!SceneActor_ActionsBlocked(p))
            Callback_InvokeActor(p, p->base.onDamage);
        SceneActorPlacement_ConsumeCallbackFlag(p);
        SceneActorMovement_UpdateStatus(p);
        dispatch(&p->base.onUpdate, p->base.context);
        SceneActorMovement_RequestAvailableAction(p);
        SceneActorMovement_UpdateAnimationAndTerrain(p);
        SceneActorPlacement_ConsumeShakeFlags(p);
        SceneActorTarget_OnBindingChanged(p);
        return 1;
    } else {
        if (!(p->base.behaviorFlags & 1) && sameRegion(&p->base.regionId))
            SceneActorPlacement_ShowActor(p, 0);
        SceneActorTarget_OnBindingChanged(p);
    }
    return 0;
}
