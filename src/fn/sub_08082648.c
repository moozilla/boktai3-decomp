#include "global.h"
// confidence=low: contact healing updates health feedback and selects a recovery phase.
#define SceneActorControl_ApplyRecoveryContact sub_08082648
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
#include "engine.h"
#include "scene_actor_damage_visibility.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void Collision_CalculateDamageResponse(void *, void *);
void SceneActorControl_ResetCollisionPhase(void *);
struct RecoveryHealthView {
    u8 unknown0[0x9A];
    s16 parameter, health;
    u8 unknown9E[0x12];
    u16 maximum;
    s16 kind;
};
struct RecoveryContactIdentity {
    u32 unknown0;
    u16 identity;
};
struct RecoveryContextView {
    u8 unknown0[0x3D0];
    u32 controlAddress;
};
static inline struct SceneActorControlState *LocalState(struct SceneActorRegistryResetView *a)
{
    return (struct SceneActorControlState *)((struct RecoveryContextView *)a)->controlAddress;
}
static inline u32 *ControlFlags(struct SceneActorControlState *state)
{
    return &state->flags3D4;
}
static inline u8 TestFlags(struct SceneActorControlState *state, u32 mask)
{
    if (*ControlFlags(state) & mask)
        return 1;
    return 0;
}
static inline void Set32(struct SceneActorControlState *state, u32 mask)
{
    *ControlFlags(state) |= mask;
}
static inline void Clear32(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline u8 TestMotion(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}
static inline void SetMotion(struct SceneActorReactionMotion *motion, u32 mask)
{
    u32 value = mask;
    value |= motion->flags;
    motion->flags = value;
}
static inline u32 Flag16(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->base.flags2D4 & mask;
}
static inline u32 Flag32(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->base.flags2D0 & mask;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Flag16(a, 0x40) || Flag32(a, 0x20000) || a->base.health <= 0)
            return 1;
    }
    return 0;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *callback, u32 kind,
                           u32 zero)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = zero;
    a->base.field2B5 = kind;
    a->base.target = callback;
    a->base.field2C8 = zero;
    {
        u32 *p = &a->base.flags114;
        s32 mask = ~1;
        *p &= mask;
    }
    a->base.word104 = zero;
}
static inline void PhaseStart(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
static inline void SetStatus(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->base.flags2D4 = mask | a->base.flags2D4;
}
static inline void Phase(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior,
                         u32 counter)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = counter;
    a->base.field2B1 = 1;
}
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

void SceneActorControl_ApplyRecoveryContact(struct SceneActorBehaviorContactView *source,
                                            struct SceneActorBehaviorContactView *inputHit,
                                            struct SceneActorRegistryResetView *input)
{
    union {
        struct SceneActorBehaviorContactView *collider;
        u32 amount;
    } contact;
    struct SceneActorRegistryResetView *a;
    u32 zero;
    struct SceneActorReactionMotion *motion;
    contact.collider = inputHit;
    if (contact.collider->kind != 1)
        return;
    a = input;
    if (Unavailable(a))
        return;
    if ((!Unavailable(input)) && (contact.collider->kind & 15)) {
        input->base.field304 = ((struct RecoveryContactIdentity *)contact.collider)->identity;
        Clear32(&input->base.flags2D0, ~0x1000000);
    }
    Collision_CalculateDamageResponse(source, contact.collider);
    {
        u32 type = a->base.paletteOffset;
        if (((type == 1) || ((type - 12) <= 3)) && contact.collider->remaining) {
            s16 *healthPtr = &a->base.health;
            union {
                s32 health;
                struct ContactCollider *collider;
            } recovery;
            u16 amount;
            s32 healed;
            u16 *maximum;
            recovery.health = *healthPtr;
            amount = contact.collider->remaining;
            healed = recovery.health + amount;
            maximum = (u16 *)&a->base.defaultHealth;

            if (healed > (*maximum)) {
                struct ContactCollider *local = &a->base.colliderA;
                local->cost = *maximum - recovery.health;
                {
                    s32 cap = *((s16 *)maximum);
                    recovery.collider = local;
                    if (cap <= 0)
                        cap = 0;
                    *healthPtr = cap;
                }
            } else {
                struct ContactCollider *local = &a->base.colliderA;
                local->cost = amount;
                {
                    u16 *output = (u16 *)healthPtr;
                    u32 oldHealth = *output;
                    contact.amount = contact.collider->remaining;
                    *output = oldHealth + contact.amount;
                }
                recovery.collider = local;
            }
            a->base.reactionSoundVariant = recovery.collider->cost;
            if (a->base.reactionSoundVariant && SceneActorDamage_ShouldShow(a)) {
                u32 zero;
                u32 buffer[2];
                s32 hp = a->base.health;
                u32 max = (u16)a->base.defaultHealth;
                s32 kind = ((struct RecoveryHealthView *)a)->kind;
                s32 param = ((struct RecoveryHealthView *)a)->parameter;
                SceneHud_SetTimedState(a->base.body.id, hp, max, kind, 0, param);
                Fill(&zero, buffer);
                if (a->base.reactionSoundVariant || (!(a->base.field2D8 & 0x20000)))
                    DamageNumberDisplay_Show(recovery.collider, a->base.reactionSoundVariant, 1,
                                             &a->base.body.position, buffer, 1);
            }
        }
    }
    {
        struct ContactCollider *c = &a->base.colliderB;
        c->flags |= 4;
    }
    {
        struct SceneActorControlState *control = LocalState(a);
        Clear32(ControlFlags(control), ~8);
        zero = TestFlags(LocalState(a), 4);
        if (zero)
            return;
    }
    Control(a, (void *)SceneActorControl_OnIdle, 16, zero);
    motion = (void *)&a->base.tracking.target;
    zero = TestMotion(motion, 8);
    if (zero) {
        PhaseStart(a, 2, 1);
        Set32((struct SceneActorControlState *)((struct RecoveryContextView *)a)->controlAddress,
              4);
    } else {
        SetMotion(motion, 8);
        SetStatus(a, 0x100);
        Phase(a, 0, 3, zero);
        SceneActorControl_ResetCollisionPhase(a);
    }
}
