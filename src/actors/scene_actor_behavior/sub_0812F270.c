#include "engine/shared/global.h"
#include "engine/shared/scene_actor_registry.h"
#include "system/game_state/game_state.h"
#include "engine/shared/ram.h"
#include "engine/shared/scene_actor_availability.h"
#include "actors/scene_actor_behavior/scene_actor_behavior.h"
extern u32 gUnk_02000560;
void SceneActorPlacement_HideActor(struct ChaseActor *, s32);
void SceneActorPlacement_ShowActor(struct ChaseActor *, s32);
void SceneActorPlacement_ConsumeShakeFlags(struct ChaseActor *);
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
void SceneActorBehavior_ActivatePendingControl(struct ChaseActor *);
s32 SceneActorBehavior_DispatchPendingFlags(struct ChaseActor *);
s32 SceneActorBehavior_UpdateTimersAndContacts(struct ChaseActor *);
void SceneActorMotion_Update(struct ChaseActor *);
s32 SceneActor_UpdateStatus(struct ChaseActor *);
s32 SceneActor_UpdateBehaviorCallbacks(struct ChaseActor *p)
{
    struct SceneActorRegistryControlGroup *block = &p->group.common;
    if (SceneActorBehavior_GlobalPauseBits(0x400))
        return 0;
    if (SceneActorBehavior_OverrideBits(4, &gUnk_02000560))
        return 0;
    if (!SceneActorBehavior_OverrideBits(2, &p->base.behaviorFlags)) {
        if (!sameRegion(&p->base.regionId)) {
            SceneActorPlacement_HideActor(p, 0);
            return 0;
        }
        if (!SceneActor_ActionsBlocked(p)) {
            if (!(u8)Callback_InvokeActor(p, p->base.onDamage))
                Callback_InvokeActor(p, block->fallback);
        }
        SceneActorBehavior_ActivatePendingControl(p);
        SceneActorBehavior_DispatchPendingFlags(p);
        SceneActor_UpdateStatus(p);
        dispatch(&p->base.onUpdate, p->base.context);
        SceneActorBehavior_UpdateTimersAndContacts(p);
        SceneActorMotion_Update(p);
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
