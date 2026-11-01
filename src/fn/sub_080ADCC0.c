// confidence=low: tentative Vorn behavior inferred from context and neighboring callbacks.
#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_terrain.h"
// confidence=low: tentative Vorn behavior role.
// confidence=low: target distance controls approach, idle animation, and a delayed action.
void Vorn_BroadcastPosition(void *);
void sub_080B1FA8(void *);
struct VornPursuitContext {
    u8 unknown0[0x6A8];
    u16 nearTimer, farTimer;
    u8 unknown6AC[0x28];
    s16 flags;
};
static inline u8 Vorn_TestNear(struct VornPursuitContext *p, u32 mask, u16 **out)
{
    u16 *field = (u16 *)&p->flags;
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}
void Vorn_UpdatePursuitDistance(struct SceneActorRegistryResetView *actor)
{
    struct VornPursuitContext *context = actor->context;
    u8 *group = (u8 *)context + 0x3D4;
    struct ContactPosition center;
    u16 *flags;
    u8 near;
    if (SceneActorReaction_TakePrepared(actor))
        return;
    Vorn_BroadcastPosition(actor);
    if (actor->tracking.distance < 0x38400) {
        near = Vorn_TestNear(context, 2, &flags);
        if (near) {
            if (context->nearTimer > 40) {
                sub_080B1FA8(actor);
                Chase_SetBehavior(actor, 15);
                {
                    s32 mask = ~2;
                    mask &= *flags;
                    *flags = mask;
                }
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                context->nearTimer = 0;
            } else {
                context->nearTimer++;
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                actor->body.direction = actor->tracking.heading;
            }
        } else {
            if (actor->tracking.distance < 0x19000) {
                if (context->nearTimer > 30) {
                    sub_080B1FA8(actor);
                    Chase_SetBehavior(actor, 15);
                    {
                        s32 mask = ~2;
                        mask &= *flags;
                        *flags = mask;
                    }
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                    context->nearTimer = 0;
                    return;
                }
                context->nearTimer++;
            }
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        }
    } else if (actor->tracking.distance > 0x90000) {
        near = Vorn_TestNear(context, 2, &flags);
        if (!near) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
            context->farTimer = 0;
        } else {
            if (context->farTimer <= 59) {
                actor->body.direction = actor->tracking.heading;
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                context->farTimer++;
            } else {
                if (!actor->tracking.target)
                    return;
                {
                    struct ChaseTarget *target = (void *)actor->tracking.target;
                    SceneActorTerrain_GetCenter(
                        Chase_CellIndex(target->position.x >> 8, target->position.z >> 8), &center);
                }
                if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                                    &center)) {
                    Callback_InvokeActor(actor, *(void **)(group + 0x224));
                    return;
                }
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                Chase_SetBehavior(actor, 15);
                {
                    s32 mask = ~2;
                    mask &= *flags;
                    *flags = mask;
                }
                context->farTimer = 0;
            }
        }
    } else {
        near = Vorn_TestNear(context, 2, &flags);
        if (near) {
            actor->body.direction = actor->tracking.heading;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (context->farTimer > 60) {
                if (!actor->tracking.target)
                    return;
                {
                    struct ChaseTarget *target = (void *)actor->tracking.target;
                    SceneActorTerrain_GetCenter(
                        Chase_CellIndex(target->position.x >> 8, target->position.z >> 8), &center);
                }
                if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                                    &center)) {
                    Callback_InvokeActor(actor, *(void **)(group + 0x224));
                    return;
                }
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                Chase_SetBehavior(actor, 15);
                {
                    s32 mask = ~2;
                    mask &= *flags;
                    *flags = mask;
                }
                context->farTimer = 0;
            } else
                context->farTimer++;
        } else
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    }
}
