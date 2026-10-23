#include "scene_actor_placement.h"

// confidence=low: resumes the actor drawable and shadow, then restores its reset control.
#define SceneActorPlacement_ResumeResetControl sub_08136D40

void SceneActorTerrain_ResumeDrawable(void *);
void ActorShadow_Enable(void *);

static inline void ClearCollider(struct ContactCollider *collider)
{
    collider->flags = 0xFFFB & collider->flags;
}
static inline void SetState(struct SceneActorPlacementView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    u32 b = behavior;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorPlacementView *actor, u32 kind, const void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->control = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->field104 = 0;
}
void SceneActorPlacement_ResumeResetControl(struct SceneActorPlacementView *actor)
{
    u32 bit = 1;
    if (actor->flags2CC & bit) {
        SceneActorTerrain_ResumeDrawable(actor);
        if (actor->shadow320[0])
            ActorShadow_Enable(actor->shadow320);
        ClearCollider(&((struct SceneActorRegistryResetView *)actor)->colliderA);
        SceneActorRegistry_ResetRouteAndColliders((void *)actor);
        SetState(actor, 3, 0, 0);
        {
            const void *control = actor->resetControl220;
            SetControl(actor, 12, control);
        }
    }
}
