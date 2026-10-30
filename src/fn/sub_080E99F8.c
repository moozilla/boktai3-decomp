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
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *target)
{
    void *callback = target;
    u32 type = kind;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = type;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void HideCollider(struct SceneActorRegistryResetView *actor)
{
    struct ContactCollider *collider = &actor->colliderA;
    u32 f = collider->flags;
    u32 m = 4;
    m |= f;
    m &= 0xFFF7;
    collider->flags = m;
    {
        u32 m = 0x800;
        actor->flags2C0 = m | actor->flags2C0;
    }
}
static inline void ShowCollider(struct SceneActorRegistryResetView *actor)
{
    actor->colliderA.flags = (actor->colliderA.flags & 0xFFFB) | 8;
    {
        u32 m = ~0x800;
        actor->flags2C0 = m & actor->flags2C0;
    }
}

// confidence=low: applies a contact effect to the held player and waits before release.
void Ivy_ApplyHeldTargetDamage(struct SceneActorRegistryResetView *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        struct DamageTarget *target = (void *)actor->tracking.target;
        if (target->kind == 4) {
            SetControl(actor, 16, SceneActorControl_OnIdle);
            SetState(actor, 0, 0, 1);
            return;
        }
        if (target->kind <= 3) {
            u8 *player = target->owner;
            u32 a3, a4, a5, a6;
            SpriteParticle_SpawnBatch(&actor->body.position, 1, 2, 0, 0x154);
            a3 = *(u16 *)((u8 *)actor + 0xB4);
            a4 = *(u16 *)((u8 *)actor + 0xBC);
            a5 = *(u32 *)((u8 *)actor + 0xCC);
            a6 = *(u16 *)((u8 *)actor + 0xC4);
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
