// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
struct VornApproachContext {
    u8 unknown0[0x6C8];
    u32 minimumDistance, maximumDistance;
    u8 unknown6D0[4];
    u16 flags;
};
/* Vorn_UpdateApproachRange: tentative */
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
void Vorn_SelectRetreatRoute(struct SceneActorAnimationView *, u32);
s32 Callback_InvokeActor(void *, void *);
static inline void ClearFlags(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void ClearFlags16(u16 *p, u32 mask)
{
    *p &= mask;
}
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    u32 offset = 0x2B2;
    u8 *field;
    field = (u8 *)actor + offset;
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}
static inline u8 TryMove(struct SceneActorRegistryResetView *actor, u8 *group)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = &actor->body.position;
    s32 result;
    register u16 *fp asm("r0");
    struct ContactPosition *destination;
    u32 base = actor->tracking.target;
    destination = (void *)(base + 12);
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        u32 o1 = 0x2D4;
        fp = (u16 *)((u8 *)actor + o1);
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            register u32 o2 asm("r2") = 0x2D4;
            asm("" : "+r"(o2));
            fp = (u16 *)((u8 *)actor + o2);
        }
    }
    {
        u32 m = ~0x200;
        m &= *fp;
        *fp = m;
    }
    return 0;
}
void Vorn_UpdateApproachRange(struct SceneActorRegistryResetView *actor)
{
    struct VornApproachContext *context = actor->context;
    u8 *group;
    {
        register u32 go asm("r2") = 0x3D4;
        asm("" : "+r"(go));
        group = (u8 *)actor + go;
    }
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    else if (actor->tracking.distance > context->maximumDistance ||
             actor->tracking.distance < context->minimumDistance) {
        Vorn_SelectRetreatRoute((void *)actor, 1);
    } else if (SceneActor_IsFacingNearbyTarget(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (!TryMove(actor, group))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
        else {
            u32 next;
            u8 zero;
            register u32 zero2 asm("r4");
            u16 *flags = &context->flags;
            u32 mask = ~0x40;
            mask &= *flags;
            zero = 0;
            zero2 = 0;
            *flags = mask;
            next = 21;
            {
                register u32 offset asm("r2") = 0x2A8;
                *((u8 *)actor + offset) = zero;
                offset += 1;
                *((u8 *)actor + offset) = zero;
                offset += 1;
                *((u8 *)actor + offset) = next;
                actor->counter2AC = zero2;
                offset += 7;
                {
                    register u8 *q asm("r1");
                    q = (u8 *)actor + offset;
                    *q = 1;
                }
            }
        }
    }
}
