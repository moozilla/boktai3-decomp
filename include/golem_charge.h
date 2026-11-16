#ifndef GUARD_GOLEM_CHARGE_H
#define GUARD_GOLEM_CHARGE_H

#include "scene_actor_chase_reaction.h"

extern const u16 gUnk_08606130[];
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

/* The context-owned charge state spans 0x680..0x6A3. Its layout is
 * distinct from the canonical actor animation record at the same offsets. */
struct GolemChargeState {
    u16 mode;
    s16 speed;
    u16 durationSpeed, field686, field688, field68A;
    u8 quadrant, phase, routeDirection, settleFrames, savedDirection, motionDirection;
    u16 field692;
    struct ContactPosition routePosition, stopPosition;
};

struct GolemChargeContext {
    u8 unknown0[0x634];
    u16 pendingHits;
    u8 unknown636[0x680 - 0x636];
    struct GolemChargeState charge;
};

static inline void Golem_SetMotionFlags(struct ChaseActor *actor, u16 mask)
{
    actor->base.flags2D4 = mask | actor->base.flags2D4;
}

static inline u32 Golem_MirrorFlags(u8 mirror)
{
    return gUnk_08606130[mirror] | mirror;
}

static inline void Golem_GetDirectionVariant(struct ChaseActor *actor, u8 *variant, u8 *mirror)
{
    s32 quadrant = ((actor->base.body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = 0;
        *variant = quadrant;
    }
}

static inline void Golem_SelectAnimation(struct ChaseActor *actor, u32 selection, u32 variant,
                                         u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->base.drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->base.animationIndex = selection;
        drawable = actor->base.drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->base.restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->base.animationIndex;
        restart = &actor->base.restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                actor->base.drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

static inline void Golem_ApplyDuration(struct ChaseActor *actor)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (SceneActorChase_QueryMotionFlags(actor, mask)) {
        u16 *p = &actor->base.field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &actor->base.field2C2;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!actor->base.drawableKind)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

#endif
