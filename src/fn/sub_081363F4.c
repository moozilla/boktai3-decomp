#include "global.h"
#include "scene_actor_damage_visibility.h"
#include "scene_actor_reset.h"
#include "area_effect_link.h"
#include "render.h"
void SceneActor_ClearReactionFlags(void *);
u32 SceneActorBehavior_DispatchFlaggedCategory(struct ChaseActor *);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorBehavior_PlayReactionSound(void *);
void SceneActorTerrain_ClearCollisionTimers(void *);
void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void CpuSet(const void *, void *, u32);
struct DamageNumberSource;
s32 DamageNumberDisplay_Show(struct DamageNumberSource *, u32, u32, const struct ContactPosition *,
                             const struct ContactPosition *, s32);
void AnimData_SelectPaletteBank(void *, s32);

static inline u8 IsDown(struct ChaseActor *actor, u16 flags)
{
    if (actor) {
        {
            u32 mask = 0x40;
            if (mask & flags)
                return 1;
        }
        {
            u32 mask = 0x20000;
            if (actor->base.flags2D0 & mask)
                return 1;
        }
        if (actor->base.health <= 0)
            return 1;
    }
    return 0;
}
union PlacementHitFlags {
    u32 packed;
    struct {
        u16 motion, response;
    } half;
};

static inline void UpdateHitFlagsShared(struct ChaseActor *actor, u16 *flags2D4, u32 *flags2CC)
{
    union PlacementHitFlags *pair = (void *)flags2D4;
    u32 flags = pair->packed & 0xFFFF;
    if (flags) {
        u32 down = IsDown(actor, flags);
        if (down) {
            u32 zero = 0;
            *flags2D4 = 0x40;
            actor->base.field2E6 = zero;
            {
                u16 *flags2D6 = &actor->base.flags2D6;
                s32 mask = ~0x980;
                *flags2D6 &= mask;
            }
            return;
        }
        {
            u32 mask = 0x803E;
            flags = *flags2D4;
            if (!(mask & flags))
                goto clearShared;
        }
        {
            {
                union {
                    u32 raw;
                    struct {
                        s32 bits : 16;
                        u32 upper : 16;
                    } field;
                } mask;
                u32 value;
                mask.raw = flags;
                mask.field.bits = ~0x201;
                value = mask.field.bits & flags;
                *flags2D4 = value;
            }
            {
                u32 mask = ~0x4000;
                *flags2CC &= mask;
            }
            {
                struct SceneActorReactionMotion *motion = (void *)&actor->base.tracking.target;
                s32 mask = ~0x10;
                motion->flags &= mask;
            }
            {
                u32 test = 0x20;
                if (!(*flags2D4 & test)) {
                    u32 mask = ~0x80;
                    *flags2CC &= mask;
                }
            }
            {
                u32 *flags2D0 = &actor->base.flags2D0;
                u32 mask = 0xFFEFFFFB;
                *flags2D0 &= mask;
            }
            {
                u32 bit = 0x400;
                actor->base.flags2D6 |= bit;
            }
            actor->base.field2E6 = down;
            {
                u32 bit = 0x2000;
                *flags2CC |= bit;
            }
            return;
        }
    }
clearShared: {
    u32 mask = ~0x2000;
    *flags2CC &= mask;
}
}

struct PlacementFlagHalfword {
    s16 bits : 16;
};
static inline void PlacementDamage_SetHitFlags(s16 bit, struct PlacementFlagHalfword *field)
{
    field->bits = bit | field->bits;
}

static inline u8 PlacementDamage_TestField(u32 mask, const u16 *field)
{
    if (*field & mask)
        return 1;
    return 0;
}
static inline u32 PlacementDamage_FieldBits(u32 mask, const u16 *field)
{
    return *field & mask;
}

static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

// confidence=low: expanded scene-actor hit processing with route and collider cleanup.
u8 SceneActorReaction_ApplyLinkedAreaDamage(void *);
u8 SceneActorReaction_TryBeginControl29(void *);
void SceneActorPlacement_BeginDestinationJump(void *);
u8 SceneActorReaction_ApplyEscalatingHit(void *);
u32 SceneActorReaction_DefeatRecordStats(struct ChaseActor *);
void SceneActorContacts_ReleaseNode(void *);

