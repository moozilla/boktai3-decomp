#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_contact_response.h"

struct ReactionOwner {
    u8 unknown0[0x3C];
    struct ChaseActor *actor;
};
extern const s16 gUnk_086149C4[];
static inline u32 Reaction_WordTest(struct ChaseActor *actor, u32 mask)
{
    return actor->base.statusFlags & mask;
}
static inline u8 Reaction_IsBlocked(struct ChaseActor *actor)
{
    if (!actor)
        return 0;
    if (SceneActorContact_MotionFlags(actor, 0x40))
        return 1;
    if (Reaction_WordTest(actor, 0x20000))
        return 1;
    if (actor->base.health <= 0)
        return 1;
    return 0;
}
static inline u8 Reaction_IsBlockedFlags(struct ChaseActor *actor, u32 flags)
{
    if (!actor)
        return 0;
    if (flags & 0x40)
        return 1;
    if (Reaction_WordTest(actor, 0x20000))
        return 1;
    if (actor->base.health <= 0)
        return 1;
    return 0;
}
static inline void Reaction_ClearHalf(u16 *field, s32 mask)
{
    *field &= mask;
}
static inline s32 Reaction_ScaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void Reaction_SetHalf(struct ChaseActor *actor, u16 mask)
{
    actor->base.reactionFlags = mask | actor->base.reactionFlags;
}

u32 SceneActorReaction_SetMotionMode(struct ReactionOwner *owner, s32 mode, s32 amplitude)
{
    struct ChaseActor *actor = owner->actor;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactCollider *second = &actor->base.colliderB;
    struct ContactCollider *first = &actor->base.colliderA;
    if (Reaction_IsBlocked(actor))
        return 0;
    switch (mode) {
        case 0: {
            u16 motionMode = 0x8000;
            u16 *entryFlags = &actor->base.motionFlags;
            u16 active = *entryFlags & motionMode;
            u16 *motion = entryFlags;
            if (!active) {
                second->flags |= 4;
                first->flags |= 4;
                Chase_SetCtl(actor, group->heldControl, 0x1F);
                Chase_SetState(actor, 3, 0, 0);
                {
                    u32 updated = (*motion | motionMode) | (u16)(active & motionMode);
                    *motion = updated;
                    if (updated) {
                        if (Reaction_IsBlockedFlags(actor, updated)) {
                            u32 zero = 0;
                            *motion = 0x40;
                            actor->base.field2E6 = zero;
                            Reaction_ClearHalf(&actor->base.reactionFlags, ~0x980);
                        } else {
                            u32 statusMask = 0x803E;
                            u32 flags = *motion;
                            if (statusMask & flags) {
                                {
                                    union {
                                        u32 raw;
                                        struct {
                                            s16 bits : 16;
                                            u32 upper : 16;
                                        } field;
                                    } mask;
                                    u32 value;
                                    mask.raw = flags;
                                    mask.field.bits = ~0x201;
                                    value = mask.field.bits & flags;
                                    *motion = value;
                                }
                                SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x4000);
                                {
                                    struct SceneActorReactionMotion *tracking =
                                        (void *)&actor->base.tracking.target;
                                    s32 mask = ~0x10;
                                    tracking->flags &= mask;
                                }
                                {
                                    u32 test = 0x20;
                                    if (!(test & *motion))
                                        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags,
                                                                       0x80);
                                }
                                SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x100004);
                                Reaction_SetHalf(actor, 0x400);
                                actor->base.field2E6 = 0;
                                {
                                    u32 mask = 0x2000;
                                    actor->base.behaviorFlags |= mask;
                                }
                            } else
                                SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x2000);
                        }
                    } else
                        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x2000);
                }
            }
            actor->base.secondaryMotion.x = 0;
            actor->base.secondaryMotion.y = 0;
            actor->base.secondaryMotion.z = 0;
            first->responseParameter = 0;
            if (amplitude) {
                actor->base.reactionSoundVariant = amplitude;
                actor->base.field2E6 = 1;
                actor->base.hitOwners = 1;
                actor->base.hitReactionFlags = 2;
                actor->base.hitAttributes = 1;
                Reaction_SetHalf(actor, 0x20);
            } else {
                actor->base.reactionSoundVariant = 0;
                actor->base.field2E6 = 0;
                actor->base.hitOwners = 0;
                actor->base.hitReactionFlags = 0;
                actor->base.hitAttributes = 0;
            }
            break;
        }

        case 1:
            Reaction_ClearHalf(&second->flags, ~4);
            Reaction_ClearHalf(&first->flags, ~4);
            Reaction_ClearHalf(&actor->base.motionFlags, ~0x8000);
            {
                u8 *dir = &actor->base.body.direction;
                struct ContactPosition *motion = &actor->base.secondaryMotion;
                u8 direction = *dir;
                motion->x = Reaction_ScaleMotion(gUnk_086149C4[(direction + 64) & 255] * amplitude);
                motion->y = 0;
                motion->z = Reaction_ScaleMotion(gUnk_086149C4[direction] * amplitude);
            }
            actor->base.secondaryMotion.y = 4;
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, group->releaseControl, 0x20);
            break;
    }

    return 1;
}
