#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorApproachContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 minimumDistance, maximumDistance;
};
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
static inline void ClearFlags(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void ClearFlags16(u16 *p, u32 mask)
{
    *p &= mask;
}
static inline u8 TryMove(struct SceneActorRegistryResetView *actor, u8 *group)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = &actor->body.position;
    struct ContactPosition *destination = &actor->tracking.target->position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        ClearFlags16(&actor->flags2D4, ~0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            ClearFlags16(&actor->flags2D4, ~0x200);
            return 0;
        }
    }
}
void SceneActorApproach_UpdateRange(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachContext *context = actor->context;
    u8 *group = (void *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    else if (actor->tracking.distance > context->maximumDistance ||
             actor->tracking.distance < context->minimumDistance) {
        ClearFlags(&context->flags, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else if (SceneActorReaction_IsRangeReady(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (!TryMove(actor, group))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
        else {
            u32 next;
            ClearFlags(&context->flags, ~0x100);
            next = 15;
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = next;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
    }
}
