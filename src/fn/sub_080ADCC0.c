// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
#ifdef NONMATCHING
// fuzzy: 85.7%
// Remaining differences: inlined cell-centre temporaries, branch layout, and saved registers.
// confidence=low: target distance controls approach, idle animation, and a delayed action.
void Vorn_BroadcastPosition(void *);
void sub_080B1FA8(void *);
struct VornPursuitContext {
    u8 unknown0[0x6A8];
    u16 nearTimer, farTimer;
    u8 unknown6AC[0x28];
    s16 flags;
};
static inline u8 Vorn_TestNear(struct VornPursuitContext *p, u32 mask)
{
    if ((u16)p->flags & mask)
        return 1;
    return 0;
}
static inline void Vorn_ApproachTarget(struct ContactPosition *center,
                                       struct SceneActorRegistryResetView *actor, u8 *group)
{
    struct ChaseTarget *target = (void *)actor->fieldDC;
    s32 cell = Chase_CellIndex(target->position.x >> 8, target->position.z >> 8);
    register struct ContactPosition *p asm("sl") = center;
    s32 x = Mod(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    s32 z = Div(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    u8 *data;
    p->x = (x << 8) + 128;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    p->y = (*data & 15) << 8;
    p->z = (z << 8) + 128;
}
void Vorn_UpdatePursuitDistance(struct SceneActorRegistryResetView *actor)
{
    register struct VornPursuitContext *context asm("r7") = actor->context;
    u8 *group = (u8 *)context + 0x3D4;
    struct ContactPosition center;
    u16 *flags;
    u8 near;
    if (SceneActorReaction_TakePrepared(actor))
        return;
    Vorn_BroadcastPosition(actor);
    if (actor->distanceF0 < 0x38400) {
        near = Vorn_TestNear(context, 2);
        flags = (u16 *)&context->flags;
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
                actor->direction = actor->fieldE9;
            }
        } else {
            if (actor->distanceF0 < 0x19000) {
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
    } else if (actor->distanceF0 > 0x90000) {
        near = Vorn_TestNear(context, 2);
        flags = (u16 *)&context->flags;
        if (!near) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
            context->farTimer = 0;
        } else {
            if (context->farTimer <= 59) {
                actor->direction = actor->fieldE9;
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                context->farTimer++;
            } else {
                if (!actor->fieldDC)
                    return;
                Vorn_ApproachTarget(&center, actor, group);
                if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, &center))
                    goto fail;
                goto arrived;
            }
        }
    } else {
        near = Vorn_TestNear(context, 2);
        flags = (u16 *)&context->flags;
        if (near) {
            actor->direction = actor->fieldE9;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (context->farTimer > 60) {
                if (!actor->fieldDC)
                    return;
                Vorn_ApproachTarget(&center, actor, group);
                if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, &center)) {
                fail:
                    Callback_InvokeActor(actor, *(void **)(group + 0x224));
                    return;
                }
            arrived:
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

#else
INCLUDE_ASM("asm/nonmatching", Vorn_UpdatePursuitDistance);
#endif