struct PlacementStatusOwner {
    u32 *field;
};
struct PlacementMotionOwner {
    u16 *field;
};
static inline u32 PlacementDamage_QueryMotionSaveStatus(struct ChaseActor *actor, u32 mask,
                                                        u32 *status,
                                                        struct PlacementStatusOwner *owner,
                                                        struct PlacementMotionOwner *motionOwner)
{
    u16 *motion = &actor->base.flags2D4;
    u32 bits = *motion & mask;
    motionOwner->field = motion;
    owner->field = status;
    return bits;
}

static inline void ResetMotion(struct SceneActorRegistryControl *control)
{
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}

static inline u32 WordBits(u32 *field, u32 mask)
{
    return *field & mask;
}
static inline u32 HitFlag(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2D4 & mask;
}
static inline u8 HasPendingHitSave(struct ChaseActor *actor, u16 **out)
{
    u16 *address = &actor->base.field2E6;
    u32 value = *address;
    *out = address;
    if (!value)
        return 0;
    return 1;
}

static inline u32 PlacementDamage_MotionBits(u32 mask, u16 *flags2D4)
{
    return (*flags2D4) & mask;
}

static inline void PlacementDamage_ReportHud(struct ChaseActor *actor, u32 id, s32 health,
                                             u32 maximum, u32 kind, u32 mode, u32 parameter,
                                             u32 *damageFlags, s32 priority,
                                             struct ContactCollider **captured)
{
    u32 zero;
    struct ContactPosition offset;
    SceneHud_SetTimedState(id, health, maximum, kind, mode >> 2, parameter);
    Fill(&zero, &offset);
    if (actor->base.reactionSoundVariant || !(*damageFlags & 0x20000)) {
        struct ContactCollider *displayCollider = &actor->base.colliderA;
        DamageNumberDisplay_Show((void *)displayCollider, actor->base.reactionSoundVariant, 0,
                                 &actor->base.body.position, &offset, priority);
        *captured = displayCollider;
    }
}

static inline void PlacementDamage_UpdateCollider(struct ContactCollider *collider,
                                                  struct ChaseActor *actor, u16 *flags2D4)
{
    if (PlacementDamage_TestField(0x40, &actor->base.flags2C0))
        collider->flags &= ~0x100;
    else if ((actor->base.field2DC & 0x84000) || PlacementDamage_FieldBits(0x2016, flags2D4))
        collider->flags &= ~0x100;
    else
        collider->flags |= 0x100;
}

