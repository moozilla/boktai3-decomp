#include "vorn.h"
#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalEntry(struct ChaseActor *, u32);
u8 Vorn_BeginContactAnimation(struct ChaseActor *);
void Vorn_BeginPursuitRoute(struct ChaseActor *, u32);
void Vorn_SelectRetreatRoute(struct ChaseActor *, u32);
static inline u8 Test(struct Vorn *context, u32 mask)
{
    if (context->flags6D4 & mask)
        return 1;
    return 0;
}
// confidence=low: pending hit flags select recovery control or pursuit/retreat behavior.
u8 Vorn_SelectHitRecovery(struct ChaseActor *actor)
{
    struct Vorn *context = actor->base.context;
    u32 blocked;
    {
        u16 *flags = &context->flags6D4;
        s32 mask = ~8;
        *flags = *flags & mask;
    }
    if (!(actor->base.hitAttributes & 0x20000)) {
        if (actor->base.reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x1CB);
    }
    {
        u32 mask = 0x8002;
        blocked = mask & actor->base.motionFlags;
    }
    if (blocked) {
        {
            u32 mask = ~0x20;
            mask &= actor->base.motionFlags;
            actor->base.motionFlags = mask;
        }
        return 0;
    }
    if (actor->base.hitReactionFlags & 0xC0000) {
        void *control = actor->base.onReaction;
        u32 kind = 0x12;
        u8 zero8;
        actor->base.field2B2 = 1;
        actor->base.stepDone = blocked;
        actor->base.controlKind = kind;
        actor->base.target = control;
        zero8 = 0;
        actor->base.field2C8 = blocked;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->base.pendingControl = blocked;
        {
            u32 state = 3;
            actor->base.state2A8 = state;
        }
        actor->base.phase2A9 = zero8;
        actor->base.behavior2AA = zero8;
        actor->base.counter2AC = blocked;
        actor->base.field2B1 = 1;
        {
            u32 *flags = &actor->base.statusFlags;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        return 1;
    }
    {
        u32 mask = ~0x20;
        actor->base.motionFlags &= mask;
    }
    if (actor->base.motionFlags & 0x100)
        return 0;
    if (actor->base.hitLockTimer)
        return 0;
    if (Test(context, 0x1D))
        return Vorn_BeginContactAnimation((void *)context);
    if (actor->group.hitReactionLatch == 0 && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 45;
        actor->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    if (!actor->base.tracking.target)
        return 0;
    {
        u32 foreign = context->foreignPosition;
        if (foreign == 0) {
            context->pursuitCooldown = foreign;
            Vorn_BeginPursuitRoute(actor, 1);
        } else {
            Vorn_SelectRetreatRoute(actor, 1);
        }
        return 1;
    }
}
