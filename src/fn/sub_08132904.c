#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_damage_visibility.h"
#include "scene_actor_damage_flash.h"
#include "scene_actor_drawable_flags.h"
#include "scene_actor_reaction.h"
#include "scene_actor_palette.h"
#include "area_effect_link.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}

void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);

struct FlagHalfword {
    s16 bits : 16;
};
static inline void ClearHitFlags(struct FlagHalfword *p, u32 mask)
{
    p->bits = mask & p->bits;
}
static inline void SetHitFlags(s16 bit, struct FlagHalfword *p)
{
    p->bits = bit | p->bits;
}
s32 AreaEffect_ScaleGainAtPosition(void *, u32);
s32 AreaEffect_ScaleLinkGainAtPosition(void *, u32);
static inline u32 NextRandomSample(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((u32)table + index * 2);
}
static inline u32 ActorStatusFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.statusFlags & mask;
}
static inline u32 GetReactionFlags(u32 **out, struct ChaseActor *actor, u32 mask)
{
    u32 *p = &actor->base.behaviorFlags;
    u32 result = *p & mask;
    *out = p;
    return result;
}
static inline struct ContactCollider *GetDamageCollider(struct ChaseActor *actor)
{
    return &actor->base.colliderA;
}
// Area-effect damage with a random knockback heading.
u8 SceneActorReaction_ApplyLinkedAreaDamage(struct ChaseActor *actor)
{
    s32 dx;
    s32 dz;
    s16 *health;
    struct ContactCollider *displayCollider;
    struct ContactPosition *position;
    u16 *timer;
    u16 *maxHealth;
    u32 *flags2CC;
    struct ContactCollider *collider;
    u32 *flags2D8;
    s32 gain;
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
        struct ContactPosition *pos;
        u16 *max;
        if (SceneActorDamage_TestFlags(gEngineFlags, 0x4800)) {
            pos = &actor->base.body.position;
            max = (u16 *)&actor->base.defaultHealth;
            gain = AreaEffect_ScaleLinkGainAtPosition(pos, *max);
            position = pos;
            maxHealth = max;
        } else {
            pos = &actor->base.body.position;
            max = (u16 *)&actor->base.defaultHealth;
            gain = AreaEffect_ScaleGainAtPosition(pos, *max);
            position = pos;
            maxHealth = max;
        }
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
        f = &actor->base.hitAttributes;
        result = *f & 0x20000;
        health = h;
        flags2D8 = f;
        if (!result) {
            SceneActorDamage_Flash(actor);
            SceneActorDamage_MarkFlashing(flags2CC);
            if (SceneActorDamage_ShouldShow(actor)) {
                struct ContactPosition offset;
                s32 h = *health;
                u32 max = *maxHealth;
                s32 kind = (s16)actor->base.hudKind;
                s32 param = (s16)actor->base.hudLabel;
                SceneHud_SetTimedState(actor->base.body.id, h, max, kind, 0, param);
                ZeroFill8(&offset);
                if (actor->base.reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                    DamageNumberDisplay_Show(displayCollider, actor->base.reactionSoundVariant, 0,
                                             position, &offset, 1);
            }
        }
    }
    SceneActorMovement_ProjectProbe(actor, dx, dz);
    SetHitFlags(8, (void *)&actor->base.motionFlags);
    actor->base.body.direction = Math_Atan2(dx, dz);
    *timer = 38;
    actor->base.hitReactionFlags = 0x40000;
    actor->base.hitAttributes = 0;
    ClearHitFlags((void *)&actor->base.motionFlags, ~0x2000);
    actor->base.hitLockTimer = 0;
    if ((*health) <= 0) {
        collider->responseValue = 0;
        ActorStatus_ClearDrawableFlags(actor, 1);
        {
            void *control = actor->base.onDefeat;
            SceneActorTerrain_InstallControl(actor, control, 17);
        }
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        actor->base.hitOwners = 32;
        if ((!SceneActorDamage_TestFlags(gEngineFlags, 0x4800)) && (!Flag2D0(actor, 0x800)))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    } else {
        SetHitFlags(0x20, (void *)&actor->base.motionFlags);
        {
            void *control = actor->base.onReaction;
            SceneActorTerrain_InstallControl(actor, control, 0x12);
        }
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
    }
    {
        u32 mask = 0x2000;
        *flags2CC |= mask;
    }
    return 1;
fail:
    return 0;
}