u8 SceneActorReaction_ResolvePlacementDamage(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u16 *pending;
    u16 *flags2D4;
    u32 *flags2CC;
    u32 *flags2D8;
    u32 mode;
    struct ContactCollider *collider;
    if (HitFlag(actor, 0x4000)) {
    fail:
        return 0;
    }
    if (!HasPendingHitSave(actor, &pending)) {
        if (!SceneActorReaction_ApplyLinkedAreaDamage(actor))
            goto fail;
        {
            struct PlacementStatusOwner saved;
            struct PlacementMotionOwner savedMotion;
            struct SceneActorRegistryControl *control = &group->common.control;
            u32 *flags = &actor->base.flags2CC;
            u32 mask = 0xFFFEBE3F;
            *flags &= mask;
            ResetMotion(control);
            {
                u32 bit = 0x1000;
                u32 bits;
                bits =
                    PlacementDamage_QueryMotionSaveStatus(actor, bit, flags, &saved, &savedMotion);
                flags2D4 = savedMotion.field;
                flags2CC = saved.field;
                if (bits) {
                    void **node = &actor->group.contactNode;
                    SceneActorContacts_ReleaseNode(*node);
                    *flags2D4 &= ~bit;
                    *node = (void *)0;
                }
            }
        }
        UpdateHitFlagsShared(actor, flags2D4, flags2CC);
        SceneActor_ClearReactionFlags(actor);
        return 1;
    }
    if ((u8)SceneActorBehavior_DispatchFlaggedCategory(actor))
        return 1;
    if (SceneActorReaction_TryBeginControl29(actor))
        return 1;
    SceneActor_ClearReactionFlags(actor);
    SceneActorMovement_ProjectProbe(actor, actor->base.colliderA.responseParameter,
                                    actor->base.colliderA.responseParameter);
    SceneActorPlacement_BeginDestinationJump(actor);
    {
        u32 damage = actor->base.reactionSoundVariant;
        s16 *health = &actor->base.health;
        *health -= damage;
    }
    {
        u32 applied = SceneActorReaction_ApplyEscalatingHit(actor);
        flags2D4 = &actor->base.flags2D4;
        if ((!applied) && (!(actor->base.field2D8 & 0x20000))) {
            PlacementDamage_SetHitFlags(0x20, (void *)flags2D4);
        }
    }
    {
        u32 bits = actor->base.field2E6 & 4;
        mode = 1;
        if (bits)
            mode = 4;
    }
    {
        u32 *damageFlags = &actor->base.field2D8;
        u32 blocked = *damageFlags & 0x20000;
        flags2CC = &actor->base.flags2CC;
        flags2D8 = damageFlags;
        collider = &actor->base.colliderA;
        if (!blocked) {
            if ((mode & 4) && (!PlacementDamage_MotionBits(4, flags2D4))) {
                SceneActorBehavior_PlayReactionSound(actor);
                *(u16 *)&actor->base.field30C = 12;
            }
            flags2CC = &actor->base.flags2CC;
            collider = &actor->base.colliderA;
            if (mode) {
                u16 bank = actor->base.paletteAlternateBank;
                if (actor->base.drawableKind == 0)
                    AnimData_SelectPaletteBank(((u8 *)actor->base.drawable) + 0x2C, bank);
                else
                    ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->palette =
                        gPaletteBankData + (bank * 32);
                {
                    u32 eight = 8;
                    actor->base.field1E8 = eight;
                }
                {
                    u32 strong = mode & 4;
                    flags2CC = &actor->base.flags2CC;
                    if (strong && (actor->base.category == 0x19)) {
                        u32 bit = 0x2000000;
                        *flags2CC |= bit;
                        {
                            u32 duration = 0x41;
                            actor->base.field1EA = duration;
                        }
                    }
                }
                {
                    u32 bit = 0x8000;
                    *flags2CC |= bit;
                }
                {
                    union {
                        struct {
                            s32 value : 16;
                        } part;
                        u32 raw;
                    } decision;
                    decision.raw = SceneActorDamage_ShouldShow(actor);
                    collider = &actor->base.colliderA;
                    if (decision.part.value) {
                        s32 h = actor->base.health;
                        s32 maxh = (u16)actor->base.defaultHealth;
                        s32 kind = (s16)actor->base.fieldB2;
                        s32 param = (s16)actor->base.field9A;
                        u32 id = actor->base.body.id;
                        s32 priority;
                        decision.part.value = 1;
                        priority = decision.part.value;
                        PlacementDamage_ReportHud(actor, id, h, maxh, kind, mode, param, flags2D8,
                                                  priority, &collider);
                    }
                }
            }
        }
    }
    {
        u32 test = 0x10;
        u32 flags = *flags2D4;
        if ((test & flags) || WordBits(flags2CC, 0x200))
            actor->base.fieldAA = 0;
        else if (flags & 6) {
            s16 *currentField = (s16 *)&actor->base.fieldAA;
            s16 *initialField = (s16 *)&actor->base.fieldA8;
            s32 current = *currentField;
            s32 initial = *initialField;
            if (current >= initial)
                actor->base.fieldAA = initial;
        }
    }
    {
        u32 bit = 0x80;
        u32 keep = bit & *flags2CC;
        struct SceneActorRegistryControl *control = &group->common.control;
        {
            u32 mask = 0xFFFEBE3F;
            *flags2CC &= mask;
        }
        ResetMotion(control);
        *flags2CC |= keep;
    }
    SceneActorReaction_DefeatRecordStats(actor);
    UpdateHitFlagsShared(actor, flags2D4, flags2CC);
    SceneActorTerrain_ClearCollisionTimers(actor);
    PlacementDamage_UpdateCollider(collider, actor, flags2D4);
    return 1;
}
