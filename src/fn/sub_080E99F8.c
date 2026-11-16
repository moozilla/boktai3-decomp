#include "global.h"
#include "scene_actor_reaction.h"
struct DamageTarget {
    u8 pad[8];
    u8 kind;
    u8 rest[0x33];
    void *owner;
};
void SpriteParticle_SpawnBatch(void *, s32, s32, s32, s32);
void SpriteEffect_SpawnTowardObject(void *, void *, u32, u32, u32, u32);
static inline void SetControl(struct ChaseActor *actor, u32 kind, void *target)
{
    void *callback = target;
    u32 type = kind;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = type;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void SetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void HideCollider(struct ChaseActor *actor)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    u32 f = collider->flags;
    u32 m = 4;
    m |= f;
    m &= 0xFFF7;
    collider->flags = m;
    {
        u32 m = 0x800;
        actor->base.placementFlags = m | actor->base.placementFlags;
    }
}
static inline void ShowCollider(struct ChaseActor *actor)
{
    actor->base.colliderA.flags = (actor->base.colliderA.flags & 0xFFFB) | 8;
    {
        u32 m = ~0x800;
        actor->base.placementFlags = m & actor->base.placementFlags;
    }
}

// confidence=low: applies a contact effect to the held player and waits before release.
void Ivy_ApplyHeldTargetDamage(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        struct DamageTarget *target = (void *)actor->base.tracking.target;
        if (target->kind == 4) {
            SetControl(actor, 16, SceneActorControl_OnIdle);
            SetState(actor, 0, 0, 1);
            return;
        }
        if (target->kind <= 3) {
            u8 *player = target->owner;
            u32 a3, a4, a5, a6;
            SpriteParticle_SpawnBatch(&actor->base.body.position, 1, 2, 0, 0x154);
            a3 = actor->base.attacks.first[0];
            a4 = actor->base.attacks.second[0];
            a5 = actor->base.attacks.flags[0];
            a6 = actor->base.attacks.parameter[0];
            SpriteEffect_SpawnTowardObject((u8 *)actor + 0x50, player + 0x24, a3, a4, a5, a6);
            Sound_PlaySong(0x1E0);
        }
        HideCollider(actor);
    }
    if (frame > 60) {
        SetControl(actor, 16, SceneActorControl_OnIdle);
        SetState(actor, 0, 0, 4);
    }
}
