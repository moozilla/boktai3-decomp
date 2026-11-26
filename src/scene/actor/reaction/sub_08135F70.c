#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_contact_response.h"
#include "scene/actor/scene_actor_palette.h"
#include "scene/area_effect/area_effect_link.h"
#include "engine/video/render.h"
#include "scene/actor/scene_actor_damage_visibility.h"
#include "scene/actor/scene_actor_damage_flash.h"
u8 SceneActorReaction_ApplyAreaDamage(void *);
void SceneActor_ClearReactionFlags(void *);
u8 SceneActorBehavior_DispatchFlaggedCategory(void *);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorBehavior_PlayReactionSound(void *);
void SceneActorReaction_RecordDefeat(void *);

void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void AnimData_SelectPaletteBank(void *, s32);

static inline u8 HasPendingHit(struct ChaseActor *actor)
{
    if (actor->base.field2E6 == 0)
        return 0;
    return 1;
}

static inline u8 IsDownAtHealth(struct ChaseActor *actor, u16 flags, const s16 *health)
{
    if (actor) {
        {
            if (0x40 & flags)
                return 1;
        }
        {
            u32 mask = 0x20000;
            if (actor->base.statusFlags & mask)
                return 1;
        }
        if (*health <= 0)
            return 1;
    }
    return 0;
}

static inline void UpdateHitFlagsShared(struct ChaseActor *actor, u16 *flags2D4, u32 *flags2CC,
                                        const s16 *health)
{
    u32 flags = *flags2D4;
    if (flags) {
        u32 down = IsDownAtHealth(actor, flags, health);
        if (down) {
            SceneActorDamage_MarkDown(actor, flags2D4);
            return;
        }
        {
            u32 mask = 0x803E;
            flags = *flags2D4;
            if (!(mask & flags))
                goto clearShared;
        }
        {
            SceneActorDamage_ClearHitStart(flags2D4, flags);
            SceneActorDamage_AcceptHit(actor, flags2D4, flags2CC, down);
            return;
        }
    }
clearShared: {
    u32 mask = ~0x2000;
    *flags2CC &= mask;
}
}

u8 SceneActorReaction_ResolveDamage(struct ChaseActor *actor)
{
    s16 *savedHealth;
    struct {
        s16 *field;
    } healthOwner;
    u16 *flags2D4;
    u32 mode;
    u32 *flags2D8;
    u32 *flags2CC;
    u32 updated;
    u32 strong;
    if (!HasPendingHit(actor)) {
        if (SceneActorReaction_ApplyAreaDamage(actor)) {
            u16 *borrowed = &actor->base.motionFlags;
            u32 flags = *borrowed;
            flags2D4 = borrowed;
            // The clear is written in both places it applies. jump2 cross-jumps the
            // two copies into one, but reload has already loaded the 0x2CC offset for
            // each, which advances its spill rotation before IsDown.
            if (!flags) {
                u32 *flags2CC = &actor->base.behaviorFlags;
                u32 mask = ~0x2000;
                *flags2CC &= mask;
            } else {
                u32 down = SceneActorDamage_IsDown(actor, flags);
                if (down) {
                    SceneActorDamage_MarkDown(actor, flags2D4);
                } else {
                    u32 mask = 0x803E;
                    flags = *flags2D4;
                    if (mask & flags) {
                        u32 *flags2CC;
                        SceneActorDamage_ClearHitStart(flags2D4, flags);
                        flags2CC = &actor->base.behaviorFlags;
                        SceneActorDamage_AcceptHit(actor, flags2D4, flags2CC, down);
                    } else {
                        u32 *flags2CC = &actor->base.behaviorFlags;
                        u32 mask = ~0x2000;
                        *flags2CC &= mask;
                    }
                }
            }
            SceneActor_ClearReactionFlags(actor);
            return 1;
        }
        return 0;
    }
    if (SceneActorBehavior_DispatchFlaggedCategory(actor))
        return 1;
    SceneActor_ClearReactionFlags(actor);
    SceneActorMovement_ProjectProbe(actor, actor->base.colliderA.responseParameter,
                                    actor->base.colliderA.responseParameter);
    {
        u32 d = actor->base.reactionSoundVariant;
        s16 *health = &actor->base.health;
        *health -= d;
        healthOwner.field = health;
    }
    {
        u16 *motion;
        u16 previous;
        u32 marked = 0x20;
        motion = SceneActorContact_MotionField(actor);
        SCENE_ACTOR_CONTACT_READ_MOTION_FIELD(previous, motion);
        marked |= previous;
        *motion = marked;
        updated = marked;

        {
            u32 responseFlags = actor->base.field2E6 & 4u;
            mode = 1;
            flags2D4 = motion;
            savedHealth = healthOwner.field;
            if (responseFlags)
                mode = 4;
        }
    }
    {
        u32 *damageFlags = &actor->base.hitAttributes;
        u32 blocked = *damageFlags & 0x20000;
        flags2CC = &actor->base.behaviorFlags;
        flags2D8 = damageFlags;
        if (blocked)
            goto done;
    }
    {
        if ((mode & 4) && (!(updated & 4))) {
            SceneActorBehavior_PlayReactionSound(actor);
            *(u16 *)&actor->base.particleTimer = 12;
        }
        flags2CC = &actor->base.behaviorFlags;
        if (mode) {
            SceneActorDamage_Flash(actor);
            strong = mode & 4;
            flags2CC = &actor->base.behaviorFlags;
            if (strong && (actor->base.category == 0x19)) {
                SceneActorDamage_StartStrongPalette(actor, flags2CC);
            }
            SceneActorDamage_MarkFlashing(flags2CC);
            {
                union {
                    u32 raw;
                    struct {
                        s32 value : 16;
                    } part;
                } decision;
                decision.raw = SceneActorDamage_ShouldShow(actor);
                if (decision.part.value) {
                    struct ContactPosition offset;
                    s32 h = *savedHealth;
                    s32 maxh = (u16)actor->base.defaultHealth;
                    s32 kind = (s16)actor->base.hudKind;
                    s32 param = (s16)actor->base.hudLabel;
                    u32 id = actor->base.body.id;
                    s32 priority;
                    decision.part.value = 1;
                    priority = decision.part.value;
                    SceneHud_SetTimedState(id, h, maxh, kind, mode >> 2, param);
                    ZeroFill8(&offset);
                    if (actor->base.reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                        DamageNumberDisplay_Show(&actor->base.colliderA,
                                                 actor->base.reactionSoundVariant, 0,
                                                 &actor->base.body.position, &offset, priority);
                }
            }
        }
    }
done: {
    u32 keep = 0x80;
    u32 value = *flags2CC;
    keep &= value;
    *flags2CC = (value & 0xFFFEBE3F) | keep;
}

    SceneActorReaction_RecordDefeat(actor);
    UpdateHitFlagsShared(actor, flags2D4, flags2CC, savedHealth);
    SceneActorTerrain_ClearCollisionTimers(&actor->base);
    return 1;
}
