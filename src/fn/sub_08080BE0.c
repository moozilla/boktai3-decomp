#include "global.h"
#include "scene_actor_chase.h"

struct ReactionOwner {
    u8 unknown0[0x3C];
    struct SceneActorRegistryResetView *actor;
};
extern const s16 gUnk_086149C4[];
static inline u32 Reaction_MotionTest(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u32 Reaction_WordTest(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u8 Reaction_IsBlocked(struct SceneActorRegistryResetView *actor)
{
    if (!actor)
        return 0;
    if (Reaction_MotionTest(actor, 0x40))
        return 1;
    if (Reaction_WordTest(actor, 0x20000))
        return 1;
    if (actor->health <= 0)
        return 1;
    return 0;
}
static inline u8 Reaction_IsBlockedFlags(struct SceneActorRegistryResetView *actor, u32 flags)
{
    if (!actor)
        return 0;
    if (flags & 0x40)
        return 1;
    if (Reaction_WordTest(actor, 0x20000))
        return 1;
    if (actor->health <= 0)
        return 1;
    return 0;
}
static inline void Reaction_ClearHalf(u16 *field, s32 mask)
{
    *field &= mask;
}
static inline void Reaction_ClearWord(u32 *field, u32 bits)
{
    u32 mask = ~bits;
    *field &= mask;
}
static inline s32 Reaction_ScaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void Reaction_SetHalf(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2D6 = mask | actor->flags2D6;
}

u32 SceneActorReaction_SetMotionMode(struct ReactionOwner *owner, s32 mode, s32 amplitude)
{
    struct SceneActorRegistryResetView *actor = owner->actor;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactCollider *second = &actor->colliderB;
    struct ContactCollider *first = &actor->colliderA;
    if (Reaction_IsBlocked(actor))
        return 0;
    switch (mode) {
        case 0: {
            u16 motionMode = 0x8000;
            u16 *entryFlags = &actor->flags2D4;
            u16 active = *entryFlags & motionMode;
            u16 *motion = entryFlags;
            if (!active) {
                second->flags |= 4;
                first->flags |= 4;
                Chase_SetCtl(actor, group->field610, 0x1F);
                Chase_SetState(actor, 3, 0, 0);
                {
                    u32 updated = (*motion | motionMode) | (u16)(active & motionMode);
                    *motion = updated;
                    if (updated) {
                        if (Reaction_IsBlockedFlags(actor, updated)) {
                            u32 zero = 0;
                            *motion = 0x40;
                            actor->field2E6 = zero;
                            Reaction_ClearHalf(&actor->flags2D6, ~0x980);
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
                                Reaction_ClearWord(&actor->flags2CC, 0x4000);
                                {
                                    struct SceneActorReactionMotion *tracking =
                                        (void *)&actor->tracking.target;
                                    s32 mask = ~0x10;
                                    tracking->flags &= mask;
                                }
                                {
                                    u32 test = 0x20;
                                    if (!(test & *motion))
                                        Reaction_ClearWord(&actor->flags2CC, 0x80);
                                }
                                Reaction_ClearWord(&actor->flags2D0, 0x100004);
                                Reaction_SetHalf(actor, 0x400);
                                actor->field2E6 = 0;
                                {
                                    u32 mask = 0x2000;
                                    actor->flags2CC |= mask;
                                }
                            } else
                                Reaction_ClearWord(&actor->flags2CC, 0x2000);
                        }
                    } else
                        Reaction_ClearWord(&actor->flags2CC, 0x2000);
                }
            }
            actor->secondaryMotion.x = 0;
            actor->secondaryMotion.y = 0;
            actor->secondaryMotion.z = 0;
            first->responseParameter = 0;
            if (amplitude) {
                actor->reactionSoundVariant = amplitude;
                actor->field2E6 = 1;
                actor->field2E0 = 1;
                actor->field2DC = 2;
                actor->field2D8 = 1;
                Reaction_SetHalf(actor, 0x20);
            } else {
                actor->reactionSoundVariant = 0;
                actor->field2E6 = 0;
                actor->field2E0 = 0;
                actor->field2DC = 0;
                actor->field2D8 = 0;
            }
            break;
        }

        case 1:
            Reaction_ClearHalf(&second->flags, ~4);
            Reaction_ClearHalf(&first->flags, ~4);
            Reaction_ClearHalf(&actor->flags2D4, ~0x8000);
            {
                u8 *dir = &actor->body.direction;
                struct ContactPosition *motion = &actor->secondaryMotion;
                u8 direction = *dir;
                motion->x = Reaction_ScaleMotion(gUnk_086149C4[(direction + 64) & 255] * amplitude);
                motion->y = 0;
                motion->z = Reaction_ScaleMotion(gUnk_086149C4[direction] * amplitude);
            }
            actor->secondaryMotion.y = 4;
            Chase_SetState(actor, 3, 0, 0);
            Chase_SetCtl(actor, group->field614, 0x20);
            break;
    }

    return 1;
}
