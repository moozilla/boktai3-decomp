#include "global.h"
#include "scene_actor_damage_visibility.h"
#include "scene_actor_reaction.h"
#include "scene_actor_palette.h"
#include "area_effect_link.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.flags2D0 & mask;
}
static inline void Reaction_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void Hide(struct ChaseActor *a)
{
    if (!a->base.drawableKind) {
        u32 *flags = a->base.drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->base.drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}

void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);

struct FlagHalfword {
    s16 bits : 16;
};
static inline void SetHitFlags(s16 bit, struct FlagHalfword *p)
{
    p->bits = bit | p->bits;
}
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
s32 AreaEffect_ScaleGainAtPosition(void *, u32);
static inline u32 NextRandomSample(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((u32)table + index * 2);
}
static inline u32 ActorStatusFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2D0 & mask;
}
static inline u32 GetReactionFlags(u32 **out, struct ChaseActor *actor, u32 mask)
{
    u32 *p = &actor->base.flags2CC;
    u32 result = *p & mask;
    *out = p;
    return result;
}
static inline struct ContactCollider *GetDamageCollider(struct ChaseActor *actor)
{
    return &actor->base.colliderA;
}
// Area-effect damage with a random knockback heading.
u8 SceneActorReaction_ApplyAreaDamage(struct ChaseActor *actor)
{
    s32 dx;
    struct ContactPosition *position;
    struct ContactCollider *displayCollider;
    s16 *health;
    u16 *timer;
    u16 *maxHealth;
    u32 *flags2CC;
    struct ContactCollider *collider;
    u32 *flags2D8;
    s32 gain;
    s32 dz;
    if ((ActorStatusFlags(actor, 8) || (actor->base.enabled != 1)) ||
        GetReactionFlags(&flags2CC, actor, 2))
        goto fail;
    collider = GetDamageCollider(actor);
    {
        u16 *p = &actor->base.colliderA.responseValue;
        u32 value = *p;
        displayCollider = collider;
        timer = p;
        if (value)
            goto fail;
    }
    {
        struct ContactPosition *pos = &actor->base.body.position;
        u16 *max = (u16 *)(&actor->base.defaultHealth);
        gain = AreaEffect_ScaleGainAtPosition(pos, *max);
        position = pos;
        maxHealth = max;
    }
    if (!gain)
        goto fail;
    dx = (NextRandomSample() & 0x3F) - 16;
    dz = (NextRandomSample() & 0x3F) - 16;
    collider->cost = gain;
    actor->base.reactionSoundVariant = gain;
    {
        u32 damage = collider->cost;
        s16 *h = &actor->base.health;
        u32 *f;
        u32 result;
        *h -= damage;
        f = &actor->base.field2D8;
        result = *f & 0x20000;
        health = h;
        flags2D8 = f;
        if (!result) {
            u16 bank = actor->base.paletteAlternateBank;
            if (actor->base.drawableKind == 0)
                AnimData_SelectPaletteBank(((u8 *)actor->base.drawable) + 0x2C, bank);
            else
                ((struct SceneActorPaletteTileView *)actor->base.drawable)->palette =
                    (const u16 *)(gPaletteBankData + bank * 32);
            {
                u32 eight = 8;
                actor->base.field1E8 = eight;
            }
            {
                u32 bit = 0x8000;
                *flags2CC |= bit;
            }
            if (SceneActorDamage_ShouldShow(actor)) {
                u32 zero;
                u32 buffer[2];
                s32 h = *health;
                u32 max = *maxHealth;
                s32 kind = (s16)actor->base.fieldB2;
                s32 param = (s16)actor->base.field9A;
                SceneHud_SetTimedState(actor->base.body.id, h, max, kind, 0, param);
                Fill(&zero, buffer);
                if (actor->base.reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                    DamageNumberDisplay_Show(displayCollider, actor->base.reactionSoundVariant, 0,
                                             position, buffer, 1);
            }
        }
    }
    SceneActorMovement_ProjectProbe(actor, dx, dz);
    SetHitFlags(8, (void *)&actor->base.flags2D4);
    actor->base.body.direction = Math_Atan2(dx, dz);
    *timer = 38;
    actor->base.field2DC = 0x40000;
    actor->base.field2D8 = 0;
    actor->base.field2B8 = 0;
    if ((*health) <= 0) {
        collider->responseValue = 0;
        Hide(actor);
        {
            void *control = actor->base.deathControl;
            Reaction_SetControl(actor, 17, control);
        }
        {
            u32 state = 3;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = 0;
            actor->base.behavior2AA = 0;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
        }
        actor->base.field2E0 = 32;
        if ((!Test(gEngineFlags, 0x4800)) && (!Flag2D0(actor, 0x800)))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    } else {
        SetHitFlags(0x20, (void *)&actor->base.flags2D4);
        {
            void *control = actor->base.reactionTarget230;
            Reaction_SetControl(actor, 0x12, control);
        }
        {
            u32 state = 3;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = 0;
            actor->base.behavior2AA = 0;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
        }
    }
    {
        u32 mask = 0x2000;
        *flags2CC |= mask;
    }
    return 1;
fail:
    return 0;
}
