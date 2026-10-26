#include "global.h"
#ifdef NONMATCHING
// fuzzy: 65.3%
// fix.py best (sweep after the inlined-insn fix)
// MATCHING_CONCESSION: register-pinning
// confidence=low: contact healing updates health feedback and selects a recovery phase.
#define SceneActorControl_ApplyRecoveryContact sub_08082648
// Open: saturating-health temporaries, inline flag tests, and phase-reset pointer lifetimes.
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
#include "engine.h"
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
static inline u32 Flag16(u16 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 Flag32(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Flag16(&a->flags2D4, 0x40) || Flag32(&a->flags2D0, 0x20000) || a->health <= 0)
            return 1;
    }
    return 0;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *callback, u32 kind,
                           u32 zero)
{
    a->field2B2 = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = zero;
    {
        u32 *p = &a->flags114;
        s32 mask = ~1;
        *p &= mask;
    }
    a->word104 = zero;
}
static inline void Phase(struct SceneActorRegistryResetView *a, u32 state, u32 phase, u32 behavior,
                         u32 counter)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = counter;
    a->field2B1 = 1;
}
static inline s32 LocalPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        register u32 mask asm("r2") = *owner;
        if (!(mask & 0x20) && mask) {
            switch (LocalPlayer()) {
                case 0:
                    if (*owner != 1)
                        return 0;
                    break;
                case 1:
                    if (*owner != 2)
                        return 0;
                    break;
                case 2:
                    if (*owner != 4)
                        return 0;
                    break;
                case 3:
                    if (*owner != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

struct FlagHalfword {
    s16 bits : 16;
};
static inline void ClearHit(struct FlagHalfword *flags, u32 mask)
{
    flags->bits = mask & flags->bits;
}
static inline void SetHit(u16 *p, u32 bit)
{
    *p = bit | *p;
}
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

void SceneActorControl_ApplyRecoveryContact(struct SceneActorBehaviorContactView *source,
                                            struct SceneActorBehaviorContactView *hit,
                                            struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *a;
    u32 zero;
    if (hit->kind != 1)
        return;
    a = input;
    if (Unavailable(a))
        return;
    if ((!Unavailable(input)) && (hit->kind & 15)) {
        input->field304 = *((u16 *)(((u8 *)hit) + 4));
        input->flags2D0 &= ~0x1000000;
    }
    Collision_CalculateDamageResponse(source, hit);
    {
        u32 type = *((u16 *)(((u8 *)a) + 0x1e6));
        if (((type == 1) || ((type - 12) <= 3)) && hit->remaining) {
            s32 health = a->health;
            u16 amount = hit->remaining;
            u16 *maximum = (void *)(((u8 *)a) + 0xb0);
            struct ContactCollider *collider;
            if ((health + amount) > (*maximum)) {
                collider = &a->colliderA;
                *((u16 *)(((u8 *)collider) + 0x3e)) = (*maximum) - health;
                {
                    s32 cap = *((s16 *)maximum);
                    if (cap <= 0)
                        cap = 0;
                    a->health = cap;
                }
            } else {
                collider = &a->colliderA;
                *((u16 *)(((u8 *)collider) + 0x3e)) = amount;
                a->health += hit->remaining;
            }
            a->reactionSoundVariant = *((u16 *)(((u8 *)collider) + 0x3e));
            if (a->reactionSoundVariant && ShouldShowDamage(a)) {
                u32 zero;
                u32 buffer[2];
                s32 hp = a->health;
                u32 max = *maximum;
                s32 kind = *((s16 *)(((u8 *)a) + 0xb2));
                s32 param = *((s16 *)(((u8 *)a) + 0x9a));
                SceneHud_SetTimedState(a->id, hp, max, kind, 0, param);
                Fill(&zero, buffer);
                if (a->reactionSoundVariant || (!(a->field2D8 & 0x20000)))
                    DamageNumberDisplay_Show(collider, a->reactionSoundVariant, 1, &a->position,
                                             buffer, 1);
            }
        }
    }
    {
        u16 *p = (void *)(((u8 *)(&a->colliderB)) + 6);
        SetHit(p, 4);
    }
    {
        struct SceneActorControlState *control = SceneActorControl_GetState(a);
        control->flags3D4 &= ~8;
        zero = Test(SceneActorControl_GetState(a)->flags3D4, 4);
        if (zero)
            return;
    }
    Control(a, (void *)SceneActorControl_OnIdle, 16, zero);
    zero = Test(a->flagsEB, 8);
    if (zero) {
        Phase(a, 0, 2, 1, 0);
        SceneActorControl_GetState(a)->flags3D4 |= 4;
    } else {
        u32 bit = 8;
        a->flagsEB = bit | a->flagsEB;
        {
            u16 *p = &a->flags2D4;
            *p = 0x100 | (*p);
        }
        Phase(a, 0, 0, 3, zero);
        SceneActorControl_ResetCollisionPhase(a);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08082648);
#endif
